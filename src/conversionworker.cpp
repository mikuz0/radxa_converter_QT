#include "conversionworker.h"
#include <QRegularExpression>
#include <QThread>
#include <QElapsedTimer>
#include <QApplication>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

ConversionWorker::ConversionWorker(SSHClient *sshClient, QObject *parent)
: QObject(parent)
, m_sshClient(sshClient)
, m_totalDuration(0)
, m_currentTime(0)
, m_lastSpeed(1.0)
, m_running(false)
, m_stopRequested(false)
{
}

ConversionWorker::~ConversionWorker()
{
    stop();
}

void ConversionWorker::start()
{
    if (!m_sshClient || !m_sshClient->isConnected()) {
        emit finished(false, "Нет подключения к Radxa", m_outputFile);
        return;
    }

    m_running = true;
    m_stopRequested = false;
    m_currentTime = 0;
    m_lastSpeed = 1.0;

    emit started();
    emit logMessage("==================================================");
    emit logMessage("ЗАПУСК КОНВЕРТАЦИИ");
    emit logMessage("==================================================");
    emit logMessage("Команда: " + m_command);

    // Получаем длительность видео если не установлена
    if (m_totalDuration <= 0) {
        QString cmd = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 '" + m_sourceFile + "'";
        QString output;
        if (m_sshClient->executeCommand(cmd, &output)) {
            m_totalDuration = output.trimmed().toDouble();
            emit logMessage("Длительность: " + QString::number(m_totalDuration, 'f', 2) + " сек");
        }
    }

    // Запускаем ffmpeg через SSH с принудительным выводом прогресса
    ssh_session session = m_sshClient->getSession();
    if (!session) {
        emit finished(false, "Ошибка SSH сессии", m_outputFile);
        return;
    }

    ssh_channel channel = ssh_channel_new(session);
    if (!channel) {
        emit finished(false, "Ошибка создания канала", m_outputFile);
        return;
    }

    int rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        ssh_channel_free(channel);
        emit finished(false, "Ошибка открытия канала", m_outputFile);
        return;
    }

    // Модифицируем команду для принудительного вывода прогресса
    QString fullCommand = "stdbuf -oL -eL " + m_command + " 2>&1";

    rc = ssh_channel_request_exec(channel, fullCommand.toStdString().c_str());
    if (rc != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        emit finished(false, "Ошибка выполнения команды", m_outputFile);
        return;
    }

    // Читаем вывод в реальном времени
    char buffer[4096];
    int nbytes;
    QString lineBuffer;

    while (!m_stopRequested && !ssh_channel_is_eof(channel)) {
        // Читаем из канала (stdout и stderr объединены через 2>&1)
        nbytes = ssh_channel_read_nonblocking(channel, buffer, sizeof(buffer) - 1, 0);
        if (nbytes > 0) {
            buffer[nbytes] = '\0';
            lineBuffer += QString::fromUtf8(buffer);

            // Обрабатываем по строкам
            int pos;
            while ((pos = lineBuffer.indexOf('\n')) != -1) {
                QString line = lineBuffer.left(pos).trimmed();
                lineBuffer = lineBuffer.mid(pos + 1);

                if (!line.isEmpty()) {
                    // Отправляем в лог
                    emit logMessage(line);

                    // Парсим прогресс
                    parseFFmpegOutput(line);

                    // Принудительно обрабатываем события Qt
                    QApplication::processEvents();
                }
            }
        } else if (nbytes < 0) {
            // Ошибка чтения
            break;
        }

        // Обновляем интерфейс каждые 50 мс
        QThread::msleep(50);
    }

    // Обрабатываем остаток буфера
    if (!lineBuffer.isEmpty()) {
        QStringList lines = lineBuffer.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            emit logMessage(line);
            parseFFmpegOutput(line);
            QApplication::processEvents();
        }
    }

    // Получаем статус выхода
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    int exit_status = ssh_channel_get_exit_status(channel);
    #pragma GCC diagnostic pop

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    if (m_stopRequested) {
        m_sshClient->executeCommand("pkill -f ffmpeg");
        emit finished(false, "Остановлено пользователем", m_outputFile);
    } else if (exit_status == 0) {
        emit finished(true, "Конвертация успешно завершена", m_outputFile);
    } else {
        emit finished(false, "Ошибка конвертации (код: " + QString::number(exit_status) + ")", m_outputFile);
    }

    m_running = false;
}
// end

void ConversionWorker::stop()
{
    if (m_running) {
        m_stopRequested = true;
        emit logMessage("■ Запрос остановки...");
    }
}
void ConversionWorker::parseFFmpegOutput(const QString &line)
{
    if (m_stopRequested) return;

    // Проверяем на ошибки аппаратного кодирования
    if (line.contains("Failed to init MPP context") ||
        line.contains("hevc_rkmpp") && line.contains("Error")) {

        emit logMessage("⚠ ОШИБКА АППАРАТНОГО КОДИРОВАНИЯ RKMPP");
    emit logMessage("   Возможные причины:");
    emit logMessage("   • Неподдерживаемое разрешение или параметры видео");
    emit logMessage("   • Отсутствуют права доступа к /dev/dri/");
    emit logMessage("   • Драйвер Rockchip MPP не загружен");
    emit logMessage("   • Видеокарта Radxa не поддерживает выбранные параметры");
    emit logMessage("   Рекомендация: попробуйте программное кодирование (libx265)");
        }

    bool progressUpdated = false;

    // Парсим время из строк типа "time=01:47:49.71"
    QRegularExpression timeRegex(R"(time=(\d{2}):(\d{2}):(\d{2})\.(\d{2}))");
    QRegularExpression timeFloatRegex(R"(time=(\d+\.?\d*))");

    QRegularExpressionMatch match;

    if (line.contains(timeRegex)) {
        match = timeRegex.match(line);
        if (match.hasMatch()) {
            int hours = match.captured(1).toInt();
            int minutes = match.captured(2).toInt();
            int seconds = match.captured(3).toInt();
            int milliseconds = match.captured(4).toInt();
            m_currentTime = hours * 3600 + minutes * 60 + seconds + milliseconds / 100.0;
            progressUpdated = true;
        }
    } else if (line.contains(timeFloatRegex)) {
        match = timeFloatRegex.match(line);
        if (match.hasMatch()) {
            m_currentTime = match.captured(1).toDouble();
            progressUpdated = true;
        }
    }

    // Парсим скорость из строк типа "speed= 274x"
    QRegularExpression speedRegex(R"(speed=\s*(\d+\.?\d*)x)");
    if (line.contains(speedRegex)) {
        match = speedRegex.match(line);
        if (match.hasMatch()) {
            m_lastSpeed = match.captured(1).toDouble();
        }
    }

    // Если прогресс обновился, сразу отправляем сигнал
    if (progressUpdated && m_totalDuration > 0) {
        updateProgress();
    }
}

//end
void ConversionWorker::updateProgress()
{
    if (m_totalDuration > 0 && m_currentTime > 0) {
        int percent = qMin(static_cast<int>((m_currentTime / m_totalDuration) * 100), 100);

        // Отправляем прогресс
        emit progress(percent, m_lastSpeed, m_currentTime, m_totalDuration, "Обработка...");

        // Для отладки можно раскомментировать:
        // qDebug() << "Progress:" << percent << "% Time:" << m_currentTime << "Speed:" << m_lastSpeed;
    }
}
