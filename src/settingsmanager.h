#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

struct EncoderProfile {
    QString name;
    QString description;
    bool noVideo = false;
    QString videoCodec = "hevc_rkmpp";
    QString resolution;
    QString videoBitrate = "5M";
    QString audioCodec = "copy";
    QString audioBitrate;
    QString audioSampleRate;
    QString audioChannels;
    QString outputTemplate = "{name}_{codec}_{res}.mp4";
    QString outputDir;
};

// Регистрируем структуру для Qt мета-системы
Q_DECLARE_METATYPE(EncoderProfile)

class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();  // Добавляем деструктор

    // Настройки подключения
    QString getHost() const;
    QString getUser() const;
    QString getPassword() const;
    void setConnectionSettings(const QString &host, const QString &user, const QString &password);

    // Настройки путей
    QString getOutputDir() const;
    void setOutputDir(const QString &path);

    // Параметры кодирования
    EncoderProfile getCurrentProfile() const;
    void setCurrentProfile(const EncoderProfile &profile);

    // Удаляем неиспользуемые методы или добавляем их реализацию
    // QList<EncoderProfile> getProfiles() const;  // Закомментируем если не нужны
    // void saveProfiles(const QList<EncoderProfile> &profiles);
    // void addProfile(const EncoderProfile &profile);
    // void removeProfile(const QString &name);
    // QByteArray getMainWindowGeometry() const;
    // void setMainWindowGeometry(const QByteArray &geometry);
    // QStringList getRecentFiles() const;
    // void addRecentFile(const QString &path);
    // void clearRecentFiles();

    // Сброс
    void resetAll();

private:
    QSettings *m_settings;
    static const int MAX_RECENT_FILES = 10;
    QString encrypt(const QString &text);
    QString decrypt(const QString &text) const;
};

#endif // SETTINGSMANAGER_H
