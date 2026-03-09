#include "mainwindow.h"
#include "sshclient.h"
#include "settingsmanager.h"
#include "filemanagerdialog.h"
#include "conversionworker.h"
#include "batchqueuedialog.h"
#include "profilesdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>
#include <QTime>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStyle>
#include <QThread>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QJsonDocument>    // Добавить
#include <QJsonObject>      // Добавить
#include <QJsonArray>       // Добавить
#include <QJsonValue>       // Добавить
#include <QProgressDialog>  // Добавить эту строку
#include <QKeyEvent>  // Добавить в начало файла, если нет

// ... остальной код mainwindow.cpp (очень длинный, оставляем как есть)

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_settings(new SettingsManager(this))
, m_sshClient(std::make_unique<SSHClient>(this))
, m_conversionThread(nullptr)
, m_conversionWorker(nullptr)
, m_fileManagerDialog(nullptr)
, m_batchQueueDialog(nullptr)
, m_profilesDialog(nullptr)
, m_conversionActive(false)
, m_batchMode(false)
, m_batchStopRequested(false)
, m_currentBatchIndex(-1)
, m_connectionTimer(new QTimer(this))  // Добавить эту строку
, m_connectionSpeed(0)
{
    setupUI();           // 1. Создаем UI (поля ввода уже существуют)
    // Скрываем лог сразу после создания UI
    m_logFrame->setVisible(false);  // <--- ДОБАВИТЬ ЗДЕСЬ
    createMenu();        // 2. Создаем меню
    loadSettings();      // 3. Загружаем настройки и применяем к полям
    initConnections();   // 4. Подключаем сигналы
    //initDefaultValues(); // 5. Устанавливаем значения по умолчанию (если настройки не загрузились)
    // Подключаем таймер
    connect(m_connectionTimer, &QTimer::timeout, this, &MainWindow::updateConnectionInfo);
    statusBar()->showMessage("Готов к работе");
}

MainWindow::~MainWindow()
{
    if (m_conversionThread) {
        m_conversionThread->quit();
        m_conversionThread->wait();
        delete m_conversionThread;
        m_conversionThread = nullptr;
    }
    m_connectionTimer = new QTimer(this);
    connect(m_connectionTimer, &QTimer::timeout, this, &MainWindow::updateConnectionInfo);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_sshClient && m_sshClient->isConnected()) {
        m_sshClient->disconnect();
    }
    saveSettings();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            if (!filePath.isEmpty()) {
                m_sourceFileEdit->setText(filePath);
                updateFileNamePreview();
            }
        }
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Radxa ROCK 4D - Удаленный перекодировщик видео");
    resize(900, 850);
    setMinimumSize(800, 700);
    setAcceptDrops(true);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Connection Frame
    QGroupBox *connGroup = new QGroupBox("Подключение к Radxa");
    QHBoxLayout *connLayout = new QHBoxLayout(connGroup);
    connLayout->setContentsMargins(5, 5, 5, 5);  // Уменьшаем внутренние отступы (было больше)
    connLayout->setSpacing(5);                    // Оставляем нормальное расстояние между элементами
    connLayout->addWidget(new QLabel("IP:"));
    m_ipEdit = new QLineEdit;
    m_ipEdit->setFixedWidth(120);
    connLayout->addWidget(m_ipEdit);

    connLayout->addWidget(new QLabel("Пользователь:"));
    m_userEdit = new QLineEdit;
    m_userEdit->setFixedWidth(100);
    connLayout->addWidget(m_userEdit);

    connLayout->addWidget(new QLabel("Пароль:"));
    m_passEdit = new QLineEdit;
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setFixedWidth(100);
    connLayout->addWidget(m_passEdit);

    m_connectBtn = new QPushButton("Подключиться");
    connLayout->addWidget(m_connectBtn);

    m_disconnectBtn = new QPushButton("Отключиться");
    m_disconnectBtn->setEnabled(false);
    connLayout->addWidget(m_disconnectBtn);

    m_statusLabel = new QLabel("● Отключено");
    m_statusLabel->setStyleSheet("color: red;");
    connLayout->addWidget(m_statusLabel);

    connLayout->addStretch();
    mainLayout->addWidget(connGroup);

    // File Selection Frame
    QGroupBox *fileGroup = new QGroupBox("Файлы");
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_fileManagerBtn = new QPushButton("📁 Файловый менеджер");
    m_fileManagerBtn->setEnabled(false);
    btnLayout->addWidget(m_fileManagerBtn);

    m_fileInfoBtn = new QPushButton("ℹ Информация");
    m_fileInfoBtn->setEnabled(false);
    btnLayout->addWidget(m_fileInfoBtn);

    btnLayout->addStretch();
    fileLayout->addLayout(btnLayout);

    QGridLayout *infoLayout = new QGridLayout;
    infoLayout->addWidget(new QLabel("Исходный файл:"), 0, 0);
    m_sourceFileEdit = new QLineEdit;
    m_sourceFileEdit->setReadOnly(true);
    infoLayout->addWidget(m_sourceFileEdit, 0, 1);

    m_queueInfoLabel = new QLabel("Очередь: 0 файлов");
    m_queueInfoLabel->setStyleSheet("color: gray;");
    infoLayout->addWidget(m_queueInfoLabel, 0, 2);

    QHBoxLayout *resultLayout = new QHBoxLayout;
    resultLayout->addWidget(new QLabel("Папка результата:"));
    m_outputDir = "/home/rock/converted";

    m_outputDirLabel = new QLabel(m_outputDir);
    m_outputDirLabel->setStyleSheet("color: black;");  // Обычный текст
    m_outputDirLabel->setCursor(Qt::ArrowCursor);      // Обычный курсор

    resultLayout->addWidget(m_outputDirLabel);
    resultLayout->addStretch();

    infoLayout->addLayout(resultLayout, 1, 0, 1, 3);
    fileLayout->addLayout(infoLayout);
    mainLayout->addWidget(fileGroup);

    // Info Frame
    QGroupBox *infoGroup = new QGroupBox("Информация о процессе");
    QVBoxLayout *infoGroupLayout = new QVBoxLayout(infoGroup);

    QGridLayout *infoGrid = new QGridLayout;

    infoGrid->addWidget(new QLabel("Скорость:"), 0, 0);
    m_speedValue = new QLabel("--");
    infoGrid->addWidget(m_speedValue, 0, 1);

    infoGrid->addWidget(new QLabel("Длительность:"), 0, 2);
    m_totalTimeValue = new QLabel("--");
    infoGrid->addWidget(m_totalTimeValue, 0, 3);

    infoGrid->addWidget(new QLabel("Текущее:"), 1, 0);
    m_currentTimeValue = new QLabel("--");
    infoGrid->addWidget(m_currentTimeValue, 1, 1);

    infoGrid->addWidget(new QLabel("Осталось (ETA):"), 1, 2);
    m_etaValue = new QLabel("--");
    infoGrid->addWidget(m_etaValue, 1, 3);

    infoGrid->addWidget(new QLabel("Прогресс:"), 2, 0);
    m_percentValue = new QLabel("0%");
    infoGrid->addWidget(m_percentValue, 2, 1);

    infoGrid->addWidget(new QLabel("Статус:"), 2, 2);
    m_statusValue = new QLabel("Ожидание");
    infoGrid->addWidget(m_statusValue, 2, 3);

    m_fileSizeEstimateLabel = new QLabel("");
    infoGrid->addWidget(m_fileSizeEstimateLabel, 2, 4);

    infoGroupLayout->addLayout(infoGrid);

    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    infoGroupLayout->addWidget(m_progressBar);

    m_showLogCheck = new QCheckBox("Показать логи");
    m_showLogCheck->setChecked(false);
    infoGroupLayout->addWidget(m_showLogCheck);

    mainLayout->addWidget(infoGroup);
// begin


        // Encoder Frame
        QGroupBox *encGroup = new QGroupBox("Параметры кодирования");
        QVBoxLayout *encLayout = new QVBoxLayout(encGroup);
        encLayout->setSpacing(5);

        m_noVideoCheck = new QCheckBox("Без видео (только аудио)");
        m_noVideoCheck->setStyleSheet("font-weight: bold;");
        encLayout->addWidget(m_noVideoCheck);

        // Video Group - УВЕЛИЧИВАЕМ ВЫСОТУ
        m_videoGroup = new QGroupBox("Видео");
        m_videoGroup->setMinimumHeight(80);  // Минимальная высота
        QGridLayout *videoLayout = new QGridLayout(m_videoGroup);
        videoLayout->setContentsMargins(5, 10, 5, 10);  // Увеличиваем отступы внутри

        videoLayout->addWidget(new QLabel("Кодек:"), 0, 0);
        m_codecCombo = new QComboBox;
        m_codecCombo->addItems({"hevc_rkmpp", "h264_rkmpp", "libx265", "libx264", "copy"});
        m_codecCombo->setMinimumHeight(25);  // Высота комбобокса
        videoLayout->addWidget(m_codecCombo, 0, 1);

        videoLayout->addWidget(new QLabel("Разрешение:"), 0, 2);
        m_resolutionCombo = new QComboBox;
        m_resolutionCombo->addItems({"", "3840x2160", "1920x1080", "1280x720", "854x480"});
        m_resolutionCombo->setEditable(true);
        m_resolutionCombo->setMinimumHeight(25);
        videoLayout->addWidget(m_resolutionCombo, 0, 3);

        videoLayout->addWidget(new QLabel("Битрейт:"), 0, 4);
        m_videoBitrateCombo = new QComboBox;
        m_videoBitrateCombo->addItems({"500k", "1M", "2M", "3M", "4M", "5M", "8M", "10M"});
        m_videoBitrateCombo->setEditable(true);
        m_videoBitrateCombo->setMinimumHeight(25);
        videoLayout->addWidget(m_videoBitrateCombo, 0, 5);

        encLayout->addWidget(m_videoGroup);

        // Audio Group - УВЕЛИЧИВАЕМ ВЫСОТУ
        QGroupBox *audioGroup = new QGroupBox("Аудио");
        audioGroup->setMinimumHeight(120);  // Минимальная высота
        QGridLayout *audioLayout = new QGridLayout(audioGroup);
        audioLayout->setContentsMargins(5, 10, 5, 10);  // Увеличиваем отступы внутри
        audioLayout->setVerticalSpacing(8);  // Увеличиваем вертикальные отступы

        audioLayout->addWidget(new QLabel("Кодек:"), 0, 0);
        m_audioCodecCombo = new QComboBox;
        m_audioCodecCombo->addItems({"copy", "aac", "mp3", "ac3", "flac", "opus", "libvorbis"});
        m_audioCodecCombo->setMinimumHeight(25);
        audioLayout->addWidget(m_audioCodecCombo, 0, 1);

        audioLayout->addWidget(new QLabel("Битрейт:"), 0, 2);
        m_audioBitrateCombo = new QComboBox;
        m_audioBitrateCombo->addItems({"", "64k", "96k", "128k", "160k", "192k", "256k", "320k"});
        m_audioBitrateCombo->setEditable(true);
        m_audioBitrateCombo->setMinimumHeight(25);
        audioLayout->addWidget(m_audioBitrateCombo, 0, 3);

        audioLayout->addWidget(new QLabel("Частота (Hz):"), 0, 4);
        m_audioSampleRateCombo = new QComboBox;
        m_audioSampleRateCombo->addItems({"", "8000", "11025", "16000", "22050", "32000", "44100", "48000", "96000"});
        m_audioSampleRateCombo->setEditable(true);
        m_audioSampleRateCombo->setMinimumHeight(25);
        audioLayout->addWidget(m_audioSampleRateCombo, 0, 5);

        audioLayout->addWidget(new QLabel("Каналы:"), 1, 0);
        m_audioChannelsCombo = new QComboBox;
        m_audioChannelsCombo->addItems({"", "1", "2", "6"});
        m_audioChannelsCombo->setMinimumHeight(25);
        audioLayout->addWidget(m_audioChannelsCombo, 1, 1);
        audioLayout->addWidget(new QLabel(""), 1, 2, 1, 4);  // Пустой виджет, занимающий 4 колонки

        encLayout->addWidget(audioGroup);



// end
    // Output filename
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setContentsMargins(0, 10, 0, 0);  // Добавляем отступ сверху 10 пикселей
    nameLayout->addWidget(new QLabel("Шаблон имени:"));
    m_outputFilenameEdit = new QLineEdit("{name}_{codec}_{res}.mp4");
    nameLayout->addWidget(m_outputFilenameEdit);

    m_previewLabel = new QLabel;
    m_previewLabel->setStyleSheet("color: gray; font-size: 8pt;");
    nameLayout->addWidget(m_previewLabel);

    m_estimateSizeBtn = new QPushButton("Оценить размер");
    nameLayout->addWidget(m_estimateSizeBtn);

    encLayout->addLayout(nameLayout);

    mainLayout->addWidget(encGroup);

    // Control buttons
    QHBoxLayout *ctrlLayout = new QHBoxLayout;

    m_startBtn = new QPushButton("▶ Начать перекодирование");
    m_startBtn->setEnabled(false);
    m_startBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    ctrlLayout->addWidget(m_startBtn);

    m_stopBtn = new QPushButton("■ Остановить");
    m_stopBtn->setEnabled(false);
    m_stopBtn->setStyleSheet("background-color: #f44336; color: white;");
    ctrlLayout->addWidget(m_stopBtn);

    m_batchBtn = new QPushButton("📋 Очередь");
    m_batchBtn->setStyleSheet("background-color: #FF9800; color: white;");
    ctrlLayout->addWidget(m_batchBtn);

    m_downloadBtn = new QPushButton("⬇ Скачать результат");
    m_downloadBtn->setEnabled(false);
    ctrlLayout->addWidget(m_downloadBtn);

    ctrlLayout->addStretch();
    mainLayout->addLayout(ctrlLayout);

    // Log area
    m_logFrame = new QWidget;
    QVBoxLayout *logLayout = new QVBoxLayout(m_logFrame);
    logLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *logGroup = new QGroupBox("Лог выполнения");
    QVBoxLayout *logGroupLayout = new QVBoxLayout(logGroup);

    m_logArea = new QTextEdit;
    m_logArea->setReadOnly(true);
    m_logArea->document()->setMaximumBlockCount(1000);
    logGroupLayout->addWidget(m_logArea);

    logLayout->addWidget(logGroup);
    mainLayout->addWidget(m_logFrame, 1);  // Фактор растяжения 1

    // Добавляем дополнительное растягивающееся пространство в конец
    mainLayout->addStretch(0);  // Растягивающееся пространство с фактором 0 (не будет расти)

}

void MainWindow::createMenu()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Меню "Файл"
    QMenu *fileMenu = menuBar->addMenu("Файл");
    fileMenu->addAction("Настройки подключения...", this, &MainWindow::showConnectionSettings);
    fileMenu->addSeparator();
    fileMenu->addAction("Сохранить профиль...", this, &MainWindow::saveProfile);
    fileMenu->addAction("Загрузить профиль...", this, &MainWindow::showProfilesDialog);
    fileMenu->addSeparator();
    fileMenu->addAction("Сохранить лог...", this, &MainWindow::saveLogToFile);
    fileMenu->addSeparator();
    fileMenu->addAction("Выход", this, &QWidget::close);

    // Меню "Подключение"
    QMenu *connMenu = menuBar->addMenu("Подключение");
    connMenu->addAction("Подключиться", this, &MainWindow::onConnectClicked);
    connMenu->addAction("Отключиться", this, &MainWindow::onDisconnectClicked);
    connMenu->addSeparator();
    connMenu->addAction("Сохранить настройки", this, &MainWindow::saveSettings);
    connMenu->addAction("Сбросить настройки", this, &MainWindow::resetSettings);

    // Меню "Очередь"
    QMenu *queueMenu = menuBar->addMenu("Очередь");
    queueMenu->addAction("Показать очередь", this, &MainWindow::showBatchQueue);
    queueMenu->addAction("Очистить очередь", this, &MainWindow::clearBatchQueue);
    queueMenu->addSeparator();
    queueMenu->addAction("Запустить обработку", this, &MainWindow::startBatchConversion);
    queueMenu->addAction("Остановить обработку", this, &MainWindow::stopBatchConversion);

    // Меню "Вид"
    QMenu *viewMenu = menuBar->addMenu("Вид");
    viewMenu->addAction("Файловый менеджер", this, &MainWindow::showFileManager);
    viewMenu->addAction("Очередь", this, &MainWindow::showBatchQueue);
    viewMenu->addSeparator();
    viewMenu->addAction("Очистить лог", this, &MainWindow::clearLog);

    // Меню "Помощь"
    QMenu *helpMenu = menuBar->addMenu("Помощь");
    helpMenu->addAction("О программе", this, &MainWindow::showAbout);
    helpMenu->addAction("Справка (F1)", this, &MainWindow::showHelp);

    // Добавляем горячие клавиши (отдельные действия, не в меню)
    QAction *openAction = new QAction("Открыть файловый менеджер", this);
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, &QAction::triggered, this, &MainWindow::showFileManager);
    addAction(openAction);  // Добавляем действие в окно

    QAction *startAction = new QAction("Старт", this);
    startAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(startAction, &QAction::triggered, this, &MainWindow::startConversion);
    addAction(startAction);

    QAction *quitAction = new QAction("Выход", this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    addAction(quitAction);

    // Добавляем действие для справки (Ctrl+H как альтернатива F1)
    QAction *helpAction = new QAction("Справка", this);
    helpAction->setShortcut(QKeySequence("Ctrl+H"));
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    addAction(helpAction);
}


void MainWindow::initConnections()
{
    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(m_fileManagerBtn, &QPushButton::clicked, this, &MainWindow::showFileManager);
    connect(m_fileInfoBtn, &QPushButton::clicked, this, &MainWindow::showFileInfo);
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::startConversion);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::stopConversion);
    connect(m_batchBtn, &QPushButton::clicked, this, &MainWindow::showBatchQueue);
    connect(m_downloadBtn, &QPushButton::clicked, [this](){ downloadFile(m_currentOutputFile); });
    connect(m_estimateSizeBtn, &QPushButton::clicked, this, &MainWindow::estimateFileSize);

    connect(m_noVideoCheck, &QCheckBox::toggled, this, &MainWindow::onNoVideoChanged);
    connect(m_codecCombo, &QComboBox::currentTextChanged, this, &MainWindow::onCodecChanged);
    connect(m_resolutionCombo, &QComboBox::currentTextChanged, this, &MainWindow::updateFileNamePreview);
    connect(m_outputFilenameEdit, &QLineEdit::textChanged, this, &MainWindow::updateFileNamePreview);
    connect(m_showLogCheck, &QCheckBox::toggled, this, &MainWindow::toggleLogVisibility);

    connect(m_sshClient.get(), &SSHClient::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(m_sshClient.get(), &SSHClient::logMessage,
            this, &MainWindow::appendLog);
}

void MainWindow::initDefaultValues()
{
    m_ipEdit->setText("192.168.1.100");
    m_userEdit->setText("rock");
    m_outputDir = "/home/rock/converted";
    m_outputDirLabel->setText(m_outputDir);
    onNoVideoChanged(false);
    updateFileNamePreview();
    // Скрываем лог при запуске
    m_logFrame->setVisible(false);
}

void MainWindow::onConnectClicked()
{
    if (!m_sshClient->isConnected()) {
        m_connectBtn->setEnabled(false);
        m_sshClient->connectToHost(m_ipEdit->text(), m_userEdit->text(), m_passEdit->text());
        appendLog("Подключение к " + m_ipEdit->text() + "...");
    }
}

void MainWindow::onDisconnectClicked()
{
    m_sshClient->disconnect();
}

void MainWindow::onConnectionStatusChanged(bool connected, const QString &message)
{
    if (connected) {
        m_statusLabel->setText("● Подключено");
        m_statusLabel->setStyleSheet("color: green;");
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        m_fileManagerBtn->setEnabled(true);
        m_fileInfoBtn->setEnabled(true);
        m_startBtn->setEnabled(true);
        appendLog("✓ Подключение установлено");
        m_connectionTimer->start(5000); // Обновление каждые 5 секунд
        updateConnectionInfo();
    } else {
        m_statusLabel->setText("● Отключено");
        m_statusLabel->setStyleSheet("color: red;");
        m_connectBtn->setEnabled(true);
        m_disconnectBtn->setEnabled(false);
        m_fileManagerBtn->setEnabled(false);
        m_fileInfoBtn->setEnabled(false);
        m_startBtn->setEnabled(false);
        if (!message.isEmpty()) {
            appendLog("✗ " + message);
        }
        m_connectionTimer->stop();
        statusBar()->showMessage("Отключено");
    }
}

void MainWindow::showFileManager()
{
    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    // Если диалог уже существует и не закрыт, просто показываем его
    if (m_fileManagerDialog) {
        if (m_fileManagerDialog->isVisible()) {
            m_fileManagerDialog->raise();
            m_fileManagerDialog->activateWindow();
        } else {
            delete m_fileManagerDialog;
            m_fileManagerDialog = nullptr;
        }
    }

    // Создаем новый диалог с сохраненной папкой результата
    if (!m_fileManagerDialog) {
        // Передаем сохраненную папку результата как начальный путь
        m_fileManagerDialog = new FileManagerDialog(m_sshClient.get(), this, m_outputDir);
        m_fileManagerDialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(m_fileManagerDialog, &FileManagerDialog::fileSelected,
                this, &MainWindow::onFileSelected);
        connect(m_fileManagerDialog, &FileManagerDialog::directorySelected,
                this, &MainWindow::onDirectorySelected);
        connect(m_fileManagerDialog, &FileManagerDialog::filesAddedToBatch,
                this, &MainWindow::onFilesAddedToBatch);

        // Очищаем указатель при закрытии
        connect(m_fileManagerDialog, &QDialog::finished, [this]() {
            m_fileManagerDialog = nullptr;
        });
    }

    m_fileManagerDialog->show();
    m_fileManagerDialog->raise();
    m_fileManagerDialog->activateWindow();
}

// begin void MainWindow::startConversion
void MainWindow::startConversion()
{
    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    if (m_sourceFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите исходный файл");
        return;
    }

    m_conversionActive = true;
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    m_downloadBtn->setEnabled(false);

    QString sourceFile = m_sourceFileEdit->text();
    QString outputFile = m_outputDir + "/" + generateOutputFileName(sourceFile);
    m_currentOutputFile = outputFile;

    appendLog("==================================================");
    appendLog("НАЧАЛО КОНВЕРТАЦИИ");
    appendLog("==================================================");
    appendLog("Исходный файл: " + sourceFile);
    appendLog("Выходной файл: " + outputFile);

    // Создаем рабочий поток для конвертации
    m_conversionThread = new QThread(this);
    m_conversionWorker = new ConversionWorker(m_sshClient.get());
    m_conversionWorker->moveToThread(m_conversionThread);

    // Подключаем сигналы
    connect(m_conversionThread, &QThread::started, m_conversionWorker, &ConversionWorker::start);
    connect(m_conversionWorker, &ConversionWorker::finished, this, &MainWindow::onConversionFinished);
    connect(m_conversionWorker, &ConversionWorker::finished, m_conversionThread, &QThread::quit);
    connect(m_conversionWorker, &ConversionWorker::finished, m_conversionWorker, &QObject::deleteLater);
    connect(m_conversionThread, &QThread::finished, m_conversionThread, &QObject::deleteLater);

    // Подключаем сигнал прогресса к обновлению интерфейса
    connect(m_conversionWorker, &ConversionWorker::progress, this, &MainWindow::onConversionProgress);

    // Подключаем сигнал прогресса к обновлению очереди (для пакетного режима)
    connect(m_conversionWorker, &ConversionWorker::progress, this, [this](int percent, double, double, double, const QString&) {
        if (m_batchMode && m_currentBatchIndex >= 0) {
            updateBatchItemProgress(m_currentBatchIndex, percent, "processing");
        }
    });

    connect(m_conversionWorker, &ConversionWorker::logMessage, this, &MainWindow::appendLog);

    // Формируем команду ffmpeg
    QString cmd = buildFFmpegCommand(sourceFile, outputFile);
    m_conversionWorker->setCommand(cmd);
    m_conversionWorker->setSourceFile(sourceFile);
    m_conversionWorker->setOutputFile(outputFile);

    // Получаем длительность видео для прогресс-бара
    QString durationCmd = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 '" + sourceFile + "'";
    QString output;
    if (m_sshClient->executeCommand(durationCmd, &output)) {
        double duration = output.trimmed().toDouble();
        m_conversionWorker->setTotalDuration(duration);
        appendLog("Длительность: " + formatTime(duration));
    }

    m_conversionThread->start();
}

void MainWindow::stopConversion()
{
    if (m_conversionActive && m_conversionWorker) {
        m_conversionWorker->stop();
        appendLog("■ Остановка конвертации...");
    }
}

void MainWindow::onConversionStarted()
{
    appendLog("Конвертация начата");
}


void MainWindow::onConversionProgress(int percent, double speed, double currentTime, double totalTime, const QString &status)
{
    // Обновляем прогресс бар
    m_progressBar->setValue(percent);
    m_percentValue->setText(QString("%1%").arg(percent));

    // Обновляем скорость
    if (speed > 0) {
        m_speedValue->setText(QString("%1x").arg(speed, 0, 'f', 2));
    }

    // Обновляем время
    if (currentTime > 0) {
        m_currentTimeValue->setText(formatTime(currentTime));
    }

    if (totalTime > 0) {
        m_totalTimeValue->setText(formatTime(totalTime));
    }

    // Рассчитываем ETA
    if (totalTime > 0 && currentTime > 0 && speed > 0) {
        double remaining = (totalTime - currentTime) / speed;
        m_etaValue->setText(formatTime(remaining));
    }

    // Обновляем статус
    m_statusValue->setText(status);

    // Принудительно обновляем интерфейс
    QApplication::processEvents();
}

void MainWindow::onConversionFinished(bool success, const QString &message, const QString &outputPath)
{
    m_conversionActive = false;
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);

    if (success) {
        appendLog("✓ " + message);
        m_progressBar->setValue(100);
        m_statusValue->setText("Завершено");
        m_downloadBtn->setEnabled(true);
        m_currentOutputFile = outputPath;

        // Если это часть пакетной обработки
        if (m_batchMode && m_currentBatchIndex >= 0) {
            // Обновляем статус на "done"
            updateBatchItemProgress(m_currentBatchIndex, 100, "done");

            // Получаем размер выходного файла
            QString sizeCmd = "stat -c %s '" + outputPath + "'";
            QString sizeOutput;
            if (m_sshClient->executeCommand(sizeCmd, &sizeOutput)) {
                m_batchQueue[m_currentBatchIndex].size = sizeOutput.trimmed().toLongLong();
            }

            // Переходим к следующему файлу
            QTimer::singleShot(1000, this, &MainWindow::processNextInBatch);
        }
    } else {
        appendLog("✗ " + message);
        m_statusValue->setText("Ошибка");

        if (m_batchMode && m_currentBatchIndex >= 0) {
            // Обновляем статус на "failed"
            updateBatchItemProgress(m_currentBatchIndex, 0, "failed");

            // Переходим к следующему файлу
            QTimer::singleShot(1000, this, &MainWindow::processNextInBatch);
        }
    }

    m_conversionThread = nullptr;
    m_conversionWorker = nullptr;
}

// Для BatchQueueDialog
void MainWindow::showBatchQueue()
{
    if (m_batchQueue.isEmpty()) {
        QMessageBox::information(this, "Информация", "Очередь пуста");
        return;
    }

    if (m_batchQueueDialog) {
        if (m_batchQueueDialog->isVisible()) {
            m_batchQueueDialog->raise();
            m_batchQueueDialog->activateWindow();
            m_batchQueueDialog->updateDisplay();
        } else {
            delete m_batchQueueDialog;
            m_batchQueueDialog = nullptr;
        }
    }

    if (!m_batchQueueDialog) {
        m_batchQueueDialog = new BatchQueueDialog(&m_batchQueue, this);
        m_batchQueueDialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(m_batchQueueDialog, &BatchQueueDialog::queueUpdated,
                this, &MainWindow::updateQueueInfo);
        connect(m_batchQueueDialog, &BatchQueueDialog::startBatch,
                this, &MainWindow::startBatchConversion);
        connect(m_batchQueueDialog, &BatchQueueDialog::stopBatch,
                this, &MainWindow::stopBatchConversion);

        connect(m_batchQueueDialog, &QDialog::finished, [this]() {
            m_batchQueueDialog = nullptr;
        });
    }

    m_batchQueueDialog->show();
    m_batchQueueDialog->raise();
}

void MainWindow::clearBatchQueue()
{
    m_batchQueue.clear();
    updateQueueInfo();
    appendLog("✓ Очередь очищена");
}

void MainWindow::startBatchConversion()
{
    m_batchMode = true;
    m_batchStopRequested = false;
    appendLog("▶ Запуск пакетной обработки");
    processNextInBatch();
}

void MainWindow::stopBatchConversion()
{
    if (m_batchMode) {
        QMessageBox::information(this, "Остановка",
                                 "Кодирование будет остановлено при достижении конца текущего файла");

        m_batchStopRequested = true;
        if (m_conversionActive) {
            stopConversion();
        }
        appendLog("■ Пакетная обработка будет остановлена после завершения текущего файла");
    }
}

void MainWindow::updateQueueInfo()
{
    int queued = 0;
    for (const auto &item : m_batchQueue) {
        if (item.status == "queued") {
            queued++;
        }
    }
    m_queueInfoLabel->setText(QString("Очередь: %1/%2 файлов")
    .arg(queued).arg(m_batchQueue.size()));
}

void MainWindow::processNextInBatch()
{
    if (m_batchStopRequested) {
        appendLog("■ Пакетная обработка остановлена пользователем");
        m_batchMode = false;
        return;
    }

    // Находим следующий необработанный файл
    for (int i = 0; i < m_batchQueue.size(); ++i) {
        if (m_batchQueue[i].status == "queued") {
            m_currentBatchIndex = i;
            m_batchQueue[i].status = "processing";
            m_batchQueue[i].progress = 0;
            m_sourceFileEdit->setText(m_batchQueue[i].path);

            appendLog(QString("▶ Обработка файла %1 из %2: %3")
            .arg(i + 1)
            .arg(m_batchQueue.size())
            .arg(m_batchQueue[i].name));

            // Обновляем отображение
            if (m_batchQueueDialog) {
                m_batchQueueDialog->updateDisplay();
            }

            startConversion();
            return;
        }
    }

    // Если дошли сюда, значит все файлы обработаны
    appendLog("✓ Пакетная обработка завершена");
    m_batchMode = false;
    m_currentBatchIndex = -1;

    // Подводим итоги
    int done = 0, failed = 0;
    for (const auto &item : m_batchQueue) {
        if (item.status == "done") done++;
        else if (item.status == "failed") failed++;
    }
    appendLog(QString("Результат: успешно %1, ошибок %2").arg(done).arg(failed));
}


void MainWindow::saveProfile()
{
    if (!m_profilesDialog) {
        m_profilesDialog = new ProfilesDialog(this);
    }
    m_profilesDialog->show();
}

void MainWindow::loadProfile(const EncoderProfile &profile)
{
    m_noVideoCheck->setChecked(profile.noVideo);
    m_codecCombo->setCurrentText(profile.videoCodec);
    m_resolutionCombo->setCurrentText(profile.resolution);
    m_videoBitrateCombo->setCurrentText(profile.videoBitrate);
    m_audioCodecCombo->setCurrentText(profile.audioCodec);
    m_audioBitrateCombo->setCurrentText(profile.audioBitrate);
    m_audioSampleRateCombo->setCurrentText(profile.audioSampleRate);
    m_audioChannelsCombo->setCurrentText(profile.audioChannels);
    m_outputFilenameEdit->setText(profile.outputTemplate);

    if (!profile.outputDir.isEmpty()) {
        m_outputDir = profile.outputDir;
        m_outputDirLabel->setText(profile.outputDir);
    }

    appendLog("✓ Профиль загружен: " + profile.name);
    updateFileNamePreview();
}

void MainWindow::showProfilesDialog()
{
    if (m_profilesDialog) {
        if (m_profilesDialog->isVisible()) {
            m_profilesDialog->raise();
            m_profilesDialog->activateWindow();
        } else {
            delete m_profilesDialog;
            m_profilesDialog = nullptr;
        }
    }

    if (!m_profilesDialog) {
        m_profilesDialog = new ProfilesDialog(this);
        m_profilesDialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(m_profilesDialog, &ProfilesDialog::profileLoaded,
                this, &MainWindow::loadProfile);

        connect(m_profilesDialog, &QDialog::finished, [this]() {
            m_profilesDialog = nullptr;
        });
    }

    m_profilesDialog->show();
    m_profilesDialog->raise();
}

void MainWindow::showConnectionSettings()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Настройки подключения");
    dialog.resize(400, 200);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *form = new QFormLayout;

    QLineEdit *ipEdit = new QLineEdit(m_ipEdit->text());
    QLineEdit *userEdit = new QLineEdit(m_userEdit->text());
    QLineEdit *passEdit = new QLineEdit(m_passEdit->text());
    passEdit->setEchoMode(QLineEdit::Password);

    form->addRow("IP адрес:", ipEdit);
    form->addRow("Пользователь:", userEdit);
    form->addRow("Пароль:", passEdit);

    layout->addLayout(form);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        m_ipEdit->setText(ipEdit->text());
        m_userEdit->setText(userEdit->text());
        m_passEdit->setText(passEdit->text());

        // НЕМЕДЛЕННО сохраняем настройки
        m_settings->setConnectionSettings(
            m_ipEdit->text(),
                                          m_userEdit->text(),
                                          m_passEdit->text()
        );

        appendLog("✓ Настройки подключения сохранены");
    }
}
//end

void MainWindow::saveSettings()
{
    // Сохраняем настройки подключения
    m_settings->setConnectionSettings(
        m_ipEdit->text(),
                                      m_userEdit->text(),
                                      m_passEdit->text()
    );

    // Сохраняем папку результата
    m_settings->setOutputDir(m_outputDir);

    // Сохраняем профиль кодирования
    EncoderProfile profile;
    profile.noVideo = m_noVideoCheck->isChecked();
    profile.videoCodec = m_codecCombo->currentText();
    profile.resolution = m_resolutionCombo->currentText();
    profile.videoBitrate = m_videoBitrateCombo->currentText();
    profile.audioCodec = m_audioCodecCombo->currentText();
    profile.audioBitrate = m_audioBitrateCombo->currentText();
    profile.audioSampleRate = m_audioSampleRateCombo->currentText();
    profile.audioChannels = m_audioChannelsCombo->currentText();
    profile.outputTemplate = m_outputFilenameEdit->text();
    profile.outputDir = m_outputDir;

    m_settings->setCurrentProfile(profile);

    appendLog("✓ Настройки сохранены");
    appendLog("  IP: " + m_ipEdit->text());
    appendLog("  Папка результата: " + m_outputDir);
}

void MainWindow::loadSettings()
{
    // Загружаем настройки подключения
    QString host = m_settings->getHost();
    QString user = m_settings->getUser();
    QString password = m_settings->getPassword();

    // Применяем к полям ввода
    m_ipEdit->setText(host);
    m_userEdit->setText(user);
    m_passEdit->setText(password);

    // Загружаем папку результата
    m_outputDir = m_settings->getOutputDir();
    m_outputDirLabel->setText(m_outputDir);

    // Загружаем профиль кодирования
    EncoderProfile profile = m_settings->getCurrentProfile();
    m_noVideoCheck->setChecked(profile.noVideo);
    m_codecCombo->setCurrentText(profile.videoCodec);
    m_resolutionCombo->setCurrentText(profile.resolution);
    m_videoBitrateCombo->setCurrentText(profile.videoBitrate);
    m_audioCodecCombo->setCurrentText(profile.audioCodec);
    m_audioBitrateCombo->setCurrentText(profile.audioBitrate);
    m_audioSampleRateCombo->setCurrentText(profile.audioSampleRate);
    m_audioChannelsCombo->setCurrentText(profile.audioChannels);
    m_outputFilenameEdit->setText(profile.outputTemplate);

    // Логируем загруженные значения для отладки
    qDebug() << "Settings loaded:";
    qDebug() << "  Host:" << host;
    qDebug() << "  User:" << user;
    qDebug() << "  Output dir:" << m_outputDir;

    appendLog("✓ Настройки загружены");
    appendLog("  IP: " + host);
    appendLog("  Папка результата: " + m_outputDir);
}

void MainWindow::resetSettings()
{
    m_settings->resetAll();
    loadSettings();
    appendLog("✓ Настройки сброшены");
}

void MainWindow::onNoVideoChanged(bool checked)
{
    m_videoGroup->setEnabled(!checked);
    updateFileNamePreview();

    if (checked) {
        appendLog("✓ Режим извлечения аудио активирован");
    } else {
        appendLog("✓ Режим перекодирования видео активирован");
    }
}

void MainWindow::onCodecChanged(const QString &codec)
{
    Q_UNUSED(codec);
    updateFileNamePreview();
}

void MainWindow::updateFileNamePreview()
{
    if (m_sourceFileEdit->text().isEmpty()) {
        m_previewLabel->clear();
        return;
    }

    QString preview = generateOutputFileName(m_sourceFileEdit->text());
    m_previewLabel->setText("→ " + preview);
}

void MainWindow::onFileSelected(const QString &path)
{
    m_sourceFileEdit->setText(path);
    updateFileNamePreview();
      // m_settings->addRecentFile(path);  // Закомментируем эту строку
    appendLog("Выбран файл: " + path);
}

void MainWindow::onDirectorySelected(const QString &path)
{
    m_outputDir = path;
    m_outputDirLabel->setText(path);
    m_settings->setOutputDir(path);
    appendLog("Папка результата: " + path);

    // Если файловый менеджер открыт, можно обновить его текущий путь
    // (опционально)
}

void MainWindow::onFilesAddedToBatch(const QStringList &files)
{
    int added = 0;
    for (const QString &file : files) {
        // Проверяем, нет ли уже такого файла в очереди
        bool exists = false;
        for (const auto &item : m_batchQueue) {
            if (item.path == file) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            BatchItem item;
            item.path = file;
            item.name = QFileInfo(file).fileName();
            item.status = "queued";
            item.progress = 0;
            m_batchQueue.append(item);
            added++;
        }
    }

    if (added > 0) {
        appendLog(QString("✓ Добавлено %1 файлов в очередь").arg(added));
        updateQueueInfo();
    }
}


//begin
void MainWindow::downloadFile(const QString &remotePath)
{
    if (remotePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет файла для скачивания");
        return;
    }

    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    QString saveDir = QFileDialog::getExistingDirectory(this,
                                                        "Выберите папку для сохранения",
                                                        QDir::homePath());

    if (saveDir.isEmpty()) return;

    QString fileName = QFileInfo(remotePath).fileName();
    QString localPath = saveDir + "/" + fileName;

    // Проверяем, существует ли уже такой файл локально
    if (QFile::exists(localPath)) {
        if (QMessageBox::question(this, "Файл существует",
            "Файл " + fileName + " уже существует. Перезаписать?")
            != QMessageBox::Yes) {
            return;
            }
    }

    appendLog("Скачивание файла: " + remotePath);
    appendLog("Сохранение в: " + localPath);

    // Создаем диалог прогресса
    QProgressDialog *progressDialog = new QProgressDialog("Скачивание файла...", "Отмена", 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setValue(0);
    progressDialog->show();

    QApplication::processEvents();

    // Скачиваем файл
    bool success = m_sshClient->downloadFile(remotePath, localPath,
                                             [progressDialog](int progress) {
                                                 progressDialog->setValue(progress);
                                                 QApplication::processEvents();
                                             });

    progressDialog->close();
    delete progressDialog;

    if (success) {
        appendLog("✓ Файл скачан: " + localPath);
        QMessageBox::information(this, "Успешно", "Файл успешно скачан");
    } else {
        appendLog("✗ Ошибка скачивания файла");
        QMessageBox::critical(this, "Ошибка", "Не удалось скачать файл");
    }
}


void MainWindow::appendLog(const QString &message)
{
    if (m_showLogCheck->isChecked()) {
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        m_logArea->append("[" + timestamp + "] " + message);
    }
}

void MainWindow::clearLog()
{
    m_logArea->clear();
    appendLog("Лог очищен");
}

void MainWindow::toggleLogVisibility()
{
    m_logFrame->setVisible(m_showLogCheck->isChecked());
}

void MainWindow::saveLogToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить лог",
                                                    QDir::homePath() + "/radxa_converter_log.txt",
                                                    "Текстовые файлы (*.txt)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << m_logArea->toPlainText();
            file.close();
            appendLog("✓ Лог сохранен в: " + fileName);
        }
    }
}

void MainWindow::showAbout()
{
    QString aboutText =
    "Radxa ROCK 4D - Удаленный перекодировщик видео\n\n"
    "Версия: 3.1\n"
    "Автор: Разработано для управления аппаратным перекодированием на Radxa ROCK 4D\n\n"
    "Что означают каналы:\n"
    "• 1 - моно (один канал)\n"
    "• 2 - стерео (левый/правый)\n"
    "• 6 - 5.1 surround (фронт L/R, центр, сабвуфер, тыл L/R)\n\n"
    "Лицензия: MIT";

    QMessageBox::about(this, "О программе", aboutText);
}

QString MainWindow::formatTime(double seconds) const
{
    if (seconds <= 0) return "00:00:00";

    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;

    return QString("%1:%2:%3")
    .arg(hours, 2, 10, QChar('0'))
    .arg(minutes, 2, 10, QChar('0'))
    .arg(secs, 2, 10, QChar('0'));
}

QString MainWindow::formatSize(qint64 bytes) const
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = bytes;

    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }

    return QString("%1 %2").arg(size, 0, 'f', 1).arg(units[unitIndex]);
}

QString MainWindow::getOutputExtension() const
{
    if (m_noVideoCheck->isChecked()) {
        QString audioCodec = m_audioCodecCombo->currentText();
        if (audioCodec == "mp3") return ".mp3";
        if (audioCodec == "aac") return ".aac";
        if (audioCodec == "flac") return ".flac";
        if (audioCodec == "opus") return ".opus";
        if (audioCodec == "libvorbis") return ".ogg";
        if (audioCodec == "ac3") return ".ac3";
        return ".m4a";
    }
    return ".mp4";
}

QString MainWindow::generateOutputFileName(const QString &sourceFile) const
{
    QFileInfo fileInfo(sourceFile);
    QString baseName = fileInfo.completeBaseName();

    QString template_ = m_outputFilenameEdit->text();
    if (template_.isEmpty()) {
        template_ = "{name}_{codec}_{res}";
    }

    QString result = template_;
    result.replace("{name}", baseName);

    if (m_noVideoCheck->isChecked()) {
        result.replace("{codec}", m_audioCodecCombo->currentText());
        result.replace("{res}", "audio");
    } else {
        result.replace("{codec}", m_codecCombo->currentText());
        QString res = m_resolutionCombo->currentText();
        result.replace("{res}", res.isEmpty() ? "original" : res);
    }

    QString ext = getOutputExtension();
    if (!result.endsWith(ext)) {
        result += ext;
    }

    return result;
}

QString MainWindow::buildFFmpegCommand(const QString &sourceFile, const QString &outputFile) const
{
    QStringList cmd;
    cmd << "ffmpeg -y -i" << "'" + sourceFile + "'";

    if (m_noVideoCheck->isChecked()) {
        // Режим извлечения аудио
        if (m_audioCodecCombo->currentText() == "copy") {
            cmd << "-c:a copy";
        } else {
            cmd << "-c:a" << m_audioCodecCombo->currentText();

            if (!m_audioBitrateCombo->currentText().isEmpty()) {
                cmd << "-b:a" << m_audioBitrateCombo->currentText();
            }

            if (!m_audioSampleRateCombo->currentText().isEmpty()) {
                cmd << "-ar" << m_audioSampleRateCombo->currentText();
            }

            if (!m_audioChannelsCombo->currentText().isEmpty()) {
                cmd << "-ac" << m_audioChannelsCombo->currentText();
            }
        }
        cmd << "-vn";
    } else {
        // Режим перекодирования видео
        if (m_codecCombo->currentText() == "copy") {
            cmd << "-c:v copy";
        } else {
            cmd << "-c:v" << m_codecCombo->currentText();

            if (!m_resolutionCombo->currentText().isEmpty()) {
                cmd << "-vf" << "scale=" + m_resolutionCombo->currentText();
            }

            if (!m_videoBitrateCombo->currentText().isEmpty()) {
                cmd << "-b:v" << m_videoBitrateCombo->currentText();
            }
        }

        // Аудио параметры
        if (m_audioCodecCombo->currentText() == "copy") {
            cmd << "-c:a copy";
        } else {
            cmd << "-c:a" << m_audioCodecCombo->currentText();

            if (!m_audioBitrateCombo->currentText().isEmpty()) {
                cmd << "-b:a" << m_audioBitrateCombo->currentText();
            }

            if (!m_audioSampleRateCombo->currentText().isEmpty()) {
                cmd << "-ar" << m_audioSampleRateCombo->currentText();
            }

            if (!m_audioChannelsCombo->currentText().isEmpty()) {
                cmd << "-ac" << m_audioChannelsCombo->currentText();
            }
        }
    }

    // Опции для вывода прогресса в реальном времени
    cmd << "-progress" << "/dev/stdout";  // Выводим прогресс в stdout
    cmd << "-loglevel" << "info";         // Уровень логирования
    cmd << "-nostats";                    // Отключаем статистику (чтобы не засорять вывод)

    cmd << "'" + outputFile + "'";

    return cmd.join(" ");
}

void MainWindow::showFileInfo()
{
    if (m_sourceFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите файл");
        return;
    }

    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    QString sourceFile = m_sourceFileEdit->text();
    appendLog("Получение информации о файле: " + sourceFile);

    // Создаем диалог загрузки
    QDialog *progressDialog = new QDialog(this);
    progressDialog->setWindowTitle("Загрузка информации");
    progressDialog->setModal(true);
    progressDialog->resize(300, 100);

    QVBoxLayout *layout = new QVBoxLayout(progressDialog);
    QLabel *label = new QLabel("Получение информации о файле...");
    layout->addWidget(label);
    QProgressBar *bar = new QProgressBar();
    bar->setRange(0, 0);  // Бесконечный прогресс
    layout->addWidget(bar);

    progressDialog->show();
    QApplication::processEvents();

    // Получаем информацию в отдельном потоке
    QString output;
    bool success = m_sshClient->getFileInfo(sourceFile, output);

    progressDialog->close();
    delete progressDialog;

    if (!success || output.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить информацию о файле");
        return;
    }

    // Парсим JSON
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (doc.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось распарсить информацию о файле");
        return;
    }

    QJsonObject obj = doc.object();
    QJsonObject format = obj["format"].toObject();
    QJsonArray streams = obj["streams"].toArray();

    // Формируем информационное окно
    QString infoText;
    infoText += "Файл: " + QFileInfo(sourceFile).fileName() + "\n";
    infoText += "Путь: " + sourceFile + "\n";
    infoText += "----------------------------------------\n";

    // Общая информация
    if (!format.isEmpty()) {
        double duration = format["duration"].toString().toDouble();
        QString durationStr = formatTime(duration);
        qint64 size = format["size"].toString().toLongLong();
        qint64 bitrate = format["bit_rate"].toString().toLongLong() / 1000;

        infoText += QString("Длительность: %1\n").arg(durationStr);
        infoText += QString("Размер: %1\n").arg(formatSize(size));
        infoText += QString("Битрейт: %1 kbps\n").arg(bitrate);
        infoText += QString("Формат: %1\n").arg(format["format_name"].toString());
        infoText += "----------------------------------------\n";
    }

    // Видео потоки
    int videoCount = 0;
    for (const QJsonValue &val : streams) {
        QJsonObject stream = val.toObject();
        if (stream["codec_type"].toString() == "video") {
            videoCount++;
            infoText += QString("\n=== Видео поток #%1 ===\n").arg(videoCount);
            infoText += QString("Кодек: %1\n").arg(stream["codec_name"].toString());
            infoText += QString("Разрешение: %1x%2\n")
            .arg(stream["width"].toInt())
            .arg(stream["height"].toInt());

            if (stream.contains("avg_frame_rate")) {
                QString fps = stream["avg_frame_rate"].toString();
                if (fps.contains('/')) {
                    QStringList parts = fps.split('/');
                    if (parts.size() == 2 && parts[1].toInt() > 0) {
                        double fpsVal = parts[0].toDouble() / parts[1].toDouble();
                        infoText += QString("FPS: %1\n").arg(fpsVal, 0, 'f', 2);
                    }
                }
            }

            infoText += QString("Битрейт: %1 kbps\n")
            .arg(stream["bit_rate"].toString().toLongLong() / 1000);
            infoText += QString("Профиль: %1\n").arg(stream["profile"].toString());
            infoText += QString("Цвет: %1\n").arg(stream["pix_fmt"].toString());
        }
    }

    // Аудио потоки
    int audioCount = 0;
    for (const QJsonValue &val : streams) {
        QJsonObject stream = val.toObject();
        if (stream["codec_type"].toString() == "audio") {
            audioCount++;
            infoText += QString("\n=== Аудио поток #%1 ===\n").arg(audioCount);
            infoText += QString("Кодек: %1\n").arg(stream["codec_name"].toString());

            int channels = stream["channels"].toInt();
            QString channelsDesc;
            if (channels == 1) channelsDesc = "моно";
            else if (channels == 2) channelsDesc = "стерео";
            else if (channels == 6) channelsDesc = "5.1";
            else if (channels == 8) channelsDesc = "7.1";
            else channelsDesc = QString::number(channels) + " каналов";

            infoText += QString("Каналы: %1 (%2)\n").arg(channels).arg(channelsDesc);
            infoText += QString("Частота: %1 Hz\n").arg(stream["sample_rate"].toString());

            if (stream.contains("bit_rate")) {
                infoText += QString("Битрейт: %1 kbps\n")
                .arg(stream["bit_rate"].toString().toLongLong() / 1000);
            }

            if (stream.contains("language")) {
                infoText += QString("Язык: %1\n").arg(stream["language"].toString());
            }
        }
    }

    QMessageBox::information(this, "Информация о файле", infoText);
    appendLog("✓ Информация о файле получена");
}

void MainWindow::estimateFileSize()
{
    if (m_sourceFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите исходный файл");
        return;
    }

    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    QString sourceFile = m_sourceFileEdit->text();

    // Получаем информацию о исходном файле
    QString output;
    if (!m_sshClient->getFileInfo(sourceFile, output)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить информацию о файле");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (doc.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось распарсить информацию");
        return;
    }

    QJsonObject obj = doc.object();
    QJsonObject format = obj["format"].toObject();
    QJsonArray streams = obj["streams"].toArray();

    double originalDuration = format["duration"].toString().toDouble();
    qint64 originalSize = format["size"].toString().toLongLong();
    qint64 originalVideoBitrate = 0;
    qint64 originalAudioBitrate = 0;

    // Находим битрейты видео и аудио
    for (const QJsonValue &val : streams) {
        QJsonObject stream = val.toObject();
        QString codecType = stream["codec_type"].toString();
        qint64 bitrate = stream["bit_rate"].toString().toLongLong();

        if (codecType == "video" && bitrate > 0) {
            originalVideoBitrate = bitrate;
        } else if (codecType == "audio" && bitrate > 0) {
            originalAudioBitrate += bitrate;
        }
    }

    // Если битрейты не найдены, рассчитываем из размера и длительности
    if (originalVideoBitrate == 0 && originalDuration > 0) {
        originalVideoBitrate = (originalSize * 8) / originalDuration;
    }

    // Получаем целевые параметры
    qint64 targetVideoBitrate = 0;
    if (!m_noVideoCheck->isChecked()) {
        QString bitrateStr = m_videoBitrateCombo->currentText();
        if (bitrateStr.endsWith('k')) {
            targetVideoBitrate = bitrateStr.left(bitrateStr.length() - 1).toLongLong() * 1000;
        } else if (bitrateStr.endsWith('M')) {
            targetVideoBitrate = bitrateStr.left(bitrateStr.length() - 1).toLongLong() * 1000000;
        }
    }

    qint64 targetAudioBitrate = 0;
    if (m_audioCodecCombo->currentText() != "copy") {
        QString bitrateStr = m_audioBitrateCombo->currentText();
        if (bitrateStr.endsWith('k')) {
            targetAudioBitrate = bitrateStr.left(bitrateStr.length() - 1).toLongLong() * 1000;
        }
    }

    // Рассчитываем примерный размер
    qint64 estimatedSize = 0;
    QString estimationDetails;

    if (m_noVideoCheck->isChecked()) {
        // Только аудио
        if (targetAudioBitrate > 0) {
            estimatedSize = (targetAudioBitrate * originalDuration) / 8;
            estimationDetails = QString("Аудио битрейт: %1 kbps")
            .arg(targetAudioBitrate / 1000);
        } else {
            estimatedSize = originalAudioBitrate * originalDuration / 8;
            estimationDetails = "Копирование аудио (оригинальный битрейт)";
        }
    } else {
        // Видео + аудио
        if (targetVideoBitrate > 0) {
            estimatedSize = (targetVideoBitrate * originalDuration) / 8;
            estimationDetails = QString("Видео битрейт: %1 kbps")
            .arg(targetVideoBitrate / 1000);
        } else if (m_codecCombo->currentText() == "copy") {
            estimatedSize = originalSize;
            estimationDetails = "Копирование видео (оригинальный размер)";
        } else {
            estimatedSize = originalSize;
            estimationDetails = "Используется оригинальный размер (приблизительно)";
        }

        // Добавляем аудио
        if (targetAudioBitrate > 0) {
            estimatedSize += (targetAudioBitrate * originalDuration) / 8;
            estimationDetails += QString("\nАудио битрейт: %1 kbps")
            .arg(targetAudioBitrate / 1000);
        }
    }

    // Добавляем небольшой оверхед (5%)
    estimatedSize = estimatedSize * 1.05;

    // Проверяем свободное место
    QString freeSpace;
    bool hasSpace = m_sshClient->checkFreeSpace(m_outputDir, estimatedSize, freeSpace);

    // ИСПРАВЛЕНО: используем ОДНУ переменную message
    QString message = QString(
        "Исходный файл: %1\n"
        "Размер исходного: %2\n"
        "Длительность: %3\n"
        "\n"
        "Параметры кодирования:\n"
        "%4\n"
        "\n"
        "ПРИМЕРНЫЙ РАЗМЕР: %5\n"
        "\n"
        "Целевая папка: %6\n"
        "Свободное место: %7\n"
        "\n"
        "%8\n"
        "\n"
        "Выходной файл: %9"
    ).arg(QFileInfo(sourceFile).fileName())
    .arg(formatSize(originalSize))
    .arg(formatTime(originalDuration))
    .arg(estimationDetails)
    .arg(formatSize(estimatedSize))
    .arg(m_outputDir)
    .arg(freeSpace)
    .arg(hasSpace ? "✓ Места достаточно" : "⚠ НЕДОСТАТОЧНО МЕСТА!")
    .arg(generateOutputFileName(sourceFile));

    QMessageBox::information(this, "Оценка размера файла", message);
    appendLog("✓ Оценка размера: " + formatSize(estimatedSize));
}

//end
void MainWindow::updateConnectionInfo()
{
    if (!m_sshClient->isConnected()) {
        statusBar()->showMessage("Не подключено");
        return;
    }

    // Получаем имя интерфейса
    QString ifaceCmd = "ip route get 1 | awk '{print $5; exit}'";
    QString iface;
    if (m_sshClient->executeCommand(ifaceCmd, &iface)) {
        m_connectionInterface = iface.trimmed();
    }

    // Получаем скорость соединения (ping)
    QString pingCmd = "ping -c 1 -W 1 " + m_ipEdit->text() + " | grep time= | awk -F'time=' '{print $2}' | awk '{print $1}'";
    QString ping;
    if (m_sshClient->executeCommand(pingCmd, &ping)) {
        m_connectionSpeed = ping.trimmed().toFloat() * 1000; // в мс
    }

    statusBar()->showMessage(
        QString("Подключено к %1 | Интерфейс: %2 | Задержка: %3 мс | Папка: %4")
        .arg(m_ipEdit->text())
        .arg(m_connectionInterface)
        .arg(m_connectionSpeed)
        .arg(m_outputDir)
    );
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1) {
        showHelp();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showHelp()
{
    QString helpText =
    "Справка по параметрам кодирования\n"
    "================================\n\n"
    "=== ВИДЕО ===\n"
    "• hevc_rkmpp - аппаратное кодирование H.265 (рекомендуется для Radxa)\n"
    "• h264_rkmpp - аппаратное кодирование H.264\n"
    "• libx265 - программное кодирование H.265 (медленнее, но совместимее)\n"
    "• libx264 - программное кодирование H.264\n"
    "• copy - копировать видео без перекодирования\n\n"
    "Разрешение: выходное разрешение видео (пусто = оригинальное)\n"
    "  Доступные варианты: 3840x2160 (4K), 1920x1080 (Full HD), \n"
    "  1280x720 (HD), 854x480 (DVD)\n\n"
    "Битрейт: качество видео (чем выше, тем лучше качество и больше файл)\n"
    "  • 1M - 1000 kbps (низкое)\n"
    "  • 5M - 5000 kbps (среднее)\n"
    "  • 10M - 10000 kbps (высокое)\n\n"
    "=== АУДИО ===\n"
    "• copy - копировать аудио без перекодирования\n"
    "• aac - хорошее качество, малый размер (рекомендуется)\n"
    "• mp3 - стандартный формат, широкая совместимость\n"
    "• ac3 - формат для DVD/Blu-ray (поддержка 5.1)\n"
    "• flac - без потерь, очень большой размер\n"
    "• opus - современный формат, отличное качество при низком битрейте\n\n"
    "=== КАНАЛЫ ===\n"
    "• 1 - моно (один канал)\n"
    "• 2 - стерео (левый/правый)\n"
    "• 6 - 5.1 surround (фронт L/R, центр, сабвуфер, тыл L/R)\n\n"
    "=== ШАБЛОН ИМЕНИ ===\n"
    "{name} - исходное имя файла (без расширения)\n"
    "{codec} - выбранный кодек\n"
    "{res} - разрешение (или 'audio' для режима без видео)\n\n"
    "Пример: {name}_{codec}_{res}.mp4 → 'video_hevc_rkmpp_1080p.mp4'\n\n"
    "=== ГОРЯЧИЕ КЛАВИШИ ===\n"
    "F1 - эта справка\n"
    "Ctrl+O - открыть файловый менеджер\n"
    "Ctrl+S - старт конвертации\n"
    "Ctrl+Q - выход";

        QMessageBox::information(this, "Справка - Radxa Converter", helpText);
}

void MainWindow::updateBatchItemProgress(int index, int percent, const QString &status)
{
    if (index >= 0 && index < m_batchQueue.size()) {
        m_batchQueue[index].progress = percent;
        m_batchQueue[index].status = status;

        // Обновляем отображение в очереди, если окно открыто
        if (m_batchQueueDialog) {
            m_batchQueueDialog->updateDisplay();
        }

        // Обновляем общую информацию о очереди
        updateQueueInfo();
    }
}
