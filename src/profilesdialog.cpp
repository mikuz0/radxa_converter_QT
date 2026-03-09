#include "profilesdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>

ProfilesDialog::ProfilesDialog(QWidget *parent)
: QDialog(parent)
{
    setWindowTitle("Управление профилями");
    resize(700, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel("Профили настроек кодирования");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Дерево профилей
    m_profileTree = new QTreeWidget;
    m_profileTree->setHeaderLabels({"Название", "Тип", "Видео", "Аудио", "Описание"});
    m_profileTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_profileTree->setAlternatingRowColors(true);

    m_profileTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_profileTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_profileTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_profileTree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_profileTree->header()->setSectionResizeMode(4, QHeaderView::Stretch);

    connect(m_profileTree, &QTreeWidget::itemSelectionChanged,
            this, &ProfilesDialog::onItemSelectionChanged);

    mainLayout->addWidget(m_profileTree);

    // Кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout;

    QPushButton *saveBtn = new QPushButton("💾 Сохранить текущие");
    connect(saveBtn, &QPushButton::clicked, this, &ProfilesDialog::onSaveProfile);
    btnLayout->addWidget(saveBtn);

    m_loadBtn = new QPushButton("📂 Загрузить");
    m_loadBtn->setEnabled(false);
    connect(m_loadBtn, &QPushButton::clicked, this, &ProfilesDialog::onLoadProfile);
    btnLayout->addWidget(m_loadBtn);

    m_deleteBtn = new QPushButton("🗑 Удалить");
    m_deleteBtn->setEnabled(false);
    m_deleteBtn->setStyleSheet("background-color: #f44336; color: white;");
    connect(m_deleteBtn, &QPushButton::clicked, this, &ProfilesDialog::onDeleteProfile);
    btnLayout->addWidget(m_deleteBtn);

    btnLayout->addSpacing(20);

    m_exportBtn = new QPushButton("⬆ Экспорт");
    m_exportBtn->setEnabled(false);
    connect(m_exportBtn, &QPushButton::clicked, this, &ProfilesDialog::onExportProfile);
    btnLayout->addWidget(m_exportBtn);

    m_importBtn = new QPushButton("⬇ Импорт");
    connect(m_importBtn, &QPushButton::clicked, this, &ProfilesDialog::onImportProfile);
    btnLayout->addWidget(m_importBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Загружаем профили
    loadProfiles();
}

void ProfilesDialog::loadProfiles()
{
    QSettings settings("Radxa", "RadxaConverter");
    int size = settings.beginReadArray("profiles");

    m_profiles.clear();

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        EncoderProfile profile;
        profile.name = settings.value("name").toString();
        profile.description = settings.value("description").toString();
        profile.noVideo = settings.value("no_video", false).toBool();
        profile.videoCodec = settings.value("video_codec", "hevc_rkmpp").toString();
        profile.resolution = settings.value("resolution").toString();
        profile.videoBitrate = settings.value("video_bitrate", "5M").toString();
        profile.audioCodec = settings.value("audio_codec", "copy").toString();
        profile.audioBitrate = settings.value("audio_bitrate").toString();
        profile.audioSampleRate = settings.value("audio_sample_rate").toString();
        profile.audioChannels = settings.value("audio_channels").toString();
        profile.outputTemplate = settings.value("output_template", "{name}_{codec}_{res}.mp4").toString();
        profile.outputDir = settings.value("output_dir").toString();

        m_profiles.append(profile);
    }

    settings.endArray();

    updateProfileList();
}

void ProfilesDialog::saveProfiles()
{
    QSettings settings("Radxa", "RadxaConverter");
    settings.beginWriteArray("profiles");

    for (int i = 0; i < m_profiles.size(); ++i) {
        settings.setArrayIndex(i);
        const EncoderProfile &profile = m_profiles.at(i);
        settings.setValue("name", profile.name);
        settings.setValue("description", profile.description);
        settings.setValue("no_video", profile.noVideo);
        settings.setValue("video_codec", profile.videoCodec);
        settings.setValue("resolution", profile.resolution);
        settings.setValue("video_bitrate", profile.videoBitrate);
        settings.setValue("audio_codec", profile.audioCodec);
        settings.setValue("audio_bitrate", profile.audioBitrate);
        settings.setValue("audio_sample_rate", profile.audioSampleRate);
        settings.setValue("audio_channels", profile.audioChannels);
        settings.setValue("output_template", profile.outputTemplate);
        settings.setValue("output_dir", profile.outputDir);
    }

    settings.endArray();
}

void ProfilesDialog::updateProfileList()
{
    m_profileTree->clear();

    for (const EncoderProfile &profile : m_profiles) {
        QTreeWidgetItem *item = new QTreeWidgetItem();

        QString type = profile.noVideo ? "Только аудио" : "Видео + аудио";
        QString video = profile.noVideo ? "-" :
        profile.videoCodec + " " + profile.resolution;
        QString audio = profile.audioCodec;
        if (!profile.audioBitrate.isEmpty()) {
            audio += " " + profile.audioBitrate;
        }

        item->setText(0, profile.name);
        item->setText(1, type);
        item->setText(2, video);
        item->setText(3, audio);
        item->setText(4, profile.description);

        // Сохраняем полные данные профиля
        QVariant variant;
        variant.setValue(profile);
        item->setData(0, Qt::UserRole, variant);

        m_profileTree->addTopLevelItem(item);
    }
}

EncoderProfile ProfilesDialog::getCurrentProfileFromUI()
{
    EncoderProfile profile;
    // Здесь должен быть код для получения текущих настроек из mainwindow
    // Пока возвращаем тестовый профиль
    profile.name = "Новый профиль";
    profile.noVideo = false;
    profile.videoCodec = "hevc_rkmpp";
    profile.videoBitrate = "5M";
    profile.audioCodec = "copy";
    profile.outputTemplate = "{name}_{codec}_{res}.mp4";
    return profile;
}

void ProfilesDialog::onSaveProfile()
{
    bool ok;
    QString name = QInputDialog::getText(this,
                                         "Сохранить профиль",
                                         "Введите название профиля:",
                                         QLineEdit::Normal,
                                         "Мой профиль",
                                         &ok);

    if (!ok || name.isEmpty()) return;

    QString description = QInputDialog::getText(this,
                                                "Сохранить профиль",
                                                "Введите описание (необязательно):",
                                                QLineEdit::Normal,
                                                "",
                                                &ok);

    EncoderProfile profile = getCurrentProfileFromUI();
    profile.name = name;
    profile.description = description;

    // Проверяем, существует ли уже профиль с таким именем
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].name == name) {
            if (QMessageBox::question(this, "Подтверждение",
                "Профиль с таким именем уже существует. Перезаписать?")
                == QMessageBox::Yes) {
                m_profiles[i] = profile;
            saveProfiles();
            loadProfiles();
            return;
                } else {
                    return;
                }
        }
    }

    m_profiles.append(profile);
    saveProfiles();
    loadProfiles();

    QMessageBox::information(this, "Успешно", "Профиль сохранен");
}

void ProfilesDialog::onLoadProfile()
{
    auto selected = m_profileTree->selectedItems();
    if (selected.isEmpty()) return;

    QVariant variant = selected.first()->data(0, Qt::UserRole);
    EncoderProfile profile = variant.value<EncoderProfile>();

    emit profileLoaded(profile);
    accept();
}

void ProfilesDialog::onDeleteProfile()
{
    auto selected = m_profileTree->selectedItems();
    if (selected.isEmpty()) return;

    QString name = selected.first()->text(0);

    if (QMessageBox::question(this, "Подтверждение",
        "Удалить профиль '" + name + "'?") == QMessageBox::Yes) {

        for (int i = 0; i < m_profiles.size(); ++i) {
            if (m_profiles[i].name == name) {
                m_profiles.removeAt(i);
                break;
            }
        }

        saveProfiles();
    loadProfiles();
        }
}

void ProfilesDialog::onExportProfile()
{
    auto selected = m_profileTree->selectedItems();
    if (selected.isEmpty()) return;

    QVariant variant = selected.first()->data(0, Qt::UserRole);
    EncoderProfile profile = variant.value<EncoderProfile>();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Экспорт профиля",
                                                    QDir::homePath() + "/" + profile.name + ".json",
                                                    "JSON файлы (*.json)");

    if (fileName.isEmpty()) return;

    QJsonObject obj;
    obj["name"] = profile.name;
    obj["description"] = profile.description;
    obj["no_video"] = profile.noVideo;
    obj["video_codec"] = profile.videoCodec;
    obj["resolution"] = profile.resolution;
    obj["video_bitrate"] = profile.videoBitrate;
    obj["audio_codec"] = profile.audioCodec;
    obj["audio_bitrate"] = profile.audioBitrate;
    obj["audio_sample_rate"] = profile.audioSampleRate;
    obj["audio_channels"] = profile.audioChannels;
    obj["output_template"] = profile.outputTemplate;
    obj["output_dir"] = profile.outputDir;

    QJsonDocument doc(obj);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        QMessageBox::information(this, "Успешно", "Профиль экспортирован");
    }
}

void ProfilesDialog::onImportProfile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Импорт профиля",
                                                    QDir::homePath(),
                                                    "JSON файлы (*.json)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Неверный формат файла");
        return;
    }

    QJsonObject obj = doc.object();

    EncoderProfile profile;
    profile.name = obj["name"].toString();
    profile.description = obj["description"].toString();
    profile.noVideo = obj["no_video"].toBool();
    profile.videoCodec = obj["video_codec"].toString();
    profile.resolution = obj["resolution"].toString();
    profile.videoBitrate = obj["video_bitrate"].toString();
    profile.audioCodec = obj["audio_codec"].toString();
    profile.audioBitrate = obj["audio_bitrate"].toString();
    profile.audioSampleRate = obj["audio_sample_rate"].toString();
    profile.audioChannels = obj["audio_channels"].toString();
    profile.outputTemplate = obj["output_template"].toString();
    profile.outputDir = obj["output_dir"].toString();

    m_profiles.append(profile);
    saveProfiles();
    loadProfiles();

    QMessageBox::information(this, "Успешно", "Профиль импортирован");
}

void ProfilesDialog::onItemSelectionChanged()
{
    bool hasSelection = !m_profileTree->selectedItems().isEmpty();
    m_loadBtn->setEnabled(hasSelection);
    m_deleteBtn->setEnabled(hasSelection);
    m_exportBtn->setEnabled(hasSelection);
}
