/* 
 * Evorobot* - rend_robot.cpp
 * Copyright (C) 2009, Stefano Nolfi
 * LARAL, Institute of Cognitive Science and Technologies, CNR, Roma, Italy 
 * http://laral.istc.cnr.it

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include "public.h"
#include "epuck.h"
#include "environment.h"
#include "rend_epuck.h"

#ifdef EVOGRAPHICS  

#include <QtGui>
#include <QGridLayout>  // add_widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog


RendRobot *rendRobot;      // The network rendering widget
RobotDialog *robotDialog;  // The network Dialog

int widgetdx = 400;        // x-size of the widget
int widgetdy = 450;        // y-size of the widget
int wrobot=0;              // whether the window has been already opened or not
float scale;               // scale of the environment rendering
float trace[1000][2][10];  // trace data to be displayed for 10 robots and 1000 cycles                
int mousex=4000;           // mouse left-clicked x position
int mousey=4000;           // mouse left-clicked y position
struct envobject *obj;     // the last moved object
int objx;                  // original x position of the mouse during a move action
int objy;                  // original y position of the mouse during a move action
int selmode;               // selected object 1=robot, 2=object
char robotwstatus[256];    // stat message within rend_robot widget

//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

/*
 * mode=1, create the robot widget
 * mode=2  create the robot dialog and associate the widget to it
 */
void
init_rendrobot(int mode)
{   
    if (mode == 1 && wrobot == 0)
      {
       rendRobot = new RendRobot;
       wrobot = 1;
       sprintf(robotwstatus,"");
      }
    if (mode == 2 && wrobot == 1)
      {
       robotDialog = new RobotDialog(NULL);
       robotDialog->setWindowTitle("2D Robot/Environment");
       robotDialog->setWidget(rendRobot, 0, 0);
       if (winrobotenvx >= 0 && winrobotenvy >= 0)
        robotDialog->move(winrobotenvx, winrobotenvy);
       if (winrobotenvdx > 0 && winrobotenvdy > 0)
        robotDialog->resize(winrobotenvdx, winrobotenvdy);
       robotDialog->show();
       update_rendrobot();
       wrobot = 2;
      }
    if (mode == 2 && wrobot == 2)
      {
       robotDialog->show();
       update_rendrobot();
      }
}

void
hide_rendrobot()
{
    if (wrobot == 2)
    robotDialog->hide();
}

void
add_robot_widget(QGridLayout *mainLayout, int row, int column)
{
  mainLayout->addWidget(rendRobot, row, column);
}

void
update_rendrobot()

{

     rendRobot->repaint();
     QCoreApplication::processEvents();

}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  ------------------------------
//----------------------------------------------------------------------

// -----------------------------------------------------
// Dialog and Toolbar
// -----------------------------------------------------

void RobotDialog::createToolBars()
{
    QToolBar *robottoolbar;

    robottoolbar = toolBar();

    if (userlevel > 1)
    {
     robottoolbar->addAction(load_envAct);
     robottoolbar->addAction(save_envAct);
    }
    robottoolbar->addAction(speed_upAct);
    robottoolbar->addAction(speed_downAct);
    robottoolbar->addAction(rotate_robotAct);
    if (userlevel > 1)
    {
     robottoolbar->addAction(add_wallAct);
     robottoolbar->addAction(add_roundAct);
     robottoolbar->addAction(add_sroundAct);
     robottoolbar->addAction(add_lightAct);
     robottoolbar->addAction(add_tareaAct);
     robottoolbar->addAction(delete_objAct);
     robottoolbar->addAction(conrobotsAct);
    }
}

void RobotDialog::createActions()
{

    load_envAct = new QAction(QIcon(":/images/open.png"), tr("&Load environment from a file .env"), this);
    load_envAct->setShortcut(tr("Ctrl+L"));
    load_envAct->setStatusTip(tr("Load environment"));
    connect(load_envAct, SIGNAL(triggered()), this, SLOT(load_env()));

    save_envAct = new QAction(QIcon(":/images/save.png"), tr("&Save environments on files .env"), this);
    save_envAct->setShortcut(tr("Ctrl+S"));
    save_envAct->setStatusTip(tr("Save environments"));
    connect(save_envAct, SIGNAL(triggered()), this, SLOT(save_env()));

    rotate_robotAct = new QAction(QIcon(":/images/rotate.png"), tr("&Rotate Robot (R)"), this);
    rotate_robotAct->setShortcut(tr("R"));
    rotate_robotAct->setStatusTip(tr("Rotate the selected robot"));
    connect(rotate_robotAct, SIGNAL(triggered()), this, SLOT(rotate_robot()));

    speed_downAct = new QAction(QIcon(":/images/speeddown.png"), tr("&Speed down display (-)"), this);
    speed_downAct->setShortcut(tr("-"));
    speed_downAct->setStatusTip(tr("Speed down behaviour display (-)"));
    connect(speed_downAct, SIGNAL(triggered()), this, SLOT(speed_down()));

    speed_upAct = new QAction(QIcon(":/images/speedup.png"), tr("&Speed up display (+)"), this);
    speed_upAct->setShortcut(tr("+"));
    speed_upAct->setStatusTip(tr("Speed up behaviour display (+)"));
    connect(speed_upAct, SIGNAL(triggered()), this, SLOT(speed_up()));

    add_wallAct = new QAction(QIcon(":/images/wall.png"), tr("Add a wall object"), this);
    add_wallAct->setStatusTip(tr("Add a wall object"));
    connect(add_wallAct, SIGNAL(triggered()), this, SLOT(add_wall()));

    add_roundAct = new QAction(QIcon(":/images/round.png"), tr("Add a round object"), this);
    add_roundAct->setStatusTip(tr("Add a round object"));
    connect(add_roundAct, SIGNAL(triggered()), this, SLOT(add_round()));

    add_sroundAct = new QAction(QIcon(":/images/sround.png"), tr("Add a sround object"), this);
    add_sroundAct->setStatusTip(tr("Add a sround object"));
    connect(add_sroundAct, SIGNAL(triggered()), this, SLOT(add_sround()));

    add_lightAct = new QAction(QIcon(":/images/light.png"), tr("Add a light object"), this);
    add_lightAct->setStatusTip(tr("Add a light object"));
    connect(add_lightAct, SIGNAL(triggered()), this, SLOT(add_light()));

    add_tareaAct = new QAction(QIcon(":/images/t_area.png"), tr("Add a target area object"), this);
    add_tareaAct->setStatusTip(tr("Add a target area object"));
    connect(add_tareaAct, SIGNAL(triggered()), this, SLOT(add_tarea()));

    delete_objAct = new QAction(QIcon(":/images/erase.png"), tr("Delete the last selected object"), this);
    delete_objAct->setStatusTip(tr("Delete the last selected object"));
    connect(delete_objAct, SIGNAL(triggered()), this, SLOT(delete_obj()));

    conrobotsAct = new QAction(QIcon(":/images/epuckBN.png"), NULL, this);
    conrobotsAct->setStatusTip(tr("Enable real robots connection"));
    conrobotsAct->setIconText(tr("Enable real robots connection"));
    connect(conrobotsAct, SIGNAL(triggered()), this, SLOT(conrobots()));

}

RobotDialog::RobotDialog(QWidget *parent) :
    QDialog(parent),
    m_mainLayout(NULL),
    m_layout(NULL),
    m_toolBar(NULL)
{
    // Creating the main layout. Direction is BottomToTop so that m_layout (added first)
    // is at bottom, while the toolbar (added last) is on top
    m_mainLayout = new QBoxLayout(QBoxLayout::BottomToTop, this);
    //m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Creating the layout for external widgets
    m_layout = new QGridLayout();
    //m_layout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(m_layout, 1);

        createActions();
    createToolBars();
}

RobotDialog::~RobotDialog()
{
}

void RobotDialog::setWidget(QWidget *w, int r, int c)
{
    m_layout->addWidget(w, r, c);
}

QToolBar* RobotDialog::toolBar()
{
    if (m_toolBar == NULL) {
        // Creating toolbar and adding it to the main layout
        m_toolBar = new QToolBar(this);
        m_mainLayout->addWidget(m_toolBar, 0);
    }

    return m_toolBar;
}

void RobotDialog::closeEvent(QCloseEvent *)
{
    emit dialogClosed();
}

void RobotDialog::keyReleaseEvent(QKeyEvent* event)
{
    /*
    if (event->matches(QKeySequence::Print)) {
        // Taking a screenshow of the widget
        shotWidget();
    } else {
        // Calling parent function
        QDialog::keyReleaseEvent(event);
    }*/
}

void RobotDialog::shotWidget()
{
    /*
    // Taking a screenshot of this widget
    QPixmap shot(size());
    render(&shot);

    // Asking the user where to save the shot
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Shot"), "./widget.png", tr("Images (*.png *.xpm *.jpg)"));
    if (!fileName.isEmpty()) {
        shot.save(fileName);

        QMessageBox::information(this, QString("File Saved"), QString("The widget shot has been saved"));
    }*/
}

/*
 * open a .env file
 */
void RobotDialog::load_env()
{

    char *f;
    QByteArray filen;
    char filename[256];


    QString fileName = QFileDialog::getOpenFileName(this,
            "Choose a filename to open",
                    "",
                    "*.env");


        if (fileName.endsWith("env"))
    {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      load_world(filename);
      update_rendrobot();
    }


}

/*
 * save a .env file
 */
void RobotDialog::save_env()
{

    char *f;
    QByteArray filen;
    char filename[256];


    QString fileName = QFileDialog::getSaveFileName(this,
            "Choose a filename to save",
                    "",
                    "*.env");


    if (fileName.endsWith("env"))
    {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      save_world(filename);
    }

}


// rotate the direction of the current robot
void RobotDialog::rotate_robot()
{
   struct individual *pind;

   pind = (ind + cindividual);
   pind->direction += 2.0;
   if (pind->direction > 360)
     pind->direction -= 360;
   sprintf(robotwstatus,"rotate robot %d", cindividual);
   update_rendrobot();
}

void RobotDialog::speed_down()

{


    if (timeadjust > 100)
      timeadjust += 20;
     else
      if (timeadjust > 50)
       timeadjust += 20;
      else
       if (timeadjust > 10)
         timeadjust += 5;
        else
         timeadjust++;
    sprintf(robotwstatus,"pause set to %d", timeadjust);
    update_rendrobot();

}

void RobotDialog::speed_up()
{


    if (timeadjust > 100)
      timeadjust -= 20;
     else
      if (timeadjust > 50)
       timeadjust -= 20;
      else
       if (timeadjust > 10)
         timeadjust -= 5;
        else
         timeadjust--;
    if (timeadjust < 0)
     timeadjust = 0;
    sprintf(robotwstatus,"pause set to %d", timeadjust);
    update_rendrobot();

}


void RobotDialog::add_wall()
{

   struct envobject *obj;

   obj = *envobjs;
   obj = (obj + envnobjs[0]);
   obj->id = envnobjs[0];
   envnobjs[0] += 1;

   obj->type = 0;
   obj->x = (envdx / 2) - 50;
   obj->y = envdy + 50;
   obj->X = (envdx / 2) + 50;
   obj->Y = envdy + 50;
   obj->c[0] = 0.0;
   obj->c[1] = 0.0;
   obj->c[2] = 0.0;

   sprintf(robotwstatus,"added swall object #%d", envnobjs[0]);
   update_rendrobot();
}



void RobotDialog::add_sround()
{

   struct envobject *obj;

   obj = *(envobjs + 1);
   obj = (obj + envnobjs[1]);
   obj->id = envnobjs[1];
   envnobjs[1] += 1;

   obj->type = 1;
   obj->x = envdx / 2;
   obj->y = envdy + 50;
   obj->r = 12.5;
   obj->c[0] = 0.0;
   obj->c[1] = 0.0;
   obj->c[2] = 0.0;
   sprintf(robotwstatus,"added sround object #%d", envnobjs[1]);
   update_rendrobot();
}

void RobotDialog::add_round()
{

   struct envobject *obj;


   obj = *(envobjs + 2);
   obj = (obj + envnobjs[2]);
   obj->id = envnobjs[2];
   envnobjs[2] += 1;

   obj->type = 2;
   obj->x = envdx / 2;
   obj->y = envdy + 50;
   obj->r = 27.0;
   obj->c[0] = 0.0;
   obj->c[1] = 0.0;
   obj->c[2] = 0.0;

   sprintf(robotwstatus,"added round object #%d", envnobjs[2]);
   update_rendrobot();
}


void RobotDialog::add_tarea()
{

   struct envobject *obj;
   struct envobject *objb;

   obj = *(envobjs + 3);
   obj = (obj + envnobjs[3]);
   obj->id = envnobjs[3];
   envnobjs[3] += 1;

   obj->type = 3;
   obj->subtype = 0;
   obj->x = envdx / 2;
   obj->y = envdy + 50;
   obj->r = 80;
   obj->c[0] = 0.75;
   obj->c[1] = 0.0;
   obj->c[2] = 0.0;
   if (envnobjs[3] > 1)
    {
     objb = *(envobjs + 3);
     objb = (objb + (envnobjs[3] - 2));
     obj->r = objb->r;
     obj->c[0] = objb->c[0];
    }

   sprintf(robotwstatus,"added t_area object #%d", envnobjs[3]);
   update_rendrobot();
}

void RobotDialog::add_light()
{

   struct envobject *obj;

   obj = *(envobjs + 4);
   obj = (obj + envnobjs[4]);
   obj->id = envnobjs[4];
   envnobjs[4] += 1;

   obj->type = 4;
   obj->x = envdx / 2;
   obj->y = envdy + 50;
   obj->r = 10.0;
   obj->c[0] = 0.0;
   obj->c[1] = 0.0;
   obj->c[2] = 0.0;

   sprintf(robotwstatus,"added light object #%d", envnobjs[4]);
   update_rendrobot();
}

void RobotDialog::delete_obj()
{

   struct envobject *obji;
   struct envobject *objii;
   int i;

   if (obj != NULL)
     {
       if (obj->type == 0)
         sprintf(robotwstatus,"deleted swall %d", obj->id);
       if (obj->type == 1)
         sprintf(robotwstatus,"deleted sround %d", obj->id);
       if (obj->type == 2)
         sprintf(robotwstatus,"deleted round %d", obj->id);
       if (obj->type == 3)
         sprintf(robotwstatus,"deleted ground %d", obj->id);
       if (obj->type == 4)
         sprintf(robotwstatus,"deleted light %d", obj->id);

       obji = *(envobjs + obj->type);
       obji = (obji + obj->id);

       for(i=obj->id; i < (envnobjs[obj->type] - 1); i++, obji++)
        {
         objii = (obji + 1);
         obji->type    = objii->type;    
         obji->subtype = objii->subtype;
         obji->id      = objii->id;
         obji->x       = objii->x;
         obji->y       = objii->y;
         obji->X       = objii->X;
         obji->Y       = objii->Y;
         obji->r       = objii->r;
         obji->h       = objii->h;
         obji->c[0]    = objii->c[0];
         obji->c[1]    = objii->c[1];
         obji->c[2]    = objii->c[2];
        }

       envnobjs[obj->type] -= 1;
       update_rendrobot();
      }
      
}

void RobotDialog::conrobots()
{

    if (real==0)
    {
         connect_disconnect_robots();
     conrobotsAct->setIcon(QIcon(":/images/epuck.png"));
     conrobotsAct->setIconText(tr("Disconnect all real robots"));
    }
    else
    {
     connect_disconnect_robots();
     conrobotsAct->setIcon(QIcon(":/images/epuckBN.png"));
     conrobotsAct->setIconText(tr("Enable real robots connection"));
    }
}


// -----------------------------------------------------
// Widget
// -----------------------------------------------------

RendRobot::RendRobot(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);

}


QSize RendRobot::minimumSizeHint() const
{
    return QSize(200, 250);
}

QSize RendRobot::sizeHint() const
{

      return QSize(widgetdx, widgetdy);

}

void RendRobot::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendRobot::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RendRobot::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendRobot::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendRobot::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void RendRobot::paintEvent(QPaintEvent *)
{
 

   int    team;
   int    i,ii;
   int    u;
   int    c;
   int    t,b;
   int    counter;
   int    n_start, n_max;
   int    h1,h2;
   int    sx,sy,dx,dy;
   float  headx, heady;
   struct individual *pind;
   int    g,n;
   int    offx,offy;
   int    maxx;
   float  x_scale, y_scale;
   double statfit_int;
   char   label[128];
   char   filen[128];
   char   *chelp;
   char   message[128];
   int    ang1, ang2;
   int    col;
   int    cx, cy;

   int    **ob;
   int    *o;
   int    *ocf;
   int    a,d,s;
   int    a1;
   int    r;
   float  ifx, ify;
   float  ifx2, ify2;
   double ifa;
   int    stx, sty;
   int    heig;
   int    scolor;
   float  csize;
   struct envobject *obj;

   double gbackdir;
   double gleftdir;
   double grightdir;     
   float  gheadx,gheady;
   float  gbackx,gbacky;     
   float  gleftx,glefty;          
   float  grightx,grighty;
   float  gflx,gfly;
   float  gfrx,gfry;                  
   float  gfdeltax,gfdeltay;
   float  gbdeltax,gbdeltay;     
   float  gblx,gbly; 
   float  gbrx,gbry;
   float  gopen;
   float  lgripper[4];
   float  rgripper[4]; 
   int    nifs; 
   
   QRect labelxy(0,0,30,20);          // neuron labels 
   QPoint pxy;

   QPainter painter(this);
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);
  
   // draw the world and the robots
   // robots are shown during test_individual
   // or when the mouse is clicked on the main window
   if (evolution_is_running == 0)
   {
    widgetdx = width();
    widgetdy = height(); 
    set_scale();
    // draw the world
    for (i=0, obj = *(envobjs + IMAGE); i < envnobjs[IMAGE]; i++, obj++)
      {
        QImage newi = obj->qi->scaled(obj->iw * scale, obj->ih * scale,Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //painter.drawImage(0,0,*(obj->qi));
        painter.drawImage(0,0,newi);
      }

    obj = *(envobjs + GROUND);
    for (i= (envnobjs[GROUND] - 1), obj =(obj + (envnobjs[GROUND] - 1)); i >= 0; i--, obj--)
       {
        scolor = (1.0 - obj->c[0]) * 155.0 + 100;
    if (scolor > 255)
      scolor = 255;
    if (scolor < 0)
      scolor = 0;
        painter.setPen(QColor(0,scolor,scolor,255));
        painter.setBrush(QBrush(QColor(0,scolor,scolor,255), Qt::SolidPattern));
        if (obj->subtype == 0)
          painter.drawEllipse((obj->x - obj->r) * scale, (obj->y - obj->r) * scale, obj->r * 2.0  * scale, obj->r * 2.0  * scale);
    else
      painter.drawRect(obj->x * scale, obj->y * scale, obj->X * scale, obj->Y * scale); 
       }
    for (i=0, obj = *(envobjs + SWALL); i < envnobjs[SWALL]; i++, obj++)
       {
    painter.setPen(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255));
        painter.drawLine(obj->x * scale, obj->y  * scale,obj->X  * scale, obj->Y  * scale);
       }
    for (i=0, obj = *(envobjs + SROUND); i < envnobjs[SROUND]; i++, obj++)
      {
    painter.setPen(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255));
        painter.setBrush(QBrush(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255), Qt::SolidPattern));
        painter.drawEllipse((obj->x - obj->r) * scale,(obj->y - obj->r) * scale, obj->r * 2.0  * scale, obj->r * 2.0  * scale);
      }
    for (i=0, obj = *(envobjs + ROUND); i < envnobjs[ROUND]; i++, obj++)
      {
    painter.setPen(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255));
        painter.setBrush(QBrush(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255), Qt::SolidPattern));
        painter.drawEllipse((obj->x - obj->r) * scale,(obj->y - obj->r) * scale, obj->r * 2.0  * scale, obj->r * 2.0  * scale);
      }
    for (i=0, obj = *(envobjs + LIGHT); i < envnobjs[LIGHT]; i++, obj++)
      {
        painter.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
        painter.setPen(Qt::yellow);
        painter.drawEllipse((obj->x - 10.0)  * scale, (obj->y - 10.0)  * scale, 20.0  * scale, 20.0  * scale);
      }

    painter.setBrush(QBrush(Qt::black, Qt::NoBrush));
    painter.setPen(Qt::red);
    pxy.setX(0);
    pxy.setY(envdy * scale + 20);
    painter.drawText(pxy, robotwstatus);
    painter.setPen(Qt::black);
    sprintf(message, "env: %dx%d", envdx, envdy);
    pxy.setX(0);
    if ((envdx * scale - 100) > 0)
      pxy.setX(envdx * scale - 100);
    else
      pxy.setX(0);
    pxy.setY(envdy * scale + 20);
    painter.drawText(pxy, message);
    pxy.setX(0);
    pxy.setY(envdy * scale + 40);
    painter.drawText(pxy, ffitness_descp);

    // draw the robots
    for(team=0,pind=ind; team < nteam; team++, pind++)
    {
          // display camera
      if (displaysensors == 1 && (ipar->linearcamera == 1 || ipar->legocam == 1))
          for (r=(pind->camera_nr-1), ang1 = (360 - pind->direction) - pind->camera_ang; r >= 0; r--, ang1 += pind->camera_dang)
          {
           if (ang1 < 360)
             ang1 += 360;
           if (ang1 < 360)
             ang1 += 360;
           if (ang1 >= 360)
             ang1 -= 360;

           painter.setPen(QColor((int) (pind->camera[0][r] * 255.0),(int) (pind->camera[1][r] * 255.0),(int) (pind->camera[2][r] * 255.0),255));
           painter.setBrush(QBrush(QColor((int)(pind->camera[0][r] * 255),(int)(pind->camera[1][r] * 255),(int)(pind->camera[2][r] * 255),255), Qt::SolidPattern));
           painter.drawPie((pind->pos[0] - (robotradius * 0.9))  * scale, (pind->pos[1] - (robotradius*0.9))  * scale,  2*(robotradius*0.9)*scale, 2*(robotradius*0.9)*scale, ang1 * 16, pind->camera_dang * 16);
          //drawing heading
		   painter.setPen(Qt::blue);
		   painter.drawLine(pind->pos[0]*scale,pind->pos[1]*scale,SetX(pind->pos[0], 300, pind->direction)*scale,SetY(pind->pos[1], 300, pind->direction)*scale );
		  
		  }
          // display infrared
      nifs = ipar->nifsensors;
      if (ipar->ngifsensors > nifs)
        nifs = ipar->ngifsensors;
      if (displaysensors == 1)
      for (i=0; i < nifs; i++)
      {
            ifa = pind->direction + pind->cifang[i];
            if (ifa > 360) ifa -= 360;
        if (ifa < 0) ifa += 360;
        ifx = SetX(pind->pos[0], robotradius * 2, ifa);
        ify = SetY(pind->pos[1], robotradius * 2, ifa);
        painter.setPen(QColor(120,120,120,255));
        painter.drawLine(pind->pos[0] * scale,pind->pos[1] * scale, ifx * scale, ify * scale);
        ifa += 180;
            if (ifa > 360) ifa -= 360;
        ifx2 = SetX(ifx, pind->cifsensors[i] * robotradius * 2, ifa);
        ify2 = SetY(ify, pind->cifsensors[i] * robotradius * 2, ifa);
        painter.setPen(QColor(255,0,0,255));
        painter.drawLine(ifx * scale,ify * scale, ifx2 * scale, ify2 * scale);
      }

          // draw body
	  if(robottype<5)
	  {
	   if (ipar->laserScan==1)
	   {
		   //drawing heading
		   painter.setPen(Qt::blue);
		   painter.drawLine(pind->pos[0]*scale,pind->pos[1]*scale,SetX(pind->pos[0], ipar->laserLength, pind->direction+pind->laserPos*2*ipar->laserScanRange-ipar->laserScanRange)*scale,SetY(pind->pos[1], ipar->laserLength, pind->direction+pind->laserPos*2*ipar->laserScanRange-ipar->laserScanRange)*scale );
		
	   }
      if (team < envnobjs[5])
          {
         obj = *(envobjs + 5);
         obj = (obj + team);
             painter.setPen(QColor((int) (obj->c[0] * 255.0),(int) (obj->c[1] * 255.0),(int) (obj->c[2] * 255.0),255));
          }
      else
          {
             painter.setPen(QColor(120,120,120,255));
          }
          painter.setBrush(QBrush(Qt::black, Qt::NoBrush));
          painter.drawEllipse((pind->pos[0] - robotradius)  * scale, (pind->pos[1] - robotradius)  * scale, 2*robotradius  * scale, 2*robotradius  * scale);
          painter.setPen(QColor(120,120,120,255));
      if (team == cindividual)
            painter.drawEllipse((pind->pos[0] - 15.0)  * scale, (pind->pos[1] - 15.0)  * scale, 30  * scale, 30  * scale);
      headx = SetX(pind->pos[0],(float) robotradius, pind->direction);
      heady = SetY(pind->pos[1],(float) robotradius, pind->direction);
      painter.drawLine(pind->pos[0] * scale,pind->pos[1] * scale, headx * scale, heady * scale);
	  }
	  if(robottype==RETINABOT)
	  {
		 
		  //here we draw the retina
		  
		  //painter.drawRect(
		 // painter.drawRect((int)(ipar->retinaX-ipar->retinaSize/2.0)*scale,(int)(ipar->retinaY-ipar->retinaSize/2.0)*scale,(int)(ipar->retinaX+ipar->retinaSize/2.0)*scale,(int)(ipar->retinaY+ipar->retinaSize/2.0)*scale);
		  //int dx=(int)(pind->wheel_motor[0]*180.0-90.0);
		  //int dy=(int)(pind->wheel_motor[1]*180.0-90.0);
		  

		 // int dx=(int)(rans(90));
		  //int dy=(int)(rans(90));
		  //int dx=0;
		  //int dy=0;
		  //ipar->cRetinaX=ipar->retinaX+dx;
		  //ipar->cRetinaY=ipar->retinaY+dy;
		  //drawing retina receptors

		  //drawing retina activation
		 
		  
		  for(int c=0;c<ipar->retina;c++)
			  for(int r=0;r<ipar->retina;r++)
			  {
				//  painter.setPen(QColor(255,50,50));
				  int col=(int)(pind->retinaMatrix[c][r]*255.0);
				  painter.fillRect(c*15,r*15,13,13,QColor(col,0,0));
			  }
		  int cf=(ipar->retina-1)/2;
		  int col=(int)(pind->retinaColorReceptor*255.0);
		  painter.fillRect(cf*15,cf*15,13,13,QColor(col,col,0));

			  //drawing the rest
		   //drawing center
		 /* painter.drawLine((ipar->retinaX-10)*scale,ipar->retinaY*scale,(ipar->retinaX+10)*scale,ipar->retinaY*scale);
		  painter.drawLine((ipar->retinaX)*scale,(ipar->retinaY-10)*scale,(ipar->retinaX)*scale,(ipar->retinaY+10)*scale);
		  float dr=(float)ipar->cRetinaSize/(float)ipar->retina;
		  painter.setPen(Qt::green);
		  for(int c=0;c<ipar->retina;c++)
			  for(int r=0;r<ipar->retina;r++)
			  {
				  if(c<(ipar->retina-1)/2) painter.setPen(QColor(243,105,31)); //arancione
				  if(c>(ipar->retina-1)/2) painter.setPen(QColor(60,185,82));
				  if(c==(ipar->retina-1)/2) painter.setPen(QColor(84,118,60));//centro

				  float px=ipar->cRetinaX-ipar->cRetinaSize/2.0+c*dr+dr/2.0;
				  float py=ipar->cRetinaY-ipar->cRetinaSize/2.0+r*dr+dr/2.0;
				  painter.drawEllipse((int)px*scale,(int)py*scale,1,1);
			  }
		
		  painter.setPen(Qt::blue);
		  painter.drawRect((int)(ipar->cRetinaX-ipar->cRetinaSize/2.0)*scale,(int)(ipar->cRetinaY-ipar->cRetinaSize/2.0)*scale,(int)(ipar->cRetinaSize)*scale,(int)(ipar->cRetinaSize)*scale);
		  painter.setBrush(Qt::SolidPattern);
		  painter.drawEllipse(((int)ipar->cRetinaX)*scale-2,((int)ipar->cRetinaY)*scale-2,4,4);
		  //painter.fi*/
		  painter.drawLine((ipar->retinaX-10)*scale,ipar->retinaY*scale,(ipar->retinaX+10)*scale,ipar->retinaY*scale);
		  painter.drawLine((ipar->retinaX)*scale,(ipar->retinaY-10)*scale,(ipar->retinaX)*scale,(ipar->retinaY+10)*scale);
		  float dr=(float)ipar->cRetinaSizeold/(float)ipar->retina;
		  painter.setPen(Qt::green);
		  for(int c=0;c<ipar->retina;c++)
			  for(int r=0;r<ipar->retina;r++)
			  {
				  if(c<(ipar->retina-1)/2) painter.setPen(QColor(243,105,31)); //arancione
				  if(c>(ipar->retina-1)/2) painter.setPen(QColor(60,185,82));
				  if(c==(ipar->retina-1)/2) painter.setPen(QColor(84,118,60));//centro

				  float px=ipar->cRetinaXold-ipar->cRetinaSizeold/2.0+c*dr+dr/2.0;
				  float py=ipar->cRetinaYold-ipar->cRetinaSizeold/2.0+r*dr+dr/2.0;
				  painter.drawEllipse((int)px*scale,(int)py*scale,1,1);
			  }
		
		  painter.setPen(Qt::blue);
		  painter.drawRect((int)(ipar->cRetinaXold-ipar->cRetinaSizeold/2.0)*scale,(int)(ipar->cRetinaYold-ipar->cRetinaSizeold/2.0)*scale,(int)(ipar->cRetinaSizeold)*scale,(int)(ipar->cRetinaSizeold)*scale);
		  painter.setBrush(Qt::SolidPattern);
		  painter.drawEllipse(((int)ipar->cRetinaXold)*scale-2,((int)ipar->cRetinaYold)*scale-2,4,4);
		  		
	  }
          // draw gripper
      if (ipar->gripper == 1)
      {
            gbackdir = pind->direction + 180.0;
            if (gbackdir > 359.0) gbackdir -= 360.0;
            gleftdir = pind->direction + 90.0;
            if (gleftdir > 359.0) gleftdir -= 360.0;
            grightdir = pind->direction - 90.0;
            if (grightdir < 0.0) grightdir += 360.0; 
    
            gheadx  = SetX(pind->pos[0],42.5f,pind->direction);
            gheady  = SetY(pind->pos[1],42.5f,pind->direction);
            gbackx  = SetX(pind->pos[0],27.5f,gbackdir);
            gbacky  = SetY(pind->pos[1],27.5f,gbackdir);     
            grightx = SetX(pind->pos[0],35.0f,grightdir);
            grighty = SetY(pind->pos[1],35.0f,grightdir);
            gleftx  = SetX(pind->pos[0],35.0f,gleftdir);
            glefty  = SetY(pind->pos[1],35.0f,gleftdir);          
            headx  = SetX(pind->pos[0],60.0f,pind->direction);
            heady  = SetY(pind->pos[1],60.0f,pind->direction);

            gfdeltax = (gheadx - pind->pos[0]);
            gfdeltay = (gheady - pind->pos[1]);          
            gbdeltax = (pind->pos[0] - gbackx);
            gbdeltay = (pind->pos[1] - gbacky);               
            gflx  =  gleftx + gfdeltax;   
            gfly  =  glefty + gfdeltay;        
            gfrx  =  grightx + gfdeltax;   
            gfry  =  grighty + gfdeltay;                  
            gblx  =  gleftx - gbdeltax;   
            gbly  =  glefty - gbdeltay;        
            gbrx  =  grightx - gbdeltax;   
            gbry  =  grighty - gbdeltay;                  
        
        painter.drawLine(gflx * scale,gfly * scale, gfrx * scale, gfry * scale);
        painter.drawLine(gblx * scale,gbly * scale, gbrx * scale, gbry * scale);
        painter.drawLine(gflx * scale,gfly * scale, gblx * scale, gbly * scale);
        painter.drawLine(gfrx * scale,gfry * scale, gbrx * scale, gbry * scale);

            if (pind->grip_obtaken == 1)
         {
          gopen = 14.0;
          if (envnobjs[SROUND] > 0)
           {
           obj = *(envobjs + SROUND);
               painter.setBrush(QBrush(QColor((int)(obj->c[0] * 255),(int)(obj->c[1] * 255),(int)(obj->c[2] * 255),255), Qt::SolidPattern));
           painter.drawEllipse((headx - obj->r) * scale,(heady - obj->r) * scale, (obj->r * 2) * scale, (obj->r * 2) * scale);
               }
             }
        else
         {
          gopen = 25.0;
             }

            lgripper[0] = SetX(gheadx,gopen,gleftdir);
            lgripper[1] = SetY(gheady,gopen,gleftdir);
            lgripper[2] = SetX(lgripper[0],35.0,pind->direction);
            lgripper[3] = SetY(lgripper[1],35.0,pind->direction);

            rgripper[0] = SetX(gheadx,gopen,grightdir);
            rgripper[1] = SetY(gheady,gopen,grightdir);
            rgripper[2] = SetX(rgripper[0],35.0f,pind->direction);
            rgripper[3] = SetY(rgripper[1],35.0f,pind->direction);
        
        painter.drawLine(lgripper[0] * scale,lgripper[1] * scale, lgripper[2] * scale, lgripper[3] * scale);
        painter.drawLine(rgripper[0] * scale,rgripper[1] * scale, rgripper[2] * scale, rgripper[3] * scale);
      }

          // draw trace
          if (drawtrace > 0 && testindividual_is_running == 1)
      {
        if (team == 0)
              painter.setPen(QColor(255,0,0,255));
        if (team == 1)
              painter.setPen(QColor(0,0,255,255));
        if (team == 2)
              painter.setPen(QColor(0,255,0,255));
        if (team == 3)
              painter.setPen(QColor(160,0,160,255));

        trace[run%drawtrace][0][team] = pind->pos[0];
        trace[run%drawtrace][1][team] = pind->pos[1];
        for(c=0; c < ((run%drawtrace) - 1); c++)
              painter.drawLine(trace[c][0][team] * scale, trace[c][1][team] * scale,trace[c+1][0][team] * scale, trace[c+1][1][team] * scale); 
        if (run > drawtrace)
         for(c=(run%drawtrace)+1; c < (drawtrace - 1); c++)
              painter.drawLine(trace[c][0][team] * scale, trace[c][1][team] * scale,trace[c+1][0][team] * scale, trace[c+1][1][team] * scale);         
      }
    }

   }

}



/*
 * handle mouse buttons
 */
void 
RendRobot::mousePressEvent(QMouseEvent *event)
{
        
    int x,y,b;
    int olddrawi;
    int i;
    struct individual *pind;

        sprintf(robotwstatus,"");
    x=event->x();
    y=event->y();
    b=event->button();
        obj = NULL;

    if (b == 2)
    {
      // right button stop evolution or testindividual
    if (testindividual_is_running == 1 || evolution_is_running == 1)
       userbreak = 1;
    }
        else
    {
     // left button
     if (b == 1 && evolution_is_running == 0)
     {
      selmode = 0;
          set_scale();
      mousex = x;
      mousey = y;
      //   on a robot: set the robot as the current individual
      for(i=0, pind = ind; i < nteam; i++, pind++)
      {
        if (mdist((float) x / scale,(float) y / scale, pind->pos[0], pind->pos[1]) < robotradius)
        {
          cindividual = i;
          sprintf(robotwstatus,"robot #%d selected", i);
          selmode = 1;
          objx = pind->pos[0];
          objy = pind->pos[1];
              rendRobot->repaint();
          repaint();
          QCoreApplication::processEvents();
          return;
        }
      }
      // sround
      for(i=0, obj = *(envobjs + 1); i < envnobjs[1]; i++, obj++)
          {
            if (mdist((float) mousex / scale,(float) mousey / scale, obj->x, obj->y) < obj->r)
        {
          sprintf(robotwstatus,"sround #%d selected", obj->id);
          objx = obj->x;
          objy = obj->y;
              selmode = 2;
              repaint();
              QCoreApplication::processEvents();
          return;
        }
          }
      // round
      for(i=0, obj = *(envobjs + 2); i < envnobjs[2]; i++, obj++)
          {
            if (mdist((float) mousex / scale,(float) mousey / scale, obj->x, obj->y) < obj->r)        
        {
          objx = obj->x;
          objy = obj->y;
              selmode = 2;
          sprintf(robotwstatus,"round #%d selected", obj->id);
              repaint();
              QCoreApplication::processEvents();
          return;
        }
          }
      // light
      for(i=0, obj = *(envobjs + 4); i < envnobjs[4]; i++, obj++)
          {
            if (mdist((float) mousex / scale,(float) mousey / scale, obj->x, obj->y) < obj->r)        
        {
          objx = obj->x;
          objy = obj->y;
              selmode = 2;
          sprintf(robotwstatus,"light #%d selected", obj->id);
              repaint();
              QCoreApplication::processEvents();
          return;
        }
          }
      // swall 
       for(i=0, obj = *(envobjs + 0); i < envnobjs[0]; i++, obj++)
          {
            // line beginning
            if (mdist((float) mousex / scale,(float) mousey / scale, obj->x, obj->y) < 3.0 / scale)
        {
          objx = obj->x;
          objy = obj->y;
              selmode = 2;
          sprintf(robotwstatus,"swall #%d (beginning) selected", obj->id);
              repaint();
              QCoreApplication::processEvents();
          return;
        }
            // line ending
            if (mdist((float) mousex / scale,(float) mousey / scale, obj->X, obj->Y) < 3 / scale)
        {
          objx = obj->X;
          objy = obj->Y;
              selmode = 3;
          sprintf(robotwstatus,"swall #%d (ending) selected", obj->id);
              repaint();
              QCoreApplication::processEvents();
          return;
        } 
          } 
      // ground
      for(i=0, obj = *(envobjs + 3); i < envnobjs[3]; i++, obj++)
          {
            if ((obj->subtype == 0 && (mdist((float) mousex / scale,(float) mousey / scale, obj->x, obj->y) < obj->r)) ||        
                (obj->subtype == 1 && (((float) mousex / scale) >= obj->x) && (((float) mousex / scale) <= (obj->x + obj->X)) && (((float) mousey / scale) >= obj->y) && (((float) mousey / scale) <= (obj->y + obj->Y))))
        {
          objx = obj->x;
          objy = obj->y;
              selmode = 2;
          sprintf(robotwstatus,"ground #%d selected", obj->id);
              repaint();
              QCoreApplication::processEvents();
          return;
        }
          }

     }

    }
      repaint();


}


/*
 * move objects (available from dedicated window only)
 */
void 
RendRobot::mouseMoveEvent(QMouseEvent *event)
{

      int x,y;
      struct individual *pind;
      QPoint xy;

      if (evolution_is_running == 0)
       {
    x=event->x();
    y=event->y();
    pind = (ind + cindividual);

    // move current robot
    if (selmode == 1)
          {
        manual_start = 1;
        pind->pos[0] = objx + (((float)(x - mousex)) / scale);
        pind->pos[1] = objy + (((float)(y - mousey)) / scale);
        sprintf(robotwstatus,"robot #%d moved to %.1f %.1f", cindividual, pind->pos[0], pind->pos[1]);
            repaint();
        return;
          }
      // move selected object
      if (obj != NULL && wrobot == 2 && selmode > 1)
        {
         if (selmode == 2)
         {
          obj->x = objx + (((float) (x - mousex)) / scale);
          obj->y = objy + (((float) (y - mousey)) / scale);
          if (grid > 0)
           {
            obj->x = ((int) obj->x / grid) * grid;
                obj->y = ((int) obj->y / grid) * grid;
           }
          }
         else
         {
          obj->X = objx + (((float) (x - mousex)) / scale);
          obj->Y = objy + (((float) (y - mousey)) / scale);
          if (grid > 0)
           {
            obj->X = ((int) obj->X / grid) * grid;
                obj->Y = ((int) obj->Y / grid) * grid;
           }
          }
          sprintf(robotwstatus,"object #%d moved to %.1f %.1f", obj->id, obj->x, obj->y);
          repaint();
          QCoreApplication::processEvents();
          return;
        }
         

    }

}



/*
 * handle mouse release events by constraints movements to the grid (if any)
 */
void 
RendRobot::mouseReleaseEvent(QMouseEvent *event)
{



}

/*
 * handle mouse buttons
 */
void 
RendRobot::mouseDoubleClickEvent(QMouseEvent *event)
{

}




/*
 * set the scale for the world display
 */
void
set_scale()

{
    float scaleb;

    scale = (float) widgetdx  / (float) (envdx + 10);
    scaleb = (float) (widgetdy - 50)  / (float) (envdy + 10);
    if (scaleb < scale)
     scale = scaleb;
   
}

/*
 * Load an image from a file and store in within an environmental object structure
 */
void
load_image(struct envobject *obj, char *filename)

{

    int x,y;
    
    obj->qi = new QImage(filename);
    obj->iw = obj->qi->width();
    obj->ih = obj->qi->height();
    if (obj->subtype == 0)
      obj->i = (float *) malloc((obj->iw * obj->ih)* sizeof(float));
    else
      obj->i = (float *) malloc((obj->iw * obj->ih * 3) * sizeof(float));

    if (obj->i == NULL)
      display_error("system error: obj->i malloc");

    for(y=0;y<obj->ih;y++)
     for(x=0;x<obj->iw;x++)
      {
       if (obj->subtype == 0)
       {
         obj->i[x+(y*obj->iw)] = (float) qGray(obj->qi->pixel(x,y)) / 255.0f; 
       }
       else
       {
         obj->i[(x+(y*obj->iw))*3]   = (float) qRed(obj->qi->pixel(x,y)) / 255.0f; 
         obj->i[(x+(y*obj->iw))*3+1] = (float) qGreen(obj->qi->pixel(x,y)) / 255.0f; 
         obj->i[(x+(y*obj->iw))*3+2] = (float) qBlue(obj->qi->pixel(x,y)) / 255.0f; 
       }
      }

}

#endif

