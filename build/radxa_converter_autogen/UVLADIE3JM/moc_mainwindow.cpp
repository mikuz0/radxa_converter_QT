/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[58];
    char stringdata0[811];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 8), // "showHelp"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 20), // "updateConnectionInfo"
QT_MOC_LITERAL(4, 42, 16), // "onConnectClicked"
QT_MOC_LITERAL(5, 59, 19), // "onDisconnectClicked"
QT_MOC_LITERAL(6, 79, 25), // "onConnectionStatusChanged"
QT_MOC_LITERAL(7, 105, 9), // "connected"
QT_MOC_LITERAL(8, 115, 7), // "message"
QT_MOC_LITERAL(9, 123, 15), // "showFileManager"
QT_MOC_LITERAL(10, 139, 14), // "onFileSelected"
QT_MOC_LITERAL(11, 154, 4), // "path"
QT_MOC_LITERAL(12, 159, 19), // "onDirectorySelected"
QT_MOC_LITERAL(13, 179, 19), // "onFilesAddedToBatch"
QT_MOC_LITERAL(14, 199, 5), // "files"
QT_MOC_LITERAL(15, 205, 16), // "onNoVideoChanged"
QT_MOC_LITERAL(16, 222, 7), // "checked"
QT_MOC_LITERAL(17, 230, 21), // "updateFileNamePreview"
QT_MOC_LITERAL(18, 252, 14), // "onCodecChanged"
QT_MOC_LITERAL(19, 267, 5), // "codec"
QT_MOC_LITERAL(20, 273, 15), // "startConversion"
QT_MOC_LITERAL(21, 289, 14), // "stopConversion"
QT_MOC_LITERAL(22, 304, 19), // "onConversionStarted"
QT_MOC_LITERAL(23, 324, 20), // "onConversionProgress"
QT_MOC_LITERAL(24, 345, 7), // "percent"
QT_MOC_LITERAL(25, 353, 5), // "speed"
QT_MOC_LITERAL(26, 359, 11), // "currentTime"
QT_MOC_LITERAL(27, 371, 9), // "totalTime"
QT_MOC_LITERAL(28, 381, 6), // "status"
QT_MOC_LITERAL(29, 388, 20), // "onConversionFinished"
QT_MOC_LITERAL(30, 409, 7), // "success"
QT_MOC_LITERAL(31, 417, 10), // "outputPath"
QT_MOC_LITERAL(32, 428, 14), // "showBatchQueue"
QT_MOC_LITERAL(33, 443, 15), // "clearBatchQueue"
QT_MOC_LITERAL(34, 459, 20), // "startBatchConversion"
QT_MOC_LITERAL(35, 480, 19), // "stopBatchConversion"
QT_MOC_LITERAL(36, 500, 15), // "updateQueueInfo"
QT_MOC_LITERAL(37, 516, 18), // "processNextInBatch"
QT_MOC_LITERAL(38, 535, 11), // "saveProfile"
QT_MOC_LITERAL(39, 547, 11), // "loadProfile"
QT_MOC_LITERAL(40, 559, 14), // "EncoderProfile"
QT_MOC_LITERAL(41, 574, 7), // "profile"
QT_MOC_LITERAL(42, 582, 18), // "showProfilesDialog"
QT_MOC_LITERAL(43, 601, 22), // "showConnectionSettings"
QT_MOC_LITERAL(44, 624, 12), // "saveSettings"
QT_MOC_LITERAL(45, 637, 12), // "loadSettings"
QT_MOC_LITERAL(46, 650, 13), // "resetSettings"
QT_MOC_LITERAL(47, 664, 9), // "appendLog"
QT_MOC_LITERAL(48, 674, 8), // "clearLog"
QT_MOC_LITERAL(49, 683, 19), // "toggleLogVisibility"
QT_MOC_LITERAL(50, 703, 13), // "saveLogToFile"
QT_MOC_LITERAL(51, 717, 9), // "showAbout"
QT_MOC_LITERAL(52, 727, 16), // "estimateFileSize"
QT_MOC_LITERAL(53, 744, 12), // "showFileInfo"
QT_MOC_LITERAL(54, 757, 12), // "downloadFile"
QT_MOC_LITERAL(55, 770, 10), // "remotePath"
QT_MOC_LITERAL(56, 781, 23), // "updateBatchItemProgress"
QT_MOC_LITERAL(57, 805, 5) // "index"

    },
    "MainWindow\0showHelp\0\0updateConnectionInfo\0"
    "onConnectClicked\0onDisconnectClicked\0"
    "onConnectionStatusChanged\0connected\0"
    "message\0showFileManager\0onFileSelected\0"
    "path\0onDirectorySelected\0onFilesAddedToBatch\0"
    "files\0onNoVideoChanged\0checked\0"
    "updateFileNamePreview\0onCodecChanged\0"
    "codec\0startConversion\0stopConversion\0"
    "onConversionStarted\0onConversionProgress\0"
    "percent\0speed\0currentTime\0totalTime\0"
    "status\0onConversionFinished\0success\0"
    "outputPath\0showBatchQueue\0clearBatchQueue\0"
    "startBatchConversion\0stopBatchConversion\0"
    "updateQueueInfo\0processNextInBatch\0"
    "saveProfile\0loadProfile\0EncoderProfile\0"
    "profile\0showProfilesDialog\0"
    "showConnectionSettings\0saveSettings\0"
    "loadSettings\0resetSettings\0appendLog\0"
    "clearLog\0toggleLogVisibility\0saveLogToFile\0"
    "showAbout\0estimateFileSize\0showFileInfo\0"
    "downloadFile\0remotePath\0updateBatchItemProgress\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  209,    2, 0x08 /* Private */,
       3,    0,  210,    2, 0x08 /* Private */,
       4,    0,  211,    2, 0x08 /* Private */,
       5,    0,  212,    2, 0x08 /* Private */,
       6,    2,  213,    2, 0x08 /* Private */,
       9,    0,  218,    2, 0x08 /* Private */,
      10,    1,  219,    2, 0x08 /* Private */,
      12,    1,  222,    2, 0x08 /* Private */,
      13,    1,  225,    2, 0x08 /* Private */,
      15,    1,  228,    2, 0x08 /* Private */,
      17,    0,  231,    2, 0x08 /* Private */,
      18,    1,  232,    2, 0x08 /* Private */,
      20,    0,  235,    2, 0x08 /* Private */,
      21,    0,  236,    2, 0x08 /* Private */,
      22,    0,  237,    2, 0x08 /* Private */,
      23,    5,  238,    2, 0x08 /* Private */,
      29,    3,  249,    2, 0x08 /* Private */,
      32,    0,  256,    2, 0x08 /* Private */,
      33,    0,  257,    2, 0x08 /* Private */,
      34,    0,  258,    2, 0x08 /* Private */,
      35,    0,  259,    2, 0x08 /* Private */,
      36,    0,  260,    2, 0x08 /* Private */,
      37,    0,  261,    2, 0x08 /* Private */,
      38,    0,  262,    2, 0x08 /* Private */,
      39,    1,  263,    2, 0x08 /* Private */,
      42,    0,  266,    2, 0x08 /* Private */,
      43,    0,  267,    2, 0x08 /* Private */,
      44,    0,  268,    2, 0x08 /* Private */,
      45,    0,  269,    2, 0x08 /* Private */,
      46,    0,  270,    2, 0x08 /* Private */,
      47,    1,  271,    2, 0x08 /* Private */,
      48,    0,  274,    2, 0x08 /* Private */,
      49,    0,  275,    2, 0x08 /* Private */,
      50,    0,  276,    2, 0x08 /* Private */,
      51,    0,  277,    2, 0x08 /* Private */,
      52,    0,  278,    2, 0x08 /* Private */,
      53,    0,  279,    2, 0x08 /* Private */,
      54,    1,  280,    2, 0x08 /* Private */,
      56,    3,  283,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QStringList,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::QString,   24,   25,   26,   27,   28,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   30,    8,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,   57,   24,   28,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showHelp(); break;
        case 1: _t->updateConnectionInfo(); break;
        case 2: _t->onConnectClicked(); break;
        case 3: _t->onDisconnectClicked(); break;
        case 4: _t->onConnectionStatusChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->showFileManager(); break;
        case 6: _t->onFileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onDirectorySelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onFilesAddedToBatch((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 9: _t->onNoVideoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->updateFileNamePreview(); break;
        case 11: _t->onCodecChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->startConversion(); break;
        case 13: _t->stopConversion(); break;
        case 14: _t->onConversionStarted(); break;
        case 15: _t->onConversionProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        case 16: _t->onConversionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 17: _t->showBatchQueue(); break;
        case 18: _t->clearBatchQueue(); break;
        case 19: _t->startBatchConversion(); break;
        case 20: _t->stopBatchConversion(); break;
        case 21: _t->updateQueueInfo(); break;
        case 22: _t->processNextInBatch(); break;
        case 23: _t->saveProfile(); break;
        case 24: _t->loadProfile((*reinterpret_cast< const EncoderProfile(*)>(_a[1]))); break;
        case 25: _t->showProfilesDialog(); break;
        case 26: _t->showConnectionSettings(); break;
        case 27: _t->saveSettings(); break;
        case 28: _t->loadSettings(); break;
        case 29: _t->resetSettings(); break;
        case 30: _t->appendLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 31: _t->clearLog(); break;
        case 32: _t->toggleLogVisibility(); break;
        case 33: _t->saveLogToFile(); break;
        case 34: _t->showAbout(); break;
        case 35: _t->estimateFileSize(); break;
        case 36: _t->showFileInfo(); break;
        case 37: _t->downloadFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 38: _t->updateBatchItemProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EncoderProfile >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
