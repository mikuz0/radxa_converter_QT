#ifndef FILEMANAGERDIALOG_H
#define FILEMANAGERDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include "sshclient.h"

class FileManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileManagerDialog(SSHClient *sshClient, QWidget *parent = nullptr);

signals:
    void fileSelected(const QString &path);
    void directorySelected(const QString &path);
    void filesAddedToBatch(const QStringList &files);

private slots:
    void refreshList();
    void navigateToPath();
    void goUp();
    void goHome();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemSelected();
    void selectAsSource();
    void selectAsOutputDir();
    void addToBatch();
    void uploadFile();
    void downloadFile();
    void createDirectory();
    void deleteItem();
    void showFileInfo();
    void filterFiles(const QString &text);

private:
    void loadDirectory(const QString &path);
    QString formatSize(qint64 size) const;
    QString getFileIcon(const QString &filename, bool isDir);

    SSHClient *m_sshClient;
    QString m_currentPath;
    QStringList m_selectedFiles;

    QLineEdit *m_pathEdit;
    QTreeWidget *m_fileTree;
    QLineEdit *m_filterEdit;
    QLabel *m_statusLabel;
    QPushButton *m_selectBtn;
    QPushButton *m_selectDirBtn;
    QPushButton *m_addBatchBtn;
    QPushButton *m_uploadBtn;
    QPushButton *m_downloadBtn;
    QPushButton *m_mkdirBtn;
    QPushButton *m_deleteBtn;

    QPushButton *m_refreshBtn;
};

#endif // FILEMANAGERDIALOG_H
