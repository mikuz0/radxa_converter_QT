/****************************************************************************
** Meta object code from reading C++ file 'filemanagerdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/filemanagerdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filemanagerdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileManagerDialog_t {
    QByteArrayData data[26];
    char stringdata0[307];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileManagerDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileManagerDialog_t qt_meta_stringdata_FileManagerDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "FileManagerDialog"
QT_MOC_LITERAL(1, 18, 12), // "fileSelected"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "path"
QT_MOC_LITERAL(4, 37, 17), // "directorySelected"
QT_MOC_LITERAL(5, 55, 17), // "filesAddedToBatch"
QT_MOC_LITERAL(6, 73, 5), // "files"
QT_MOC_LITERAL(7, 79, 11), // "refreshList"
QT_MOC_LITERAL(8, 91, 14), // "navigateToPath"
QT_MOC_LITERAL(9, 106, 4), // "goUp"
QT_MOC_LITERAL(10, 111, 6), // "goHome"
QT_MOC_LITERAL(11, 118, 19), // "onItemDoubleClicked"
QT_MOC_LITERAL(12, 138, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(13, 155, 4), // "item"
QT_MOC_LITERAL(14, 160, 6), // "column"
QT_MOC_LITERAL(15, 167, 14), // "onItemSelected"
QT_MOC_LITERAL(16, 182, 14), // "selectAsSource"
QT_MOC_LITERAL(17, 197, 17), // "selectAsOutputDir"
QT_MOC_LITERAL(18, 215, 10), // "addToBatch"
QT_MOC_LITERAL(19, 226, 10), // "uploadFile"
QT_MOC_LITERAL(20, 237, 12), // "downloadFile"
QT_MOC_LITERAL(21, 250, 15), // "createDirectory"
QT_MOC_LITERAL(22, 266, 10), // "deleteItem"
QT_MOC_LITERAL(23, 277, 12), // "showFileInfo"
QT_MOC_LITERAL(24, 290, 11), // "filterFiles"
QT_MOC_LITERAL(25, 302, 4) // "text"

    },
    "FileManagerDialog\0fileSelected\0\0path\0"
    "directorySelected\0filesAddedToBatch\0"
    "files\0refreshList\0navigateToPath\0goUp\0"
    "goHome\0onItemDoubleClicked\0QTreeWidgetItem*\0"
    "item\0column\0onItemSelected\0selectAsSource\0"
    "selectAsOutputDir\0addToBatch\0uploadFile\0"
    "downloadFile\0createDirectory\0deleteItem\0"
    "showFileInfo\0filterFiles\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileManagerDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  104,    2, 0x06 /* Public */,
       4,    1,  107,    2, 0x06 /* Public */,
       5,    1,  110,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,  113,    2, 0x08 /* Private */,
       8,    0,  114,    2, 0x08 /* Private */,
       9,    0,  115,    2, 0x08 /* Private */,
      10,    0,  116,    2, 0x08 /* Private */,
      11,    2,  117,    2, 0x08 /* Private */,
      15,    0,  122,    2, 0x08 /* Private */,
      16,    0,  123,    2, 0x08 /* Private */,
      17,    0,  124,    2, 0x08 /* Private */,
      18,    0,  125,    2, 0x08 /* Private */,
      19,    0,  126,    2, 0x08 /* Private */,
      20,    0,  127,    2, 0x08 /* Private */,
      21,    0,  128,    2, 0x08 /* Private */,
      22,    0,  129,    2, 0x08 /* Private */,
      23,    0,  130,    2, 0x08 /* Private */,
      24,    1,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   25,

       0        // eod
};

void FileManagerDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileManagerDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->directorySelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->filesAddedToBatch((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 3: _t->refreshList(); break;
        case 4: _t->navigateToPath(); break;
        case 5: _t->goUp(); break;
        case 6: _t->goHome(); break;
        case 7: _t->onItemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->onItemSelected(); break;
        case 9: _t->selectAsSource(); break;
        case 10: _t->selectAsOutputDir(); break;
        case 11: _t->addToBatch(); break;
        case 12: _t->uploadFile(); break;
        case 13: _t->downloadFile(); break;
        case 14: _t->createDirectory(); break;
        case 15: _t->deleteItem(); break;
        case 16: _t->showFileInfo(); break;
        case 17: _t->filterFiles((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileManagerDialog::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManagerDialog::fileSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileManagerDialog::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManagerDialog::directorySelected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileManagerDialog::*)(const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManagerDialog::filesAddedToBatch)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileManagerDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FileManagerDialog.data,
    qt_meta_data_FileManagerDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileManagerDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileManagerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileManagerDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FileManagerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void FileManagerDialog::fileSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileManagerDialog::directorySelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FileManagerDialog::filesAddedToBatch(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
