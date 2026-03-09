#include "settingsmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QByteArray>

SettingsManager::SettingsManager(QObject *parent)
: QObject(parent)
, m_settings(new QSettings("Radxa", "RadxaConverter", this))
{
}

SettingsManager::~SettingsManager()
{
}

QString SettingsManager::getHost() const
{
    return m_settings->value("connection/host", "192.168.1.100").toString();
}

QString SettingsManager::getUser() const
{
    return m_settings->value("connection/user", "rock").toString();
}




QString SettingsManager::getOutputDir() const
{
    return m_settings->value("paths/output_dir", "/home/rock/converted").toString();
}

void SettingsManager::setOutputDir(const QString &path)
{
    m_settings->setValue("paths/output_dir", path);
}

EncoderProfile SettingsManager::getCurrentProfile() const
{
    EncoderProfile profile;
    profile.noVideo = m_settings->value("encoder/no_video", false).toBool();
    profile.videoCodec = m_settings->value("encoder/video_codec", "hevc_rkmpp").toString();
    profile.resolution = m_settings->value("encoder/resolution").toString();
    profile.videoBitrate = m_settings->value("encoder/video_bitrate", "5M").toString();
    profile.audioCodec = m_settings->value("encoder/audio_codec", "copy").toString();
    profile.audioBitrate = m_settings->value("encoder/audio_bitrate").toString();
    profile.audioSampleRate = m_settings->value("encoder/audio_sample_rate").toString();
    profile.audioChannels = m_settings->value("encoder/audio_channels").toString();
    profile.outputTemplate = m_settings->value("encoder/output_template", "{name}_{codec}_{res}.mp4").toString();
    return profile;
}

void SettingsManager::setCurrentProfile(const EncoderProfile &profile)
{
    m_settings->setValue("encoder/no_video", profile.noVideo);
    m_settings->setValue("encoder/video_codec", profile.videoCodec);
    m_settings->setValue("encoder/resolution", profile.resolution);
    m_settings->setValue("encoder/video_bitrate", profile.videoBitrate);
    m_settings->setValue("encoder/audio_codec", profile.audioCodec);
    m_settings->setValue("encoder/audio_bitrate", profile.audioBitrate);
    m_settings->setValue("encoder/audio_sample_rate", profile.audioSampleRate);
    m_settings->setValue("encoder/audio_channels", profile.audioChannels);
    m_settings->setValue("encoder/output_template", profile.outputTemplate);
}



void SettingsManager::resetAll()
{
    m_settings->clear();
}

QString SettingsManager::encrypt(const QString &text)
{
    // Простое XOR шифрование для примера
    QByteArray data = text.toUtf8();
    QByteArray key = QByteArray("RadxaSecretKey2025").leftJustified(16, '\0');

    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ key[i % key.size()];
    }

    return data.toBase64();
}

QString SettingsManager::decrypt(const QString &text) const
{
    QByteArray data = QByteArray::fromBase64(text.toUtf8());
    QByteArray key = QByteArray("RadxaSecretKey2025").leftJustified(16, '\0');

    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ key[i % key.size()];
    }

    return QString::fromUtf8(data);
}

void SettingsManager::setConnectionSettings(const QString &host, const QString &user, const QString &password)
{
    m_settings->setValue("connection/host", host);
    m_settings->setValue("connection/user", user);
    m_settings->setValue("connection/password", encrypt(password));
}

QString SettingsManager::getPassword() const
{
    QString encrypted = m_settings->value("connection/password").toString();
    return encrypted.isEmpty() ? "" : decrypt(encrypted);
}
