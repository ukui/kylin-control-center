/****************************************************************************
** Meta object code from reading C++ file 'm_updatelog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/m_updatelog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'm_updatelog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_m_updatelog_t {
    QByteArrayData data[8];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_m_updatelog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_m_updatelog_t qt_meta_stringdata_m_updatelog = {
    {
QT_MOC_LITERAL(0, 0, 11), // "m_updatelog"
QT_MOC_LITERAL(1, 12, 6), // "initUI"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 18), // "dynamicLoadingInit"
QT_MOC_LITERAL(4, 39, 14), // "dynamicLoading"
QT_MOC_LITERAL(5, 54, 1), // "i"
QT_MOC_LITERAL(6, 56, 9), // "updatesql"
QT_MOC_LITERAL(7, 66, 11) // "defaultItem"

    },
    "m_updatelog\0initUI\0\0dynamicLoadingInit\0"
    "dynamicLoading\0i\0updatesql\0defaultItem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_m_updatelog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    1,   41,    2, 0x08 /* Private */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void m_updatelog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<m_updatelog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initUI(); break;
        case 1: _t->dynamicLoadingInit(); break;
        case 2: _t->dynamicLoading((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updatesql(); break;
        case 4: _t->defaultItem(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject m_updatelog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_m_updatelog.data,
    qt_meta_data_m_updatelog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *m_updatelog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *m_updatelog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_m_updatelog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int m_updatelog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
