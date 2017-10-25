/****************************************************************************
** Meta object code from reading C++ file 'rend_evolution.h'
**
** Created: Fri 1. Apr 15:50:41 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../header_files/rend_evolution.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rend_evolution.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RendEvolution[] = {

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
      21,   15,   14,   14, 0x0a,
      41,   37,   14,   14, 0x0a,
      60,   54,   14,   14, 0x0a,
      89,   77,   14,   14, 0x0a,
     122,  110,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RendEvolution[] = {
    "RendEvolution\0\0shape\0setShape(Shape)\0"
    "pen\0setPen(QPen)\0brush\0setBrush(QBrush)\0"
    "antialiased\0setAntialiased(bool)\0"
    "transformed\0setTransformed(bool)\0"
};

void RendEvolution::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RendEvolution *_t = static_cast<RendEvolution *>(_o);
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

const QMetaObjectExtraData RendEvolution::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RendEvolution::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RendEvolution,
      qt_meta_data_RendEvolution, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RendEvolution::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RendEvolution::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RendEvolution::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RendEvolution))
        return static_cast<void*>(const_cast< RendEvolution*>(this));
    return QWidget::qt_metacast(_clname);
}

int RendEvolution::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_EvolutionDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   16,   16,   16, 0x08,
      39,   16,   16,   16, 0x08,
      53,   16,   16,   16, 0x08,
      70,   16,   16,   16, 0x08,
      86,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EvolutionDialog[] = {
    "EvolutionDialog\0\0dialogClosed()\0open()\0"
    "displaystat()\0displayallstat()\0"
    "createlineage()\0randomizeparameters()\0"
};

void EvolutionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EvolutionDialog *_t = static_cast<EvolutionDialog *>(_o);
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->open(); break;
        case 2: _t->displaystat(); break;
        case 3: _t->displayallstat(); break;
        case 4: _t->createlineage(); break;
        case 5: _t->randomizeparameters(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EvolutionDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EvolutionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EvolutionDialog,
      qt_meta_data_EvolutionDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EvolutionDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EvolutionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EvolutionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EvolutionDialog))
        return static_cast<void*>(const_cast< EvolutionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EvolutionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void EvolutionDialog::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
