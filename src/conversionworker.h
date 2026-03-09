#ifndef CONVERSIONWORKER_H
#define CONVERSIONWORKER_H

#include <QObject>
#include <QProcess>
#include "sshclient.h"

class ConversionWorker : public QObject
{
    Q_OBJECT

public:
    explicit ConversionWorker(SSHClient *sshClient, QObject *parent = nullptr);
    ~ConversionWorker();

    void setCommand(const QString &cmd) { m_command = cmd; }
    void setSourceFile(const QString &file) { m_sourceFile = file; }
    void setOutputFile(const QString &file) { m_outputFile = file; }
    void setTotalDuration(double duration) { m_totalDuration = duration; }

public slots:
    void start();
    void stop();

signals:
    void started();
    void progress(int percent, double speed, double currentTime, double totalTime, const QString &status);
    void finished(bool success, const QString &message, const QString &outputPath);
    void logMessage(const QString &message);

private:
    void parseFFmpegOutput(const QString &line);
    void updateProgress();

    SSHClient *m_sshClient;
    QString m_command;
    QString m_sourceFile;
    QString m_outputFile;
    double m_totalDuration = 0;
    double m_currentTime = 0;
    double m_lastSpeed = 1.0;
    bool m_running = false;
    bool m_stopRequested = false;
};

#endif // CONVERSIONWORKER_H