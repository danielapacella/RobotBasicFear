/* 
 * Evorobot* - A software for running evolutionary robotics experiments
 * Copyright (C) 2005 
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

#include "mode.h"
#include "defs.h"
#include "public.h"
#include "rend_robot.h"

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
      }
    if (mode == 2 && wrobot == 1)
      {
       robotDialog = new RobotDialog(NULL);
       robotDialog->setWindowTitle("Robot/Environment");
       robotDialog->setWidget(rendRobot, 0, 0);
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
     int olddrawi;
     olddrawi = drawindividual; 
     drawindividual = 1;

     rendRobot->repaint();
     QCoreApplication::processEvents();
     drawindividual = olddrawi;

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

    robottoolbar->addAction(speed_upAct);
    robottoolbar->addAction(speed_downAct);
    robottoolbar->addAction(rotate_robotAct);
    robottoolbar->addAction(add_wallAct);
    robottoolbar->addAction(add_roundAct);
    robottoolbar->addAction(add_sroundAct);
    robottoolbar->addAction(add_lightAct);
    robottoolbar->addAction(add_tareaAct);
    robottoolbar->addAction(conrobotsAct);

}

void RobotDialog::createActions()
{

    rotate_robotAct = new QAction(QIcon(":/images/rotate.png"), tr("&Rotate Robot CTRL-R"), this);
    rotate_robotAct->setShortcut(tr("Ctrl+R"));
    rotate_robotAct->setStatusTip(tr("Rotate the selected robot"));
    connect(rotate_robotAct, SIGNAL(triggered()), this, SLOT(rotate_robot()));

    speed_downAct = new QAction(QIcon(":/images/speeddown.png"), tr("&Speed down display (CTRL-D)"), this);
    speed_downAct->setShortcut(tr("Ctrl+D"));
    speed_downAct->setStatusTip(tr("Speed down behaviour display (CTRL-D)"));
    connect(speed_downAct, SIGNAL(triggered()), this, SLOT(speed_down()));

    speed_upAct = new QAction(QIcon(":/images/speedup.png"), tr("&Speed up display (CTRL-U)"), this);
    speed_upAct->setShortcut(tr("Ctrl+U"));
    speed_upAct->setStatusTip(tr("Speed up behaviour display (CTRL-U)"));
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
	m_mainLayout->addLayout(m_layout);

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
		m_mainLayout->addWidget(m_toolBar);
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


// rotate the direction of the current robot
void RobotDialog::rotate_robot()
{
   struct individual *pind;
   int olddrawi;

   pind = (ind + cindividual);
   pind->direction += 2.0;
   olddrawi = drawindividual;

   drawindividual = 1;
   update_rendrobot();
   drawindividual = olddrawi;
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
    sprintf(drawdata_st,"pause set to %d", timeadjust);
    display_stat_message(drawdata_st);
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
    sprintf(drawdata_st,"pause set to %d", timeadjust);
    display_stat_message(drawdata_st);
}


void RobotDialog::add_wall()
{

   int i;

   i = env[cworld].nobstacles;
   env[cworld].nobstacles++;

   env[cworld].obstacles[i][0] = (env[cworld].dx / 2) - 50;
   env[cworld].obstacles[i][1] = env[cworld].dy + 50;
   env[cworld].obstacles[i][2] = (env[cworld].dx / 2) + 50;
   env[cworld].obstacles[i][3] = env[cworld].dy + 50;

   sprintf(drawdata_st,"added wall object #%d", i);
   display_stat_message(drawdata_st);
   update_rendrobot();
}

void RobotDialog::add_round()
{

   int i;

   i = env[cworld].nroundobst;
   env[cworld].nroundobst++;

   env[cworld].roundobst[i][0] = env[cworld].dx / 2;
   env[cworld].roundobst[i][1] = env[cworld].dy + 50;

   sprintf(drawdata_st,"added round object #%d", i);
   display_stat_message(drawdata_st);
   update_rendrobot();
}


void RobotDialog::add_sround()
{

   int i;

   i = env[cworld].nsroundobst;
   env[cworld].nsroundobst++;

   env[cworld].sroundobst[i][0] = env[cworld].dx / 2;
   env[cworld].sroundobst[i][1] = env[cworld].dy + 50;

   sprintf(drawdata_st,"added sround object #%d", i);
   display_stat_message(drawdata_st);
   update_rendrobot();
}


void RobotDialog::add_light()
{

   int i;

   i = env[cworld].nbulblights;
   env[cworld].nbulblights++;

   env[cworld].bulblights[i][0] = env[cworld].dx / 2;
   env[cworld].bulblights[i][1] = env[cworld].dy + 50;

   sprintf(drawdata_st,"added light object #%d", i);
   display_stat_message(drawdata_st);
   update_rendrobot();
}


void RobotDialog::add_tarea()
{

   int i;

   i = env[cworld].nfoodzones;
   env[cworld].nfoodzones++;

   env[cworld].foodzones[i][0] = env[cworld].dx / 2;
   env[cworld].foodzones[i][1] = env[cworld].dy + 50;
   env[cworld].foodzones[i][2] = 80;
   if (i > 0)
     env[cworld].foodzones[i][2] = env[cworld].foodzones[i-1][2];

   sprintf(drawdata_st,"added t_area object #%d", i);
   display_stat_message(drawdata_st);
   update_rendrobot();
}


void RobotDialog::conrobots()
{
	//conneccting robots if present
	//icona on

	//icona off
	//
	int id;
	if (real==0)
	{
	real=1;
	initRobots();
	conrobotsAct->setIcon(QIcon(":/images/epuck.png"));
	conrobotsAct->setIconText(tr("Disconnect all real robots"));

	}else
	{
	real=0;
	for (id=0;id<7;id++)
	{
		if (robots[id].connected==1)
		{
		setRobotSpeed(&robots[id], 0,0);//stop robots before disconnection
		disconnectRobot(&robots[id]);
		}

	}
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
    return QSize(widgetdx, widgetdy);
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
   int    i;
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

   int    **ob;
   int    *o;
   int    *ocf;
   int    a,d,s,obj;
   int    a1;
   int    stx, sty;
   int    heig;
   int    scolor;
   
   QRect labelxy(0,0,30,20);          // neuron labels 
   QPoint pxy;

   QPainter painter(this);
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);
  
   // draw the world and the robots
   // robots are shown during test_individual
   // or when the mouse is clicked on the main window
   if (testindividual_is_running == 1 || drawindividual == 1)
   {
     set_scale();

    // draw the world
    painter.setPen(Qt::black);
    for (i=0; i < env[cworld].nobstacles; i++)
          painter.drawLine(env[cworld].obstacles[i][0] * scale,env[cworld].obstacles[i][1]  * scale,env[cworld].obstacles[i][2]  * scale, env[cworld].obstacles[i][3]  * scale);
    painter.setPen(Qt::magenta);
    painter.setBrush(QBrush(Qt::magenta, Qt::SolidPattern));
    for (i=0; i < env[cworld].nroundobst; i++)
           painter.drawEllipse((env[cworld].roundobst[i][0] - env[cworld].radius) * scale,(env[cworld].roundobst[i][1] - env[cworld].radius)  * scale,env[cworld].radius * 2.0  * scale,env[cworld].radius * 2.0  * scale);
    for (i=0; i < env[cworld].nsroundobst; i++)
           painter.drawEllipse((env[cworld].sroundobst[i][0] - env[cworld].sradius)  * scale,(env[cworld].sroundobst[i][1] - env[cworld].sradius)  * scale,env[cworld].sradius * 2.0  * scale,env[cworld].sradius * 2.0  * scale);
    painter.setBrush(QBrush(Qt::green, Qt::NoBrush));
    for (i=0; i < env[cworld].nfoodzones; i++)
	{
	  if ((i % 2) == 0)
            painter.setPen(Qt::green);
	   else
            painter.setPen(Qt::blue);
          painter.drawEllipse((env[cworld].foodzones[i][0] - env[cworld].foodzones[i][2])  * scale, (env[cworld].foodzones[i][1] - env[cworld].foodzones[i][2]) * scale, env[cworld].foodzones[i][2] * 2.0  * scale, env[cworld].foodzones[i][2] * 2.0  * scale);
	}
    painter.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
    painter.setPen(Qt::yellow);
    for (i=0; i < env[cworld].nbulblights; i++)
           painter.drawEllipse((env[cworld].bulblights[i][0] - 10.0)  * scale, (env[cworld].bulblights[i][1] - 10.0)  * scale, 20.0  * scale, 20.0  * scale);

    painter.setBrush(QBrush(Qt::black, Qt::NoBrush));
    painter.setPen(Qt::black);
    sprintf(drawdata_st, "dx %d dy %d", env[cworld].dx, env[cworld].dy);
    pxy.setX(0);
    pxy.setY(env[cworld].dy * scale + 20);
    painter.drawText(pxy, drawdata_st);
    pxy.setX(0);
    pxy.setY(env[cworld].dy * scale + 40);
    painter.drawText(pxy, ffitness_descp);

    // draw the robots
    for(team=0,pind=ind; team < nteam; team++, pind++)
	{
          painter.setBrush(QBrush(Qt::black, Qt::NoBrush));
          painter.setPen(QColor(120,120,120,255));
	  if (team == cindividual)
            painter.drawEllipse((pind->pos[0] - 15.0)  * scale, (pind->pos[1] - 15.0)  * scale, 30  * scale, 30  * scale);

	  if (team == 0)
            painter.setPen(QColor(255,0,0,255));
	  if (team == 1)
            painter.setPen(QColor(0,0,255,255));
	  if (team == 2)
            painter.setPen(QColor(0,255,0,255));
	  if (team == 3)
            painter.setPen(QColor(160,0,160,255));

          painter.drawEllipse((pind->pos[0] - ROBOTRADIUS)  * scale, (pind->pos[1] - ROBOTRADIUS)  * scale, 2*ROBOTRADIUS  * scale, 2*ROBOTRADIUS  * scale);
	  headx = SetX(pind->pos[0],(float) 27.0, pind->direction);
	  heady = SetY(pind->pos[1],(float) 27.0, pind->direction);
	  painter.drawLine(pind->pos[0] * scale,pind->pos[1] * scale, headx * scale, heady * scale);

          if (drawtrace > 0 && testindividual_is_running == 1)
	  {
	    trace[run%drawtrace][0][team] = pind->pos[0];
	    trace[run%drawtrace][1][team] = pind->pos[1];
	    for(c=0; c < ((run%drawtrace) - 1); c++)
              painter.drawLine(trace[c][0][team] * scale, trace[c][1][team] * scale,trace[c+1][0][team] * scale, trace[c+1][1][team] * scale); 
	    if (run > drawtrace)
	     for(c=(run%drawtrace)+1; c < (drawtrace - 1); c++)
              painter.drawLine(trace[c][0][team] * scale, trace[c][1][team] * scale,trace[c+1][0][team] * scale, trace[c+1][1][team] * scale); 	    
	  }
	}
    for (i=0; i < env[cworld].nlandmarks; i++)
    {
	    float x         = env[cworld].landmark[i][0];
	    float y         = env[cworld].landmark[i][1];
	    float radius    = env[cworld].landmark[i][2];
	    int   red       = (int)(env[cworld].landmark[i][4] * 255);
	    QPen pen;
	    pen.setStyle(Qt::SolidLine);

	    pen.setColor(  QColor(red, 0, 0, 255)  );
	    painter.setBrush(QBrush(QColor(red, 0, 0, 255), Qt::NoBrush));
	    pen.setWidth(3);    
	    painter.setPen(pen);
            if (env[cworld].landmark[i][3] == 0.0)
              painter.drawEllipse((x-radius) * scale, (y-radius) * scale, radius * 2.0  * scale, radius * 2.0  * scale);
            if (env[cworld].landmark[i][3] == 1.0)
	      painter.drawLine((x - radius)*scale, y, (x + radius)*scale, y);
            if (env[cworld].landmark[i][3] == 2.0)	 
	      painter.drawLine(x*scale, (y - radius)*scale, x*scale, (y + radius)*scale);
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
	int mode;
	struct individual *pind;

	
	x=event->x();
	y=event->y();
	b=event->button();


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
	  mode = 0;
          set_scale();
	  mousex = x;
	  mousey = y;
	  //   on a robot: set the robot as the current individual
	  for(i=0, pind = ind; i < nteam; i++, pind++)
	  {
	    if (mdist((float) x / scale,(float) y / scale, pind->pos[0], pind->pos[1]) < ROBOTRADIUS)
	    {
	      cindividual = i;
	      mode = 1;
	    }
	  }
	 }
         olddrawi = drawindividual; 
         drawindividual = 1;
         rendRobot->repaint();
	 QCoreApplication::processEvents();
         drawindividual = olddrawi;
	}

}


/*
 * handle mouse move events
 */
void 
RendRobot::mouseMoveEvent(QMouseEvent *event)
{

	int x,y;
	int i;
	double dist;
	int mode;
	struct individual *pind;
	QPoint xy;
	float *o;
	int   cn;
	char  nobject[24];
	int   idobject;


	if (evolution_is_running == 0)
	{

	x=event->x();
	y=event->y();

	pind = (ind + cindividual);
	mode = 0;

	// move current robot
	if (mdist((float) mousex / scale,(float) mousey / scale, pind->pos[0], pind->pos[1]) < ROBOTRADIUS)
	      {
                o = pind->pos;
		mode = 3;
		sprintf(nobject,"robot");
                idobject = cindividual;
		manual_start = 1;
	      }
	// move world object
	if (mode == 0)
	  for(i=0; i < env[cworld].nobstacles; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].obstacles[i][0], env[cworld].obstacles[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].obstacles[i];
		  mode = 3;
		  sprintf(nobject,"wall-a");
                  idobject = i;
		}
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].obstacles[i][2], env[cworld].obstacles[i][3]);
                if (dist < 10)
		{
		  o = env[cworld].obstacles[i];
		  o = (o + 2);
		  mode = 3;
		  sprintf(nobject,"wall-b");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nroundobst; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].roundobst[i][0], env[cworld].roundobst[i][1]);
                if (dist < env[cworld].radius)
		{
		  o = env[cworld].roundobst[i];
		  mode = 3;
		  sprintf(nobject,"round");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nsroundobst; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].sroundobst[i][0], env[cworld].sroundobst[i][1]);
                if (dist < env[cworld].sradius)
		{
		  o = env[cworld].sroundobst[i];
		  mode = 3;
		  sprintf(nobject,"sround");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nfoodzones; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].foodzones[i][0], env[cworld].foodzones[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].foodzones[i];
		  mode = 3;
		  sprintf(nobject,"t_area");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nbulblights; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].bulblights[i][0], env[cworld].bulblights[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].bulblights[i];
		  mode = 3;
		  sprintf(nobject,"light");
                  idobject = i;
		}
	      }
	if (mode == 3)
		{
		*o += (x - mousex) / scale;
		*(o + 1) += (y - mousey) / scale;
		if ((*(o + 1) * scale) >= renderdy)
                  remove_object(nobject, idobject);
                if (strcmp(nobject, "robot") == 0)
		{
		 if (*o > env[cworld].dx || *(o + 1) > env[cworld].dy)
		   pind->dead = 1;
		  else
		   pind->dead = 0;
		}
	        drawindividual = 1;
		sprintf(drawdata_st,"object %s #%d moved to %.1f %.1f", nobject, idobject, *o, *(o + 1));
		display_stat_message(drawdata_st);
	        repaint();
	        drawindividual = 0;
		mousex = x;
		mousey = y;
		}

	}


}


/*
 * handle mouse release events by constraints movements to the grid (if any)
 */
void 
RendRobot::mouseReleaseEvent(QMouseEvent *event)
{

	int x,y;
	int i;
	double dist;
	int mode;
	struct individual *pind;
	QPoint xy;
	float *o;
	float oldp1,oldp2;
	int   cn;
	char  nobject[24];
	int   idobject;


	if (evolution_is_running == 0 && grid > 0)
	{

	mousex=event->x();
	mousey=event->y();

	pind = (ind + cindividual);
	mode = 0;

	// move current robot
	if (mdist((float) mousex / scale,(float) mousey / scale, pind->pos[0], pind->pos[1]) < ROBOTRADIUS)
	      {
                o = pind->pos;
		mode = 3;
		sprintf(nobject,"robot");
                idobject = cindividual;
		manual_start = 1;
	      }
	// move world object
	if (mode == 0)
	  for(i=0; i < env[cworld].nobstacles; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].obstacles[i][0], env[cworld].obstacles[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].obstacles[i];
		  mode = 3;
		  sprintf(nobject,"wall-a");
                  idobject = i;
		}
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].obstacles[i][2], env[cworld].obstacles[i][3]);
                if (dist < 10)
		{
		  o = env[cworld].obstacles[i];
		  o = (o + 2);
		  mode = 3;
		  sprintf(nobject,"wall-b");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nroundobst; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].roundobst[i][0], env[cworld].roundobst[i][1]);
                if (dist < env[cworld].radius)
		{
		  o = env[cworld].roundobst[i];
		  mode = 3;
		  sprintf(nobject,"round");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nsroundobst; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].sroundobst[i][0], env[cworld].sroundobst[i][1]);
                if (dist < env[cworld].sradius)
		{
		  o = env[cworld].sroundobst[i];
		  mode = 3;
		  sprintf(nobject,"sround");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nfoodzones; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].foodzones[i][0], env[cworld].foodzones[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].foodzones[i];
		  mode = 3;
		  sprintf(nobject,"t_area");
                  idobject = i;
		}
	      }
	if (mode == 0)
	  for(i=0; i < env[cworld].nbulblights; i++)
	      {
	        dist = mdist((float) mousex / scale,(float) mousey / scale, env[cworld].bulblights[i][0], env[cworld].bulblights[i][1]);
                if (dist < 10)
		{
		  o = env[cworld].bulblights[i];
		  mode = 3;
		  sprintf(nobject,"light");
                  idobject = i;
		}
	      }
	if (mode == 3)
		{
		oldp1 = *o;
		oldp2 = *(o + 1);
		*o = (float) (((int) *o) / grid * grid);
		*(o + 1) = (float) (((int) *(o + 1)) / grid * grid);
		if ((oldp1 - *o) > (grid / 2))
		  *o += grid;
		if ((oldp2 - *(o + 1)) > (grid / 2))
		  *(o + 1) += grid;
	        drawindividual = 1;
		sprintf(drawdata_st,"object %s #%d released to %.1f %.1f", nobject, idobject, *o, *(o + 1));
		display_stat_message(drawdata_st);
	        repaint();
	        drawindividual = 0;
		}

	}


}

/*
 * handle mouse buttons
 */
void 
RendRobot::mouseDoubleClickEvent(QMouseEvent *event)
{
	
/*	
	x=event->x();
	y=event->y();
	b=event->button();

	pind = (ind + cindividual);
        pind->pos[0] = x;
	pind->pos[1] = y;
	olddrawi = drawindividual; 
	drawindividual = 1;
	repaint();
	drawindividual = olddrawi;
 */   
}


/* 
 * remove the object that have been drag and drop out of the world 
 */
void
remove_object(char *nobject, int idobject)


{

   int    i,ii;
   
   if ((strcmp(nobject, "wall-a") == 0) || (strcmp(nobject, "wall-b") == 0))
   {
     for (i=idobject; i < (env[cworld].nobstacles - 1); i++)
       for (ii=0; ii < 4; ii++)
          env[cworld].obstacles[i][ii]  = env[cworld].obstacles[i+1][ii];
     env[cworld].nobstacles--;
   }
   if (strcmp(nobject, "round") == 0)
   {
     for (i=idobject; i < (env[cworld].nroundobst - 1); i++)
       for (ii=0; ii < 2; ii++)
          env[cworld].roundobst[i][ii]  = env[cworld].roundobst[i+1][ii];
     env[cworld].nroundobst--;
   }
   if (strcmp(nobject, "sround") == 0)
   {
     for (i=idobject; i < (env[cworld].nsroundobst - 1); i++)
       for (ii=0; ii < 2; ii++)
          env[cworld].sroundobst[i][ii]  = env[cworld].sroundobst[i+1][ii];
     env[cworld].nsroundobst--;
   }
   if (strcmp(nobject, "light") == 0)
   {
     for (i=idobject; i < (env[cworld].nbulblights - 1); i++)
       for (ii=0; ii < 2; ii++)
          env[cworld].bulblights[i][ii]  = env[cworld].bulblights[i+1][ii];
     env[cworld].nbulblights--;
   }
   if (strcmp(nobject, "t_area") == 0)
   {
     for (i=idobject; i < (env[cworld].nfoodzones - 1); i++)
       for (ii=0; ii < 2; ii++)
          env[cworld].foodzones[i][ii]  = env[cworld].foodzones[i+1][ii];
     env[cworld].nfoodzones--;
   }

}




/*
 * set the scale for the world display
 */
void
set_scale()
{
    if (env[cworld].dx > env[cworld].dy)
      scale = (float) widgetdx  / (float) (env[cworld].dx + 10);
    else
      scale = (float) (widgetdy - 50)  / (float) (env[cworld].dy + 10);
   

}


#endif
