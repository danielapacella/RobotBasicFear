/****************************************************************************
** Meta object code from reading C++ file 'rend_controller.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../header_files/rend_controller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rend_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RendNetwork[] = {

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
      19,   13,   12,   12, 0x0a,
      39,   35,   12,   12, 0x0a,
      58,   52,   12,   12, 0x0a,
      87,   75,   12,   12, 0x0a,
     120,  108,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RendNetwork[] = {
    "RendNetwork\0\0shape\0setShape(Shape)\0"
    "pen\0setPen(QPen)\0brush\0setBrush(QBrush)\0"
    "antialiased\0setAntialiased(bool)\0"
    "transformed\0setTransformed(bool)\0"
};

void RendNetwork::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RendNetwork *_t = static_cast<RendNetwork *>(_o);
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

const QMetaObjectExtraData RendNetwork::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RendNetwork::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RendNetwork,
      qt_meta_data_RendNetwork, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RendNetwork::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RendNetwork::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RendNetwork::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RendNetwork))
        return static_cast<void*>(const_cast< RendNetwork*>(this));
    return QWidget::qt_metacast(_clname);
}

int RendNetwork::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_RendControlstate[] = {

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
      24,   18,   17,   17, 0x0a,
      44,   40,   17,   17, 0x0a,
      63,   57,   17,   17, 0x0a,
      92,   80,   17,   17, 0x0a,
     125,  113,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RendControlstate[] = {
    "RendControlstate\0\0shape\0setShape(Shape)\0"
    "pen\0setPen(QPen)\0brush\0setBrush(QBrush)\0"
    "antialiased\0setAntialiased(bool)\0"
    "transformed\0setTransformed(bool)\0"
};

void RendControlstate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RendControlstate *_t = static_cast<RendControlstate *>(_o);
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

const QMetaObjectExtraData RendControlstate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RendControlstate::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RendControlstate,
      qt_meta_data_RendControlstate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RendControlstate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RendControlstate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RendControlstate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RendControlstate))
        return static_cast<void*>(const_cast< RendControlstate*>(this));
    return QWidget::qt_metacast(_clname);
}

int RendControlstate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_NetworkDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   14,   14,   14, 0x08,
      50,   14,   14,   14, 0x08,
      67,   14,   14,   14, 0x08,
      85,   14,   14,   14, 0x08,
     102,   14,   14,   14, 0x08,
     115,   14,   14,   14, 0x08,
     131,   14,   14,   14, 0x08,
     148,   14,   14,   14, 0x08,
     164,   14,   14,   14, 0x08,
     179,   14,   14,   14, 0x08,
     194,   14,   14,   14, 0x08,
     211,   14,   14,   14, 0x08,
     223,   14,   14,   14, 0x08,
     236,   14,   14,   14, 0x08,
     249,   14,   14,   14, 0x08,
     262,   14,   14,   14, 0x08,
     274,   14,   14,   14, 0x08,
     286,   14,   14,   14, 0x08,
     293,   14,   14,   14, 0x08,
     300,   14,   14,   14, 0x08,
     312,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NetworkDialog[] = {
    "NetworkDialog\0\0dialogClosed()\0"
    "set_neurondisplay()\0set_neuronbias()\0"
    "set_neurondelta()\0set_neurongain()\0"
    "set_lesion()\0display_label()\0"
    "display_weight()\0display_delta()\0"
    "display_bias()\0display_gain()\0"
    "display_blocks()\0erase_net()\0add_ublock()\0"
    "add_cblock()\0add_gblock()\0increasev()\0"
    "decreasev()\0open()\0save()\0downloadc()\0"
    "randomfreep()\0"
};

void NetworkDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkDialog *_t = static_cast<NetworkDialog *>(_o);
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->set_neurondisplay(); break;
        case 2: _t->set_neuronbias(); break;
        case 3: _t->set_neurondelta(); break;
        case 4: _t->set_neurongain(); break;
        case 5: _t->set_lesion(); break;
        case 6: _t->display_label(); break;
        case 7: _t->display_weight(); break;
        case 8: _t->display_delta(); break;
        case 9: _t->display_bias(); break;
        case 10: _t->display_gain(); break;
        case 11: _t->display_blocks(); break;
        case 12: _t->erase_net(); break;
        case 13: _t->add_ublock(); break;
        case 14: _t->add_cblock(); break;
        case 15: _t->add_gblock(); break;
        case 16: _t->increasev(); break;
        case 17: _t->decreasev(); break;
        case 18: _t->open(); break;
        case 19: _t->save(); break;
        case 20: _t->downloadc(); break;
        case 21: _t->randomfreep(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData NetworkDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetworkDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NetworkDialog,
      qt_meta_data_NetworkDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkDialog))
        return static_cast<void*>(const_cast< NetworkDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int NetworkDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void NetworkDialog::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
