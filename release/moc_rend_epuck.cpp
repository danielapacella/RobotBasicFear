/****************************************************************************
** Meta object code from reading C++ file 'rend_epuck.h'
**
** Created: Fri 1. Apr 15:50:41 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../header_files/rend_epuck.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rend_epuck.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RendRobot[] = {

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

static const char qt_meta_stringdata_RendRobot[] = {
    "RendRobot\0\0shape\0setShape(Shape)\0pen\0"
    "setPen(QPen)\0brush\0setBrush(QBrush)\0"
    "antialiased\0setAntialiased(bool)\0"
    "transformed\0setTransformed(bool)\0"
};

void RendRobot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RendRobot *_t = static_cast<RendRobot *>(_o);
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

const QMetaObjectExtraData RendRobot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RendRobot::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RendRobot,
      qt_meta_data_RendRobot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RendRobot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RendRobot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RendRobot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RendRobot))
        return static_cast<void*>(const_cast< RendRobot*>(this));
    return QWidget::qt_metacast(_clname);
}

int RendRobot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_RobotDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   12,   12,   12, 0x08,
      39,   12,   12,   12, 0x08,
      50,   12,   12,   12, 0x08,
      65,   12,   12,   12, 0x08,
      78,   12,   12,   12, 0x08,
      89,   12,   12,   12, 0x08,
     100,   12,   12,   12, 0x08,
     112,   12,   12,   12, 0x08,
     125,   12,   12,   12, 0x08,
     137,   12,   12,   12, 0x08,
     149,   12,   12,   12, 0x08,
     162,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RobotDialog[] = {
    "RobotDialog\0\0dialogClosed()\0load_env()\0"
    "save_env()\0rotate_robot()\0speed_down()\0"
    "speed_up()\0add_wall()\0add_round()\0"
    "add_sround()\0add_light()\0add_tarea()\0"
    "delete_obj()\0conrobots()\0"
};

void RobotDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RobotDialog *_t = static_cast<RobotDialog *>(_o);
        switch (_id) {
        case 0: _t->dialogClosed(); break;
        case 1: _t->load_env(); break;
        case 2: _t->save_env(); break;
        case 3: _t->rotate_robot(); break;
        case 4: _t->speed_down(); break;
        case 5: _t->speed_up(); break;
        case 6: _t->add_wall(); break;
        case 7: _t->add_round(); break;
        case 8: _t->add_sround(); break;
        case 9: _t->add_light(); break;
        case 10: _t->add_tarea(); break;
        case 11: _t->delete_obj(); break;
        case 12: _t->conrobots(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData RobotDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RobotDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RobotDialog,
      qt_meta_data_RobotDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RobotDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RobotDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RobotDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RobotDialog))
        return static_cast<void*>(const_cast< RobotDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int RobotDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void RobotDialog::dialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
