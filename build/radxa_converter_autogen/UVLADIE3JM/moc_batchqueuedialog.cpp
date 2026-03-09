/****************************************************************************
** Meta object code from reading C++ file 'batchqueuedialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/batchqueuedialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'batchqueuedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BatchQueueDialog_t {
    QByteArrayData data[12];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BatchQueueDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BatchQueueDialog_t qt_meta_stringdata_BatchQueueDialog = {
    {
QT_MOC_LITERAL(0, 0, 16), // "BatchQueueDialog"
QT_MOC_LITERAL(1, 17, 12), // "queueUpdated"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 10), // "startBatch"
QT_MOC_LITERAL(4, 42, 9), // "stopBatch"
QT_MOC_LITERAL(5, 52, 16), // "onRemoveSelected"
QT_MOC_LITERAL(6, 69, 12), // "onClearQueue"
QT_MOC_LITERAL(7, 82, 8), // "onMoveUp"
QT_MOC_LITERAL(8, 91, 10), // "onMoveDown"
QT_MOC_LITERAL(9, 102, 12), // "onStartBatch"
QT_MOC_LITERAL(10, 115, 11), // "onStopBatch"
QT_MOC_LITERAL(11, 127, 22) // "onItemSelectionChanged"

    },
    "BatchQueueDialog\0queueUpdated\0\0"
    "startBatch\0stopBatch\0onRemoveSelected\0"
    "onClearQueue\0onMoveUp\0onMoveDown\0"
    "onStartBatch\0onStopBatch\0"
    "onItemSelectionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BatchQueueDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void BatchQueueDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BatchQueueDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->queueUpdated(); break;
        case 1: _t->startBatch(); break;
        case 2: _t->stopBatch(); break;
        case 3: _t->onRemoveSelected(); break;
        case 4: _t->onClearQueue(); break;
        case 5: _t->onMoveUp(); break;
        case 6: _t->onMoveDown(); break;
        case 7: _t->onStartBatch(); break;
        case 8: _t->onStopBatch(); break;
        case 9: _t->onItemSelectionChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BatchQueueDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BatchQueueDialog::queueUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BatchQueueDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BatchQueueDialog::startBatch)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BatchQueueDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BatchQueueDialog::stopBatch)) {
                *result = 2;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject BatchQueueDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_BatchQueueDialog.data,
    qt_meta_data_BatchQueueDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BatchQueueDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BatchQueueDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BatchQueueDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int BatchQueueDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void BatchQueueDialog::queueUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BatchQueueDialog::startBatch()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void BatchQueueDialog::stopBatch()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
