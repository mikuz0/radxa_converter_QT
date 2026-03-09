#include "sshclient.h"
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <fcntl.h>
#include <sys/stat.h>

SSHClient::SSHClient(QObject *parent)
: QObject(parent)
, m_session(nullptr)
, m_sftp(nullptr)
, m_connected(false)
{
    ssh_init();
}

SSHClient::~SSHClient()
{
    cleanupSftp();
    disconnect();
    ssh_finalize();
}

void SSHClient::connectToHost(const QString &host, const QString &user, const QString &password)
{
    m_session = ssh_new();
    if (!m_session) {
        emit connectionStatusChanged(false, "Failed to create SSH session");
        return;
    }

    ssh_options_set(m_session, SSH_OPTIONS_HOST, host.toStdString().c_str());
    ssh_options_set(m_session, SSH_OPTIONS_USER, user.toStdString().c_str());

    int port = 22;
    ssh_options_set(m_session, SSH_OPTIONS_PORT, &port);

    int timeout = 10;
    ssh_options_set(m_session, SSH_OPTIONS_TIMEOUT, &timeout);

    int rc = ssh_connect(m_session);
    if (rc != SSH_OK) {
        QString error = ssh_get_error(m_session);
        ssh_free(m_session);
        m_session = nullptr;
        emit connectionStatusChanged(false, "Connection failed: " + error);
        return;
    }

    rc = ssh_userauth_password(m_session, nullptr, password.toStdString().c_str());
    if (rc != SSH_AUTH_SUCCESS) {
        QString error = ssh_get_error(m_session);
        ssh_disconnect(m_session);
        ssh_free(m_session);
        m_session = nullptr;
        emit connectionStatusChanged(false, "Authentication failed: " + error);
        return;
    }

    m_connected = true;
    emit connectionStatusChanged(true, "Connected successfully");
}

void SSHClient::disconnect()
{
    cleanupSftp();
    if (m_session) {
        ssh_disconnect(m_session);
        ssh_free(m_session);
        m_session = nullptr;
    }
    m_connected = false;
    emit connectionStatusChanged(false, "Disconnected");
}

bool SSHClient::executeCommand(const QString &command, QString *output)
{
    if (!m_connected || !m_session) {
        return false;
    }

    ssh_channel channel = ssh_channel_new(m_session);
    if (!channel) return false;

    int rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        ssh_channel_free(channel);
        return false;
    }

    rc = ssh_channel_request_exec(channel, command.toStdString().c_str());
    if (rc != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return false;
    }

    if (output) {
        char buffer[256];
        int nbytes;
        while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
            output->append(QByteArray(buffer, nbytes));
        }
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return true;
}

bool SSHClient::executeCommandWithChannel(const QString &command,
                                          std::function<void(const QString&)> outputHandler)
{
    if (!m_connected || !m_session) {
        return false;
    }

    ssh_channel channel = ssh_channel_new(m_session);
    if (!channel) return false;

    int rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        ssh_channel_free(channel);
        return false;
    }

    rc = ssh_channel_request_exec(channel, command.toStdString().c_str());
    if (rc != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return false;
    }

    char buffer[256];
    int nbytes;
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        if (outputHandler) {
            outputHandler(QString::fromUtf8(buffer, nbytes));
        }
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return true;
}

bool SSHClient::initSftp()
{
    if (m_sftp) {
        return true;
    }

    if (!m_session) {
        return false;
    }

    m_sftp = sftp_new(m_session);
    if (!m_sftp) {
        emit logMessage("Ошибка создания SFTP сессии: " + QString(ssh_get_error(m_session)));
        return false;
    }

    if (sftp_init(m_sftp) != SSH_OK) {
        emit logMessage("Ошибка инициализации SFTP: " + QString(ssh_get_error(m_session)));
        sftp_free(m_sftp);
        m_sftp = nullptr;
        return false;
    }

    return true;
}

void SSHClient::cleanupSftp()
{
    if (m_sftp) {
        sftp_free(m_sftp);
        m_sftp = nullptr;
    }
}

QString SSHClient::getHomeDirectory()
{
    if (!m_connected || !m_session) {
        return "/home/rock";
    }

    QString output;
    if (executeCommand("echo $HOME", &output)) {
        QString home = output.trimmed();
        if (!home.isEmpty()) {
            return home;
        }
    }

    if (executeCommand("pwd", &output)) {
        QString pwd = output.trimmed();
        if (!pwd.isEmpty()) {
            return pwd;
        }
    }

    return "/home/rock";
}

bool SSHClient::uploadFile(const QString &localPath, const QString &remotePath,
                           std::function<void(int)> progressCallback)
{
    if (!m_connected || !m_session) {
        emit logMessage("Ошибка: нет подключения к SSH");
        return false;
    }

    if (!initSftp()) {
        return false;
    }

    QFile localFile(localPath);
    if (!localFile.open(QIODevice::ReadOnly)) {
        emit logMessage("Ошибка открытия локального файла: " + localPath);
        return false;
    }

    qint64 fileSize = localFile.size();

    // Создаем удаленный файл
    int accessType = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file remoteFile = sftp_open(m_sftp, remotePath.toStdString().c_str(),
                                     accessType, S_IRUSR | S_IWUSR);

    if (!remoteFile) {
        emit logMessage("Ошибка создания удаленного файла: " + QString(ssh_get_error(m_sftp)));
        localFile.close();
        return false;
    }

    // Читаем локальный файл и пишем в удаленный
    char buffer[16384];
    qint64 totalWritten = 0;
    int bytesRead;

    while ((bytesRead = localFile.read(buffer, sizeof(buffer))) > 0) {
        int bytesWritten = sftp_write(remoteFile, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            emit logMessage("Ошибка записи в удаленный файл");
            sftp_close(remoteFile);
            localFile.close();
            return false;
        }

        totalWritten += bytesWritten;

        if (progressCallback && fileSize > 0) {
            int progress = static_cast<int>((totalWritten * 100) / fileSize);
            progressCallback(progress);
        }
    }

    sftp_close(remoteFile);
    localFile.close();

    emit logMessage("✓ Файл загружен: " + QFileInfo(localPath).fileName() +
    " -> " + remotePath);
    return true;
}

bool SSHClient::downloadFile(const QString &remotePath, const QString &localPath,
                             std::function<void(int)> progressCallback)
{
    if (!m_connected || !m_session) {
        emit logMessage("Ошибка: нет подключения к SSH");
        return false;
    }

    if (!initSftp()) {
        return false;
    }

    sftp_attributes attributes = sftp_stat(m_sftp, remotePath.toStdString().c_str());
    if (!attributes) {
        emit logMessage("Ошибка получения информации о файле: " + remotePath);
        return false;
    }

    qint64 fileSize = attributes->size;
    sftp_attributes_free(attributes);

    sftp_file remoteFile = sftp_open(m_sftp, remotePath.toStdString().c_str(), O_RDONLY, 0);
    if (!remoteFile) {
        emit logMessage("Ошибка открытия удаленного файла: " + QString(ssh_get_error(m_sftp)));
        return false;
    }

    QFile localFile(localPath);
    if (!localFile.open(QIODevice::WriteOnly)) {
        emit logMessage("Ошибка создания локального файла: " + localPath);
        sftp_close(remoteFile);
        return false;
    }

    char buffer[16384];
    qint64 totalRead = 0;
    int bytesRead;

    while ((bytesRead = sftp_read(remoteFile, buffer, sizeof(buffer))) > 0) {
        qint64 bytesWritten = localFile.write(buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            emit logMessage("Ошибка записи в локальный файл");
            sftp_close(remoteFile);
            localFile.close();
            return false;
        }

        totalRead += bytesWritten;

        if (progressCallback && fileSize > 0) {
            int progress = static_cast<int>((totalRead * 100) / fileSize);
            progressCallback(progress);
        }
    }

    sftp_close(remoteFile);
    localFile.close();

    emit logMessage("✓ Файл скачан: " + remotePath + " -> " +
    QFileInfo(localPath).fileName());
    return true;
}

bool SSHClient::createDirectory(const QString &path)
{
    if (!m_connected || !m_session) {
        return false;
    }

    if (!initSftp()) {
        return false;
    }

    int rc = sftp_mkdir(m_sftp, path.toStdString().c_str(), S_IRWXU);
    if (rc != SSH_OK) {
        emit logMessage("Ошибка создания директории: " + QString(ssh_get_error(m_sftp)));
        return false;
    }

    emit logMessage("✓ Директория создана: " + path);
    return true;
}

bool SSHClient::removeFile(const QString &path)
{
    if (!m_connected || !m_session) {
        return false;
    }

    if (!initSftp()) {
        return false;
    }

    int rc = sftp_unlink(m_sftp, path.toStdString().c_str());
    if (rc != SSH_OK) {
        emit logMessage("Ошибка удаления файла: " + QString(ssh_get_error(m_sftp)));
        return false;
    }

    emit logMessage("✓ Файл удален: " + path);
    return true;
}

bool SSHClient::fileExists(const QString &path)
{
    if (!m_connected || !m_session || !initSftp()) {
        return false;
    }

    sftp_attributes attributes = sftp_stat(m_sftp, path.toStdString().c_str());
    if (attributes) {
        sftp_attributes_free(attributes);
        return true;
    }
    return false;
}

bool SSHClient::getFileInfo(const QString &path, QString &info)
{
    if (!m_connected || !m_session) {
        return false;
    }

    QString cmd = "ffprobe -v quiet -print_format json -show_format -show_streams '" + path + "'";
    return executeCommand(cmd, &info);
}

bool SSHClient::checkFreeSpace(const QString &path, qint64 requiredSize, QString &available)
{
    if (!m_connected || !m_session) {
        return false;
    }

    QString cmd = "df -B1 '" + path + "' | tail -1 | awk '{print $4}'";
    QString output;
    if (executeCommand(cmd, &output)) {
        qint64 freeBytes = output.trimmed().toLongLong();
        if (freeBytes > 0) {
            double freeGB = freeBytes / (1024.0 * 1024.0 * 1024.0);
            double requiredGB = requiredSize / (1024.0 * 1024.0 * 1024.0);
            available = QString("%1 GB").arg(freeGB, 0, 'f', 2);

            if (freeBytes < requiredSize) {
                return false;  // Не хватает места
            }
            return true;  // Места достаточно
        }
    }
    return false;
}
