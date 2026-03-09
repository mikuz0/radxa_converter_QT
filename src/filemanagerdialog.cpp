#include "filemanagerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QDateTime>
#include <libssh/sftp.h>
#include <libssh/libssh.h>
#include <QDebug>
#include <QProgressDialog>

FileManagerDialog::FileManagerDialog(SSHClient *sshClient, QWidget *parent)
: QDialog(parent)
, m_sshClient(sshClient)
, m_currentPath(sshClient->getHomeDirectory())  // Используем домашний каталог пользователя
{
    setWindowTitle("Файловый менеджер Radxa");
    resize(900, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Адресная строка
    QHBoxLayout *navLayout = new QHBoxLayout;
    navLayout->addWidget(new QLabel("Путь:"));

    m_pathEdit = new QLineEdit;
    m_pathEdit->setText(m_currentPath);
    connect(m_pathEdit, &QLineEdit::returnPressed, this, &FileManagerDialog::navigateToPath);
    navLayout->addWidget(m_pathEdit);

    QPushButton *goBtn = new QPushButton("Перейти");
    connect(goBtn, &QPushButton::clicked, this, &FileManagerDialog::navigateToPath);
    navLayout->addWidget(goBtn);

    QPushButton *upBtn = new QPushButton("▲ Вверх");
    connect(upBtn, &QPushButton::clicked, this, &FileManagerDialog::goUp);
    navLayout->addWidget(upBtn);

    QPushButton *homeBtn = new QPushButton("🏠 Домой");
    connect(homeBtn, &QPushButton::clicked, this, &FileManagerDialog::goHome);
    navLayout->addWidget(homeBtn);

    navLayout->addStretch();
    mainLayout->addLayout(navLayout);

    // Фильтр
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Фильтр:"));
    m_filterEdit = new QLineEdit;
    m_filterEdit->setPlaceholderText("Введите текст для фильтрации...");
    connect(m_filterEdit, &QLineEdit::textChanged, this, &FileManagerDialog::filterFiles);
    filterLayout->addWidget(m_filterEdit);
    mainLayout->addLayout(filterLayout);

    // Дерево файлов
    m_fileTree = new QTreeWidget;
    m_fileTree->setHeaderLabels({"Имя", "Размер", "Тип", "Дата изменения", "Права"});
    m_fileTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileTree->setAlternatingRowColors(true);
    m_fileTree->setSortingEnabled(true);
    m_fileTree->sortByColumn(0, Qt::AscendingOrder);

    m_fileTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_fileTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_fileTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_fileTree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_fileTree->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    connect(m_fileTree, &QTreeWidget::itemDoubleClicked, this, &FileManagerDialog::onItemDoubleClicked);
    connect(m_fileTree, &QTreeWidget::itemSelectionChanged, this, &FileManagerDialog::onItemSelected);

    mainLayout->addWidget(m_fileTree);

    // Кнопки действий
    QHBoxLayout *btnLayout = new QHBoxLayout;

    m_selectBtn = new QPushButton("📁 Выбрать как исходный");
    connect(m_selectBtn, &QPushButton::clicked, this, &FileManagerDialog::selectAsSource);
    btnLayout->addWidget(m_selectBtn);

    m_selectDirBtn = new QPushButton("📂 Выбрать как папку результата");
    connect(m_selectDirBtn, &QPushButton::clicked, this, &FileManagerDialog::selectAsOutputDir);
    btnLayout->addWidget(m_selectDirBtn);

    m_addBatchBtn = new QPushButton("➕ Добавить в очередь");
    connect(m_addBatchBtn, &QPushButton::clicked, this, &FileManagerDialog::addToBatch);
    btnLayout->addWidget(m_addBatchBtn);

    m_uploadBtn = new QPushButton("⬆ Загрузить с ПК");
    connect(m_uploadBtn, &QPushButton::clicked, this, &FileManagerDialog::uploadFile);
    btnLayout->addWidget(m_uploadBtn);

    m_downloadBtn = new QPushButton("⬇ Скачать на ПК");
    connect(m_downloadBtn, &QPushButton::clicked, this, &FileManagerDialog::downloadFile);
    btnLayout->addWidget(m_downloadBtn);

    m_mkdirBtn = new QPushButton("📁 Создать папку");
    connect(m_mkdirBtn, &QPushButton::clicked, this, &FileManagerDialog::createDirectory);
    btnLayout->addWidget(m_mkdirBtn);

    m_deleteBtn = new QPushButton("🗑 Удалить");
    connect(m_deleteBtn, &QPushButton::clicked, this, &FileManagerDialog::deleteItem);
    btnLayout->addWidget(m_deleteBtn);



    m_refreshBtn = new QPushButton("🔄 Обновить");
    connect(m_refreshBtn, &QPushButton::clicked, this, &FileManagerDialog::refreshList);
    btnLayout->addWidget(m_refreshBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Статус бар
    m_statusLabel = new QLabel;
    mainLayout->addWidget(m_statusLabel);

    // Загружаем список файлов
    refreshList();

    // Контекстное меню
    m_fileTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_fileTree, &QTreeWidget::customContextMenuRequested, [this](const QPoint &pos) {
        QMenu menu;
        menu.addAction("Выбрать как исходный", this, &FileManagerDialog::selectAsSource);
        menu.addAction("Выбрать как папку результата", this, &FileManagerDialog::selectAsOutputDir);
        menu.addAction("Добавить в очередь", this, &FileManagerDialog::addToBatch);
        menu.addSeparator();
        menu.addAction("Скачать", this, &FileManagerDialog::downloadFile);
        menu.addAction("Удалить", this, &FileManagerDialog::deleteItem);

        menu.exec(m_fileTree->viewport()->mapToGlobal(pos));
    });
}

void FileManagerDialog::refreshList()
{
    loadDirectory(m_currentPath);
}



void FileManagerDialog::goUp()
{
    if (m_currentPath == "/") return;

    // Находим родительский каталог
    QString parentPath = m_currentPath;
    if (parentPath.endsWith('/')) {
        parentPath.chop(1);
    }

    int lastSlash = parentPath.lastIndexOf('/');
    if (lastSlash > 0) {
        parentPath = parentPath.left(lastSlash);
    } else {
        parentPath = "/";
    }

    loadDirectory(parentPath);
}

void FileManagerDialog::goHome()
{
    loadDirectory(m_sshClient->getHomeDirectory());  // Используем домашний каталог пользователя
}

void FileManagerDialog::loadDirectory(const QString &path)
{
    if (!m_sshClient->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к Radxa");
        return;
    }

    m_statusLabel->setText("Загрузка " + path + "...");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();  // Обновляем интерфейс

    m_fileTree->clear();
    m_currentPath = path;
    m_pathEdit->setText(path);

    // Получаем сессию SSH
    ssh_session session = m_sshClient->getSession();
    if (!session) {
        m_statusLabel->setText("Ошибка: нет SSH сессии");
        QApplication::restoreOverrideCursor();
        return;
    }

    // Открываем SFTP сессию
    sftp_session sftp = sftp_new(session);
    if (!sftp) {
        m_statusLabel->setText("Ошибка создания SFTP сессии");
        QApplication::restoreOverrideCursor();
        return;
    }

    if (sftp_init(sftp) != SSH_OK) {
        sftp_free(sftp);
        m_statusLabel->setText("Ошибка инициализации SFTP");
        QApplication::restoreOverrideCursor();
        return;
    }

    // Открываем директорию
    sftp_dir dir = sftp_opendir(sftp, path.toStdString().c_str());
    if (!dir) {
        sftp_free(sftp);
        m_statusLabel->setText("Ошибка открытия директории: " + path);
        QApplication::restoreOverrideCursor();
        return;
    }

    // Читаем содержимое
    QList<QTreeWidgetItem*> items;
    sftp_attributes attributes;

    while ((attributes = sftp_readdir(sftp, dir)) != nullptr) {
        QString name = QString::fromUtf8(attributes->name);

        // Пропускаем . и ..
        if (name == "." || name == "..") {
            sftp_attributes_free(attributes);
            continue;
        }

        bool isDir = (attributes->type == SSH_FILEXFER_TYPE_DIRECTORY);
        QString size = isDir ? "<DIR>" : formatSize(attributes->size);
        QString type = isDir ? "Папка" : QFileInfo(name).suffix().toUpper();
        QString date = QDateTime::fromSecsSinceEpoch(attributes->mtime).toString("dd.MM.yyyy hh:mm");
        QString perms;

        // Форматируем права доступа
        if (!isDir) {
            perms += (attributes->permissions & 0400) ? "r" : "-";
            perms += (attributes->permissions & 0200) ? "w" : "-";
            perms += (attributes->permissions & 0100) ? "x" : "-";
            perms += (attributes->permissions & 0040) ? "r" : "-";
            perms += (attributes->permissions & 0020) ? "w" : "-";
            perms += (attributes->permissions & 0010) ? "x" : "-";
            perms += (attributes->permissions & 0004) ? "r" : "-";
            perms += (attributes->permissions & 0002) ? "w" : "-";
            perms += (attributes->permissions & 0001) ? "x" : "-";
        }

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, isDir ? name + "/" : name);
        item->setText(1, size);
        item->setText(2, type);
        item->setText(3, date);
        item->setText(4, perms);

        // Устанавливаем иконку
        item->setIcon(0, QIcon::fromTheme(isDir ? "folder" : "text-x-generic"));

        // Сохраняем полный путь
        QString fullPath;
        if (path == "/") {
            fullPath = "/" + name;
        } else if (path.endsWith('/')) {
            fullPath = path + name;
        } else {
            fullPath = path + "/" + name;
        }

        item->setData(0, Qt::UserRole, fullPath);
        item->setData(1, Qt::UserRole, isDir);

        items.append(item);

        // Периодически обрабатываем события
        if (items.size() % 10 == 0) {
            QApplication::processEvents();
        }

        sftp_attributes_free(attributes);
    }

    sftp_closedir(dir);
    sftp_free(sftp);

    // Добавляем элементы в дерево
    m_fileTree->addTopLevelItems(items);

    // Принудительно обновляем отображение
    m_fileTree->update();
    m_fileTree->repaint();
    m_fileTree->expandAll();  // Раскрываем все элементы (опционально)
    QApplication::processEvents();

    m_statusLabel->setText(QString("Загружено %1 элементов").arg(items.size()));
    QApplication::restoreOverrideCursor();
}
// 'nj  rjytw '
void FileManagerDialog::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    bool isDir = item->data(1, Qt::UserRole).toBool();
    QString fullPath = item->data(0, Qt::UserRole).toString();

    if (isDir) {
        // Убираем слеш в конце если он есть
        if (fullPath.endsWith('/')) {
            fullPath.chop(1);
        }
        loadDirectory(fullPath);
    }
}

void FileManagerDialog::onItemSelected()
{
    m_selectedFiles.clear();
    auto items = m_fileTree->selectedItems();

    for (auto item : items) {
        QString path = item->data(0, Qt::UserRole).toString();
        bool isDir = item->data(1, Qt::UserRole).toBool();
        if (!isDir) {
            m_selectedFiles.append(path);
        }
    }

    m_statusLabel->setText(QString("Выбрано: %1 файлов").arg(m_selectedFiles.size()));
}

void FileManagerDialog::selectAsSource()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите файл");
        return;
    }

    emit fileSelected(m_selectedFiles.first());
    accept();
}

void FileManagerDialog::selectAsOutputDir()
{
    // Если выбран файл, используем его директорию
    if (!m_selectedFiles.isEmpty()) {
        QFileInfo info(m_selectedFiles.first());
        emit directorySelected(info.path());
    } else {
        // Иначе используем текущую директорию
        emit directorySelected(m_currentPath);
    }
    accept();
}

void FileManagerDialog::addToBatch()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите файлы");
        return;
    }

    emit filesAddedToBatch(m_selectedFiles);
    QMessageBox::information(this, "Успешно",
                             QString("Добавлено %1 файлов в очередь").arg(m_selectedFiles.size()));
}

void FileManagerDialog::uploadFile()
{
    QString localFile = QFileDialog::getOpenFileName(this,
                                                     "Выберите файл для загрузки",
                                                     QDir::homePath());

    if (localFile.isEmpty()) return;

    QString remotePath = m_currentPath + "/" + QFileInfo(localFile).fileName();

    // Проверяем, существует ли уже такой файл
    if (m_sshClient->fileExists(remotePath)) {
        if (QMessageBox::question(this, "Файл существует",
            "Файл " + QFileInfo(localFile).fileName() + " уже существует. Перезаписать?")
            != QMessageBox::Yes) {
            return;
            }
    }

    m_statusLabel->setText("Загрузка " + QFileInfo(localFile).fileName() + "...");
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Создаем диалог прогресса
    QProgressDialog progressDialog("Загрузка файла...", "Отмена", 0, 100, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setValue(0);

    // Загружаем файл с callback'ом прогресса
    bool success = m_sshClient->uploadFile(localFile, remotePath,
                                           [&progressDialog](int progress) {
                                               progressDialog.setValue(progress);
                                               QApplication::processEvents();
                                           });

    QApplication::restoreOverrideCursor();
    progressDialog.close();

    if (success) {
        m_statusLabel->setText("Файл загружен");
        refreshList();
    } else {
        m_statusLabel->setText("Ошибка загрузки");
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл");
    }
}



void FileManagerDialog::downloadFile()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите файлы для скачивания");
        return;
    }

    QString saveDir = QFileDialog::getExistingDirectory(this,
                                                        "Выберите папку для сохранения",
                                                        QDir::homePath());

    if (saveDir.isEmpty()) return;

    for (const QString &remoteFile : m_selectedFiles) {
        QString fileName = QFileInfo(remoteFile).fileName();
        QString localPath = saveDir + "/" + fileName;

        // Проверяем, существует ли уже такой файл локально
        if (QFile::exists(localPath)) {
            if (QMessageBox::question(this, "Файл существует",
                "Файл " + fileName + " уже существует локально. Перезаписать?")
                != QMessageBox::Yes) {
                continue;
                }
        }

        m_statusLabel->setText("Скачивание " + fileName + "...");
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Создаем диалог прогресса
        QProgressDialog progressDialog("Скачивание " + fileName + "...", "Отмена", 0, 100, this);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setValue(0);

        // Скачиваем файл с callback'ом прогресса
        bool success = m_sshClient->downloadFile(remoteFile, localPath,
                                                 [&progressDialog](int progress) {
                                                     progressDialog.setValue(progress);
                                                     QApplication::processEvents();
                                                 });

        QApplication::restoreOverrideCursor();
        progressDialog.close();

        if (success) {
            m_statusLabel->setText("Файл " + fileName + " скачан");
        } else {
            m_statusLabel->setText("Ошибка скачивания " + fileName);
            QMessageBox::critical(this, "Ошибка", "Не удалось скачать файл " + fileName);
        }
    }
}


void FileManagerDialog::createDirectory()
{
    bool ok;
    QString dirName = QInputDialog::getText(this,
                                            "Создать папку",
                                            "Имя новой папки:",
                                            QLineEdit::Normal,
                                            "Новая папка",
                                            &ok);

    if (!ok || dirName.isEmpty()) return;

    QString newPath = m_currentPath + "/" + dirName;

    m_statusLabel->setText("Создание папки...");
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (m_sshClient->createDirectory(newPath)) {
        m_statusLabel->setText("Папка создана");
        refreshList();
    } else {
        m_statusLabel->setText("Ошибка создания папки");
        QMessageBox::critical(this, "Ошибка", "Не удалось создать папку");
    }

    QApplication::restoreOverrideCursor();
}

void FileManagerDialog::deleteItem()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите файлы для удаления");
        return;
    }

    QString message = m_selectedFiles.size() == 1
    ? "Удалить файл " + QFileInfo(m_selectedFiles.first()).fileName() + "?"
    : QString("Удалить %1 файлов?").arg(m_selectedFiles.size());

    if (QMessageBox::question(this, "Подтверждение", message) != QMessageBox::Yes) {
        return;
    }

    m_statusLabel->setText("Удаление...");
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool allSuccess = true;
    for (const QString &file : m_selectedFiles) {
        if (!m_sshClient->removeFile(file)) {
            allSuccess = false;
        }
    }

    QApplication::restoreOverrideCursor();

    if (allSuccess) {
        m_statusLabel->setText("Файлы удалены");
    } else {
        m_statusLabel->setText("Ошибка при удалении некоторых файлов");
    }

    refreshList();
}




void FileManagerDialog::filterFiles(const QString &text)
{
    for (int i = 0; i < m_fileTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_fileTree->topLevelItem(i);
        bool match = text.isEmpty() || item->text(0).contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

QString FileManagerDialog::formatSize(qint64 size) const
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double s = size;

    while (s >= 1024 && unitIndex < 4) {
        s /= 1024;
        unitIndex++;
    }

    return QString::number(s, 'f', 1) + " " + units[unitIndex];
}

void FileManagerDialog::navigateToPath()
{
    QString newPath = m_pathEdit->text().trimmed();
    if (newPath.isEmpty()) return;

    // Проверяем, существует ли путь
    ssh_session session = m_sshClient->getSession();
    if (!session) return;

    sftp_session sftp = sftp_new(session);
    if (!sftp) return;

    if (sftp_init(sftp) != SSH_OK) {
        sftp_free(sftp);
        return;
    }

    sftp_dir dir = sftp_opendir(sftp, newPath.toStdString().c_str());
    if (!dir) {
        sftp_free(sftp);
        QMessageBox::warning(this, "Ошибка", "Путь не существует: " + newPath);
        return;
    }

    sftp_closedir(dir);
    sftp_free(sftp);

    loadDirectory(newPath);
}

// Удалите или закомментируйте эту функцию полностью:
void FileManagerDialog::showFileInfo()
{
    if (m_selectedFiles.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Выберите файл");
        return;
    }

    QString path = m_selectedFiles.first();
    QFileInfo info(path);

    QString infoText = QString(
        "Имя: %1\n"
        "Путь: %2\n"
        "Размер: %3\n"
        "Тип: %4"
    ).arg(info.fileName())
    .arg(info.path())
    .arg(formatSize(info.size()))
    .arg(info.suffix().toUpper());

    QMessageBox::information(this, "Свойства файла", infoText);
}
