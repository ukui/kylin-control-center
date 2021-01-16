/****************************************************************************
** Meta object code from reading C++ file 'tabwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/tabwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabWid_t {
    QByteArrayData data[27];
    char stringdata0[380];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabWid_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabWid_t qt_meta_stringdata_TabWid = {
    {
QT_MOC_LITERAL(0, 0, 6), // "TabWid"
QT_MOC_LITERAL(1, 7, 15), // "updateAllSignal"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "needBackUp"
QT_MOC_LITERAL(4, 35, 11), // "startBackUp"
QT_MOC_LITERAL(5, 47, 17), // "showHistoryWidget"
QT_MOC_LITERAL(6, 65, 20), // "isAutoCheckedChanged"
QT_MOC_LITERAL(7, 86, 18), // "slotCancelDownload"
QT_MOC_LITERAL(8, 105, 16), // "slotGetImportant"
QT_MOC_LITERAL(9, 122, 4), // "args"
QT_MOC_LITERAL(10, 127, 23), // "loadingOneUpdateMsgSlot"
QT_MOC_LITERAL(11, 151, 9), // "AppAllMsg"
QT_MOC_LITERAL(12, 161, 3), // "msg"
QT_MOC_LITERAL(13, 165, 19), // "loadingFinishedSlot"
QT_MOC_LITERAL(14, 185, 4), // "size"
QT_MOC_LITERAL(15, 190, 18), // "downloadFailedSlot"
QT_MOC_LITERAL(16, 209, 8), // "exitCode"
QT_MOC_LITERAL(17, 218, 20), // "waitCrucialInstalled"
QT_MOC_LITERAL(18, 239, 17), // "hideUpdateBtnSlot"
QT_MOC_LITERAL(19, 257, 19), // "changeUpdateAllSlot"
QT_MOC_LITERAL(20, 277, 18), // "slotUpdateTemplate"
QT_MOC_LITERAL(21, 296, 6), // "status"
QT_MOC_LITERAL(22, 303, 15), // "slotUpdateCache"
QT_MOC_LITERAL(23, 319, 23), // "slotUpdateCacheProgress"
QT_MOC_LITERAL(24, 343, 8), // "progress"
QT_MOC_LITERAL(25, 352, 12), // "bakeupFinish"
QT_MOC_LITERAL(26, 365, 14) // "backupProgress"

    },
    "TabWid\0updateAllSignal\0\0needBackUp\0"
    "startBackUp\0showHistoryWidget\0"
    "isAutoCheckedChanged\0slotCancelDownload\0"
    "slotGetImportant\0args\0loadingOneUpdateMsgSlot\0"
    "AppAllMsg\0msg\0loadingFinishedSlot\0"
    "size\0downloadFailedSlot\0exitCode\0"
    "waitCrucialInstalled\0hideUpdateBtnSlot\0"
    "changeUpdateAllSlot\0slotUpdateTemplate\0"
    "status\0slotUpdateCache\0slotUpdateCacheProgress\0"
    "progress\0bakeupFinish\0backupProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabWid[] = {

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
       1,    0,  104,    2, 0x06 /* Public */,
       3,    0,  105,    2, 0x06 /* Public */,
       4,    1,  106,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  109,    2, 0x0a /* Public */,
       6,    0,  110,    2, 0x0a /* Public */,
       7,    0,  111,    2, 0x0a /* Public */,
       8,    1,  112,    2, 0x0a /* Public */,
      10,    1,  115,    2, 0x0a /* Public */,
      13,    1,  118,    2, 0x0a /* Public */,
      15,    1,  121,    2, 0x0a /* Public */,
      17,    0,  124,    2, 0x0a /* Public */,
      18,    0,  125,    2, 0x0a /* Public */,
      19,    0,  126,    2, 0x0a /* Public */,
      20,    1,  127,    2, 0x0a /* Public */,
      22,    1,  130,    2, 0x0a /* Public */,
      23,    1,  133,    2, 0x0a /* Public */,
      25,    1,  136,    2, 0x0a /* Public */,
      26,    1,  139,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   24,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void TabWid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabWid *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateAllSignal(); break;
        case 1: { int _r = _t->needBackUp();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->startBackUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showHistoryWidget(); break;
        case 4: _t->isAutoCheckedChanged(); break;
        case 5: _t->slotCancelDownload(); break;
        case 6: _t->slotGetImportant((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 7: _t->loadingOneUpdateMsgSlot((*reinterpret_cast< AppAllMsg(*)>(_a[1]))); break;
        case 8: _t->loadingFinishedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->downloadFailedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->waitCrucialInstalled(); break;
        case 11: _t->hideUpdateBtnSlot(); break;
        case 12: _t->changeUpdateAllSlot(); break;
        case 13: _t->slotUpdateTemplate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->slotUpdateCache((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->slotUpdateCacheProgress((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->bakeupFinish((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->backupProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< AppAllMsg >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TabWid::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWid::updateAllSignal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = int (TabWid::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWid::needBackUp)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TabWid::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWid::startBackUp)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TabWid::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_TabWid.data,
    qt_meta_data_TabWid,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabWid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabWid::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabWid.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TabWid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void TabWid::updateAllSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
int TabWid::needBackUp()
{
    int _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}

// SIGNAL 2
void TabWid::startBackUp(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
