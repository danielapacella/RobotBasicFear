/****************************************************************************
** Meta object code from reading C++ file 'rend_param.h'
**
** Created: Fri 1. Apr 15:50:41 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../header_files/rend_param.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rend_param.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RendParam[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x0a,
      37,   33,   10,   10, 0x0a,
      56,   50,   10,   10, 0x0a,
      85,   73,   10,   10, 0x0a,
     118,  106,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RendParam[] = {
    "RendParam\0\0shape\0setShape(Shape)\0pen\0"
    "setPen(QPen)\0brush\0setBrush(QBrush)\0"
    "antialiased\0setAntialiased(bool)\0"
    "transformed\0setTransformed(bool)\0"
};

void RendParam::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RendParam *_t = static_cast<RendParam *>(_o);
        switch (_id) {
        case 0: _t->setShape((*reinterpret_cast< Shape(*)>(_a[1]))); break;
        case 1: _t->setPen((*reinterpret_cast< const QPen(*)>(_a[1]))); break;
        case 2: _t->setBrush((*reinterpret_cast< const QBrush(*)>(_a[1]))); break;
        case 3: _t->setAntialiased((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setTransformed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RendParam::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RendParam::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RendParam,
      qt_meta_data_RendParam, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RendParam::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RendParam::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RendParam::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RendParam))
        return static_cast<void*>(const_cast< RendParam*>(this));
    return QWidget::qt_metacast(_clname);
}

int RendParam::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_ParamDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   12,   12,   12, 0x08,
      46,   12,   12,   12, 0x08,
      57,   12,   12,   12, 0x08,
      75,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ParamDialog[] = {
    "ParamDialog\0\0dialogClosed()\0"
    "show_allfolders()\0parsetxt()\0"
    "loadandparsetxt()\0verbose_or_not()\0"
};

void ParamDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ParamDialog *_t = static_cast<ParamDialog *>(_o);
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->show_allfolders(); break;
        case 2: _t->parsetxt(); break;
        case 3: _t->loadandparsetxt(); break;
        case 4: _t->verbose_or_not(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ParamDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ParamDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ParamDialog,
      qt_meta_data_ParamDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ParamDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ParamDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ParamDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ParamDialog))
        return static_cast<void*>(const_cast< ParamDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ParamDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ParamDialog::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
