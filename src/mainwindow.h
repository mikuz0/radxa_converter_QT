#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QJsonObject>
#include <memory>
#include <QThread>
#include <QCloseEvent>
#include "settingsmanager.h"  // Добавляем этот include

// Forward declarations
QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class QProgressBar;
class QTextEdit;
class QGroupBox;
class QSettings;
class QThread;
QT_END_NAMESPACE

class SSHClient;
class FileManagerDialog;
class BatchQueueDialog;
class ProfilesDialog;
class ConversionWorker;

// Структура для элемента очереди
struct BatchItem {
    QString path;           // Полный путь к файлу
    QString name;           // Имя файла
    QString status;         // Статус: queued, processing, done, failed
    QString outputPath;     // Путь к выходному файлу
    qint64 size;            // Размер выходного файла
    double progress;        // Прогресс (0-100)

    BatchItem() : size(0), progress(0) {}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;  // Добавить эту строку
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void showHelp();
    void updateConnectionInfo();
    // Подключение
    void onConnectClicked();
    void onDisconnectClicked();
    void onConnectionStatusChanged(bool connected, const QString &message);
    
    // Файловый менеджер
    void showFileManager();
    void onFileSelected(const QString &path);
    void onDirectorySelected(const QString &path);
    void onFilesAddedToBatch(const QStringList &files);
    
    // Параметры кодирования
    void onNoVideoChanged(bool checked);
    void updateFileNamePreview();
    void onCodecChanged(const QString &codec);
    
    // Управление конвертацией
    void startConversion();
    void stopConversion();
    void onConversionStarted();
    void onConversionProgress(int percent, double speed, double currentTime, 
                              double totalTime, const QString &status);
    void onConversionFinished(bool success, const QString &message, const QString &outputPath);
    
    // Очередь
    void showBatchQueue();
    void clearBatchQueue();
    void startBatchConversion();
    void stopBatchConversion();
    void updateQueueInfo();
    void processNextInBatch();
    
    // Профили
    void saveProfile();
    void loadProfile(const EncoderProfile &profile);
    void showProfilesDialog();
    
    // Настройки
    void showConnectionSettings();
    void saveSettings();
    void loadSettings();
    void resetSettings();
    
    // Лог
    void appendLog(const QString &message);
    void clearLog();
    void toggleLogVisibility();
    void saveLogToFile();
    
    // О программе
    void showAbout();
    
    // Дополнительные функции
    void estimateFileSize();
    void showFileInfo();
    void downloadFile(const QString &remotePath);

        void updateBatchItemProgress(int index, int percent, const QString &status);

private:
    void setupUI();
    void createMenu();
    void createConnectionFrame();
    void createFileSelectionFrame();
    void createInfoFrame();
    void createEncoderFrame();
    void createControlFrame();
    void createLogArea();
    void createStatusBar();
    
    QString formatTime(double seconds) const;
    QString formatSize(qint64 bytes) const;
    QString getOutputExtension() const;
    QString generateOutputFileName(const QString &sourceFile) const;
    QString buildFFmpegCommand(const QString &sourceFile, const QString &outputFile) const;
    
    QTimer *m_connectionTimer;
    QString m_connectionInterface;
    int m_connectionSpeed;

    // Инициализация
    void initConnections();
    void initDefaultValues();
    
    // SSH клиент
    std::unique_ptr<SSHClient> m_sshClient;
    QString m_currentRemotePath = "/home/rock";
    
    // Настройки
    SettingsManager *m_settings;
    
    // Очередь пакетной обработки
    QList<BatchItem> m_batchQueue;
    bool m_batchMode = false;
    bool m_batchStopRequested = false;
    int m_currentBatchIndex = -1;
    
    // Рабочий поток для конвертации
    ConversionWorker *m_conversionWorker = nullptr;
    QThread *m_conversionThread = nullptr;
    
    // Диалоги
    FileManagerDialog *m_fileManagerDialog = nullptr;
    BatchQueueDialog *m_batchQueueDialog = nullptr;
    ProfilesDialog *m_profilesDialog = nullptr;
    
    // UI элементы - Connection Frame
    QLineEdit *m_ipEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;
    QLabel *m_statusLabel;
    
    // UI элементы - File Selection Frame
    QPushButton *m_fileManagerBtn;
    QLineEdit *m_sourceFileEdit;
    QLabel *m_outputDirLabel;
    QString m_outputDir;
    QLabel *m_queueInfoLabel;
    QPushButton *m_fileInfoBtn;
    
    // UI элементы - Info Frame
    QLabel *m_speedValue;
    QLabel *m_totalTimeValue;
    QLabel *m_currentTimeValue;
    QLabel *m_etaValue;
    QLabel *m_percentValue;
    QLabel *m_statusValue;
    QProgressBar *m_progressBar;
    QCheckBox *m_showLogCheck;
    QLabel *m_fileSizeEstimateLabel;
    
    // UI элементы - Encoder Frame
    QCheckBox *m_noVideoCheck;
    QGroupBox *m_videoGroup;
    QComboBox *m_codecCombo;
    QComboBox *m_resolutionCombo;
    QComboBox *m_videoBitrateCombo;
    QComboBox *m_audioCodecCombo;
    QComboBox *m_audioBitrateCombo;
    QComboBox *m_audioSampleRateCombo;
    QComboBox *m_audioChannelsCombo;
    QLineEdit *m_outputFilenameEdit;
    QLabel *m_previewLabel;
    QPushButton *m_estimateSizeBtn;
    
    // UI элементы - Control Frame
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_batchBtn;
    QPushButton *m_downloadBtn;
    
    // UI элементы - Log Area
    QTextEdit *m_logArea;
    QWidget *m_logFrame;
    
    bool m_conversionActive = false;
    QString m_currentOutputFile;
};

#endif // MAINWINDOW_H
