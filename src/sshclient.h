#ifndef SSHCLIENT_H
#define SSHCLIENT_H

#include <QObject>
#include <QString>
#include <functional>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

class SSHClient : public QObject
{
    Q_OBJECT

public:
    explicit SSHClient(QObject *parent = nullptr);
    ~SSHClient();
    bool checkFreeSpace(const QString &path, qint64 requiredSize, QString &available);
    bool isConnected() const { return m_connected; }

    void connectToHost(const QString &host, const QString &user, const QString &password);
    void disconnect();
    // Добавьте в public раздел
    bool getFileInfo(const QString &path, QString &info);

    bool executeCommand(const QString &command, QString *output = nullptr);
    bool executeCommandWithChannel(const QString &command,
                                   std::function<void(const QString&)> outputHandler);

    ssh_session getSession() const { return m_session; }

    // Новые методы для работы с файлами
    QString getHomeDirectory();
    bool uploadFile(const QString &localPath, const QString &remotePath,
                    std::function<void(int)> progressCallback = nullptr);
    bool downloadFile(const QString &remotePath, const QString &localPath,
                      std::function<void(int)> progressCallback = nullptr);
    bool createDirectory(const QString &path);
    bool removeFile(const QString &path);
    bool fileExists(const QString &path);

signals:
    void connectionStatusChanged(bool connected, const QString &message);
    void logMessage(const QString &message);

private:
    ssh_session m_session;
    sftp_session m_sftp;  // Добавляем SFTP сессию
    bool m_connected = false;
    bool initSftp();
    void cleanupSftp();
};

#endif // SSHCLIENT_H
