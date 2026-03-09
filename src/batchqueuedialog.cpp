#include "batchqueuedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSpinBox>

// Вспомогательная функция для форматирования размера
static QString formatSize(qint64 size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double s = size;

    while (s >= 1024 && unitIndex < 4) {
        s /= 1024;
        unitIndex++;
    }

    return QString::number(s, 'f', 1) + " " + units[unitIndex];
}

BatchQueueDialog::BatchQueueDialog(QList<BatchItem> *queue, QWidget *parent)
: QDialog(parent)
, m_queue(queue)
{
    setWindowTitle("Очередь пакетной обработки");
    resize(700, 600);  // Увеличиваем высоту для новых элементов

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel("Очередь файлов для обработки");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Таблица
    m_table = new QTableWidget;
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Имя файла", "Статус", "Размер"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &BatchQueueDialog::onItemSelectionChanged);

    mainLayout->addWidget(m_table);

    // Общий прогресс
    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(new QLabel("Общий прогресс:"));
    m_overallProgress = new QProgressBar;
    m_overallProgress->setRange(0, 100);
    progressLayout->addWidget(m_overallProgress);
    mainLayout->addLayout(progressLayout);





    // Кнопки управления
    QHBoxLayout *btnLayout = new QHBoxLayout;

    m_startBtn = new QPushButton("▶ Запустить");
    m_startBtn->setStyleSheet("background-color: #4CAF50; color: white;");
    connect(m_startBtn, &QPushButton::clicked, this, &BatchQueueDialog::onStartBatch);
    btnLayout->addWidget(m_startBtn);

    m_stopBtn = new QPushButton("■ Остановить");
    m_stopBtn->setStyleSheet("background-color: #f44336; color: white;");
    m_stopBtn->setEnabled(false);
    connect(m_stopBtn, &QPushButton::clicked, this, &BatchQueueDialog::onStopBatch);
    btnLayout->addWidget(m_stopBtn);

    btnLayout->addSpacing(20);

    m_upBtn = new QPushButton("↑ Вверх");
    connect(m_upBtn, &QPushButton::clicked, this, &BatchQueueDialog::onMoveUp);
    btnLayout->addWidget(m_upBtn);

    m_downBtn = new QPushButton("↓ Вниз");
    connect(m_downBtn, &QPushButton::clicked, this, &BatchQueueDialog::onMoveDown);
    btnLayout->addWidget(m_downBtn);

    btnLayout->addSpacing(20);

    m_removeBtn = new QPushButton("✖ Удалить");
    m_removeBtn->setStyleSheet("background-color: #ff9800; color: white;");
    connect(m_removeBtn, &QPushButton::clicked, this, &BatchQueueDialog::onRemoveSelected);
    btnLayout->addWidget(m_removeBtn);

    m_clearBtn = new QPushButton("🗑 Очистить все");
    m_clearBtn->setStyleSheet("background-color: #f44336; color: white;");
    connect(m_clearBtn, &QPushButton::clicked, this, &BatchQueueDialog::onClearQueue);
    btnLayout->addWidget(m_clearBtn);

    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Статус
    m_statusLabel = new QLabel;
    mainLayout->addWidget(m_statusLabel);

    m_totalSizeLabel = new QLabel;
    mainLayout->addWidget(m_totalSizeLabel);

    updateDisplay();
}

//end
//begin
void BatchQueueDialog::updateDisplay()
{
    m_table->setRowCount(m_queue->size());

    qint64 totalSize = 0;
    int done = 0;
    int failed = 0;
    int processing = 0;

    for (int i = 0; i < m_queue->size(); ++i) {
        const BatchItem &item = m_queue->at(i);

        // Имя файла
        m_table->setItem(i, 0, new QTableWidgetItem(item.name));

        // Статус с прогрессом
        QString statusText = item.status;
        if (item.status == "processing" && item.progress > 0) {
            statusText = QString("processing (%1%)").arg(item.progress);
        }

        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        if (item.status == "done") {
            statusItem->setForeground(Qt::darkGreen);
            done++;
        } else if (item.status == "failed") {
            statusItem->setForeground(Qt::red);
            failed++;
        } else if (item.status == "processing") {
            statusItem->setForeground(Qt::blue);
            processing++;
        }
        m_table->setItem(i, 1, statusItem);

        // Размер
        QString sizeStr;
        if (item.status == "done" && item.size > 0) {
            sizeStr = formatSize(item.size);
            totalSize += item.size;
        } else {
            sizeStr = "N/A";
        }
        m_table->setItem(i, 2, new QTableWidgetItem(sizeStr));
    }

    // Обновляем общий прогресс
    if (m_queue->size() > 0) {
        int progress = ((done + failed) * 100) / m_queue->size();
        m_overallProgress->setValue(progress);
    }

    // Обновляем статус
    m_statusLabel->setText(
        QString("Всего: %1 | Готово: %2 | Ошибок: %3 | В обработке: %4")
        .arg(m_queue->size()).arg(done).arg(failed).arg(processing)
    );

    m_totalSizeLabel->setText("Общий размер обработанных: " + formatSize(totalSize));

    // Обновляем состояние кнопок
    bool hasSelection = !m_table->selectedItems().isEmpty();
    m_removeBtn->setEnabled(hasSelection);
    m_upBtn->setEnabled(hasSelection);
    m_downBtn->setEnabled(hasSelection);
}

void BatchQueueDialog::onRemoveSelected()
{
    QList<int> rowsToRemove;
    auto selectedItems = m_table->selectedItems();

    for (auto item : selectedItems) {
        int row = item->row();
        if (!rowsToRemove.contains(row)) {
            rowsToRemove.append(row);
        }
    }

    // Удаляем в обратном порядке
    std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());

    for (int row : rowsToRemove) {
        m_queue->removeAt(row);
    }

    emit queueUpdated();
    updateDisplay();
}

void BatchQueueDialog::onClearQueue()
{
    if (QMessageBox::question(this, "Подтверждение",
        "Очистить всю очередь?") == QMessageBox::Yes) {
        m_queue->clear();
    emit queueUpdated();
    updateDisplay();
        }
}

void BatchQueueDialog::onMoveUp()
{
    auto selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    if (row > 0) {
        m_queue->swapItemsAt(row, row - 1);
        updateDisplay();
        m_table->selectRow(row - 1);
    }
}

void BatchQueueDialog::onMoveDown()
{
    auto selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    if (row < m_queue->size() - 1) {
        m_queue->swapItemsAt(row, row + 1);
        updateDisplay();
        m_table->selectRow(row + 1);
    }
}

void BatchQueueDialog::onStartBatch()
{
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    emit startBatch();
}

void BatchQueueDialog::onStopBatch()
{
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    emit stopBatch();
}

void BatchQueueDialog::onItemSelectionChanged()
{
    bool hasSelection = !m_table->selectedItems().isEmpty();
    m_removeBtn->setEnabled(hasSelection);
    m_upBtn->setEnabled(hasSelection);
    m_downBtn->setEnabled(hasSelection);
}


