/****************************************************************************
** Meta object code from reading C++ file 'conversionworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/conversionworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'conversionworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConversionWorker_t {
    QByteArrayData data[16];
    char stringdata0[136];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConversionWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConversionWorker_t qt_meta_stringdata_ConversionWorker = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ConversionWorker"
QT_MOC_LITERAL(1, 17, 7), // "started"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 8), // "progress"
QT_MOC_LITERAL(4, 35, 7), // "percent"
QT_MOC_LITERAL(5, 43, 5), // "speed"
QT_MOC_LITERAL(6, 49, 11), // "currentTime"
QT_MOC_LITERAL(7, 61, 9), // "totalTime"
QT_MOC_LITERAL(8, 71, 6), // "status"
QT_MOC_LITERAL(9, 78, 8), // "finished"
QT_MOC_LITERAL(10, 87, 7), // "success"
QT_MOC_LITERAL(11, 95, 7), // "message"
QT_MOC_LITERAL(12, 103, 10), // "outputPath"
QT_MOC_LITERAL(13, 114, 10), // "logMessage"
QT_MOC_LITERAL(14, 125, 5), // "start"
QT_MOC_LITERAL(15, 131, 4) // "stop"

    },
    "ConversionWorker\0started\0\0progress\0"
    "percent\0speed\0currentTime\0totalTime\0"
    "status\0finished\0success\0message\0"
    "outputPath\0logMessage\0start\0stop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConversionWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    5,   45,    2, 0x06 /* Public */,
       9,    3,   56,    2, 0x06 /* Public */,
      13,    1,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    0,   66,    2, 0x0a /* Public */,
      15,    0,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::QString,    4,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   10,   11,   12,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ConversionWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConversionWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->started(); break;
        case 1: _t->progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        case 2: _t->finished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->start(); break;
        case 5: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConversionWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversionWorker::started)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConversionWorker::*)(int , double , double , double , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversionWorker::progress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ConversionWorker::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversionWorker::finished)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ConversionWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversionWorker::logMessage)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ConversionWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ConversionWorker.data,
    qt_meta_data_ConversionWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConversionWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConversionWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConversionWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ConversionWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ConversionWorker::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConversionWorker::progress(int _t1, double _t2, double _t3, double _t4, const QString & _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConversionWorker::finished(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ConversionWorker::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
