#ifndef PROFILESDIALOG_H
#define PROFILESDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QJsonObject>
#include "settingsmanager.h"

class ProfilesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfilesDialog(QWidget *parent = nullptr);

signals:
    void profileLoaded(const EncoderProfile &profile);

private slots:
    void onSaveProfile();
    void onLoadProfile();
    void onDeleteProfile();
    void onExportProfile();
    void onImportProfile();
    void onItemSelectionChanged();

private:
    void loadProfiles();
    void saveProfiles();
    void updateProfileList();
    EncoderProfile getCurrentProfileFromUI();

    QTreeWidget *m_profileTree;
    QPushButton *m_loadBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_importBtn;
    QList<EncoderProfile> m_profiles;
};

#endif // PROFILESDIALOG_H
