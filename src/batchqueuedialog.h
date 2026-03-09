#ifndef BATCHQUEUEDIALOG_H
#define BATCHQUEUEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QSpinBox>
#include "mainwindow.h"

class BatchQueueDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BatchQueueDialog(QList<BatchItem> *queue, QWidget *parent = nullptr);
    void updateDisplay();



signals:
    void queueUpdated();
    void startBatch();
    void stopBatch();

private slots:
    void onRemoveSelected();
    void onClearQueue();
    void onMoveUp();
    void onMoveDown();
    void onStartBatch();
    void onStopBatch();
    void onItemSelectionChanged();

private:
    QList<BatchItem> *m_queue;
    QTableWidget *m_table;
    QLabel *m_statusLabel;
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_removeBtn;
    QPushButton *m_clearBtn;
    QPushButton *m_upBtn;
    QPushButton *m_downBtn;
    QLabel *m_totalSizeLabel;
    QProgressBar *m_overallProgress;

};

#endif // BATCHQUEUEDIALOG_H
