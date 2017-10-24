/*
 * Evorobot* - mainwindow.cpp
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

//#include "defs.h"

#include "public.h"

#ifdef EVOGRAPHICS
#include <QtGui>
#include <QApplication>
#include <QScrollArea>
#include <QCheckBox>

#include "mainwindow.h"

#ifdef EVOWINDOWS
const int IdRole = Qt::UserRole;
#endif

MainWindow *mainWin;             // The main window
QLineEdit  *lineedit;            // The command line widget

int mainrendparam = 0;           // Whether the param window is on or off
int mainrendnetwork = 0;         // Whether the network window is on or off
int mainrendrobot = 0;           // Whether the robot window is on or off
int mainrendevolution = 0;       // Whether the evolution window is on or off
int mainrendrobot3d = 0;         // Whether the robot3d window is on or off
int wincontroller = 0;           // controller window opens automatically
int winadaptation = 0;           // adaptation window opens automatically
int winrobotenv   = 0;           // robot-env window opens automatically
int winrobotenv3d = 0;           // robot-env3D window opens automatically
int winparameters = 0;           // parameters window opens automatically
int winx = -1;                   // x coordinate of main window (-1 = undefined)
int winy = -1;                   // y coordinate of main window (-1 = undefined)
int windx = 0;                   // dx of controller main window (0 = undefined)
int windy = 0;                   // dy of controller main window (0 = undefined)
int userbreak=0;                 // Whether the user press the stop buttom   
void init_modules(); 
int userlevel=5;                 // the user expertise (0=beginner, 5= expert)

//------------------------------------------------------------------------
//---------------- Public Functions --------------------------------------
//------------------------------------------------------------------------

/*
 * display a message on the statusbar
 */
void display_stat_message(char *emessage)

{

     mainWin->statusBar()->showMessage(emessage);
     mainWin->statusBar()->repaint();
}


/*
 * display an error message in a box and exit (graphic)
 */
void
display_error(char *emessage)

{
   mainWin->error(emessage);
}

/*
 * display a warinig message in a box (graphic)
 */
void
display_warning(char *emessage)

{

   mainWin->warning(emessage);


}

/*
 * display a warinig message in a box (graphic)
 */
void
update_events()
{
    QCoreApplication::processEvents();
}




//------------------------------------------------------------------------
//---------------- Private Functions -------------------------------------
//------------------------------------------------------------------------


int main(int argc, char *argv[])
{


     Q_INIT_RESOURCE(application);
     QApplication app(argc, argv);
     mainWin = new MainWindow;
     init_modules();
     if (winx >= 0 && winy >= 0)
        mainWin->move(winx, winy);
     if (windx > 0 && windy > 0)
        mainWin->resize(windx, windy);
     mainWin->show();

     return app.exec();

}


MainWindow::MainWindow()
{

    Window* window = new Window(this);

    QScrollArea* scroll = new QScrollArea( this );
    setCentralWidget( scroll );
    scroll->setWidget( window );

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();


    setWindowTitle("Evorobot*");

#ifdef EVOLANGUAGEIT
    setWindowTitle("Edubot");
#endif
}


/*
 * Create a gridlayout that include a graphic
 * and a lineedit widget
 */
Window::Window(MainWindow* main)
{
    QGridLayout *mainLayout = new QGridLayout;

    init_rendevolution(1);
    add_evolution_widget(mainLayout, 1, 1);

    init_rendrobot(1);
    add_robot_widget(mainLayout, 1, 0);

    init_rendnetwork(1);
    add_controlstate_widget(mainLayout, 0, 0);
    add_network_widget(mainLayout, 0, 1);

    setLayout(mainLayout);
    
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Evorobot*"),

   tr("<b>Evorobot* 2.0</b> is a tool for developing adaptive robots"
       "<br>Copyright (C) 2009"
       "<br>Stefano Nolfi & Onofrio Gigliotta, LARAL, ISTC-CNR, Roma, Italy, http://laral.istc.cnr.it"));
}




void MainWindow::createActions()
{

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    runevoAct = new QAction(tr("&Evolution"), this);
    runevoAct->setStatusTip(tr("Run the evolutionary process"));
    connect(runevoAct, SIGNAL(triggered()), this, SLOT(qt_evolution()));

    adaptAct = new QAction(tr("&Adapt"), this);
    adaptAct->setStatusTip(tr("Run the adaptive process"));
    connect(adaptAct, SIGNAL(triggered()), this, SLOT(qt_adapt()));

    testbiAct = new QAction(tr("&Test best"), this);
    testbiAct->setStatusTip(tr("Test the best individual"));
    connect(testbiAct, SIGNAL(triggered()), this, SLOT(qt_test_best_individual()));

    testallAct = new QAction(tr("&Test all"), this);
    testallAct->setStatusTip(tr("Test all individuals"));
    connect(testallAct, SIGNAL(triggered()), this, SLOT(qt_test_all_individuals()));

    userbreakAct = new QAction(tr("&Stop"), this);
    userbreakAct->setStatusTip(tr("Stop evolution or Test"));
    connect(userbreakAct, SIGNAL(triggered()), this, SLOT(qt_userbreak()));

    itestAct = new QAction(QIcon(":/images/test.png"), tr("Test"), this);
    itestAct->setStatusTip(tr("Test robot/s"));
    connect(itestAct, SIGNAL(triggered()), this, SLOT(qt_test_individual()));

    iadaptAct = new QAction(QIcon(":/images/adaptation.png"), tr("Adapt"), this);
    iadaptAct->setStatusTip(tr("Run the adaptation process"));
    connect(iadaptAct, SIGNAL(triggered()), this, SLOT(qt_adapt()));

    ievolveAct = new QAction(QIcon(":/images/evolution.png"), tr("Evolve"), this);
    ievolveAct->setStatusTip(tr("Run the evolution process/s"));
    connect(ievolveAct, SIGNAL(triggered()), this, SLOT(qt_evolution()));

    istopAct = new QAction(QIcon(":/images/light.png"), tr("Stop"), this);
    istopAct->setStatusTip(tr("Stop test or adaptation/evolution/s"));
    connect(istopAct, SIGNAL(triggered()), this, SLOT(qt_userbreak()));
    istopAct->setEnabled(false);

    showparamAct = new QAction(QIcon(":/images/parameters.png"), tr("Parameters window"), this);
    showparamAct->setStatusTip(tr("Open/close parameters window"));
    connect(showparamAct, SIGNAL(triggered()), this, SLOT(show_param()));

    showevolutionAct = new QAction(QIcon(":/images/fitness.png"), tr("Adaptation window"), this);
    showevolutionAct->setStatusTip(tr("Open/close adaptation window"));
    connect(showevolutionAct, SIGNAL(triggered()), this, SLOT(show_evolution()));

    shownetworkAct = new QAction(QIcon(":/images/showNetwork.png"), tr("Controller window"), this);
    shownetworkAct->setStatusTip(tr("Open/close nervous-system window"));
    connect(shownetworkAct, SIGNAL(triggered()), this, SLOT(show_network()));


    showrobotAct = new QAction(QIcon(":/images/robot-env2D.png"), tr("Robot/Env 2D-window"), this);
    showrobotAct->setStatusTip(tr("Open/close robot window"));
    connect(showrobotAct, SIGNAL(triggered()), this, SLOT(show_robot()));

    showrobot3dAct = new QAction(QIcon(":/images/robot-env.png"), tr("Robot/Env 3D-window"), this);
    showrobot3dAct->setStatusTip(tr("Open/close robot 3dwindow"));
    connect(showrobot3dAct, SIGNAL(triggered()), this, SLOT(show_robot3d()));


}



void MainWindow::createMenus()
{
    mMenu = menuBar()->addMenu(tr("&Run"));

    mMenu->addAction(testbiAct);
    mMenu->addAction(testallAct);
    mMenu->addAction(runevoAct);
    mMenu->addAction(adaptAct);
    mMenu->addAction(userbreakAct);
    mMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{

    mToolBar = addToolBar(tr("Show"));
    mToolBar->addAction(itestAct);
    mToolBar->addAction(iadaptAct);
    mToolBar->addAction(ievolveAct);
    mToolBar->addAction(istopAct);
    mToolBar->addAction(showparamAct);
    mToolBar->addAction(showevolutionAct);
    mToolBar->addAction(shownetworkAct);
    mToolBar->addAction(showrobotAct);
    mToolBar->addAction(showrobot3dAct);

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


/*
 * show or hide the network window
 */
void MainWindow::show_network()
{
    if (mainrendnetwork == 0)
     {
      init_rendnetwork(2);
      mainrendnetwork = 1;
      shownetworkAct->setEnabled(false);
     }
    else
     {
      hide_rendnetwork();
      mainrendnetwork = 0;
     }
}


/*
 * show or hide the robot window
 */
void MainWindow::show_robot()
{
    if (mainrendrobot == 0)
     {
      init_rendrobot(2);
      mainrendrobot = 1;
      showrobotAct->setEnabled(false);
     }
    else
     {
      hide_rendrobot();
      mainrendrobot = 0;
     }

}

/*
 * show or hide the robot window
 */
void MainWindow::show_robot3d()
{

#ifdef EVO3DGRAPHICS
    init_rend3drobot();
    showrobot3dAct->setEnabled(false);

#endif 
}




/*
 * show or hide the robot window
 */
void MainWindow::show_evolution()
{
    if (mainrendevolution == 0)
     {
      init_rendevolution(2);
      mainrendevolution = 1;
      showevolutionAct->setEnabled(false);

     }
    else
     {
      hide_rendevolution();
      mainrendevolution = 0;
     }


}

/*
 * show or hide the parameter window
 */
void MainWindow::show_param()
{
    if (mainrendparam == 0)
     {
      init_rendparam();
      mainrendparam = 1;
      showparamAct->setEnabled(false);
     }
    else
     {
      hide_rendparam();
      mainrendparam = 0;
     }

}


/****************************************************************************
 * SLOTS toward functions that do not belong to the graphic interface
 */




void MainWindow::qt_evolution()
{

    itestAct->setEnabled(false);
    iadaptAct->setEnabled(false);
    ievolveAct->setEnabled(false);
    istopAct->setEnabled(true);
    runevolution();
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);

}


void MainWindow::qt_adapt()
{

    itestAct->setEnabled(false);
    iadaptAct->setEnabled(false);
    ievolveAct->setEnabled(false);
    istopAct->setEnabled(true);
    runadapt();
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);

}


void MainWindow::qt_test_individual()
{
    
    itestAct->setEnabled(false);
    iadaptAct->setEnabled(false);
    ievolveAct->setEnabled(false);
    istopAct->setEnabled(true);
    test(0);
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);


}


void MainWindow::qt_test_best_individual()
{

    itestAct->setEnabled(false);
    iadaptAct->setEnabled(false);
    ievolveAct->setEnabled(false);
    istopAct->setEnabled(true);
    test(1);
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);


}

void MainWindow::qt_test_all_individuals()
{
    itestAct->setEnabled(false);
    iadaptAct->setEnabled(false);
    ievolveAct->setEnabled(false);
    istopAct->setEnabled(true);
    test(2);
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);


}


void MainWindow::qt_userbreak()
{

    userbreak = 1;
    itestAct->setEnabled(true);
    iadaptAct->setEnabled(true);
    ievolveAct->setEnabled(true);
    istopAct->setEnabled(false);


}


void MainWindow::error(char *emessage)
{
   QMessageBox::about(this, "ERROR", emessage);
}

void MainWindow::warning(char *emessage)
{
   QMessageBox::about(this, "WARNING", emessage);
}

#else

int main()
{
 userbreak = 0;

 init_modules();
 runevolution();
 return 0;
}


void
update_renderarea()

{

}

void
update_basic_graphic()

{

}

void
create_neurons_labels()

{


}

/*
 * display a message on the statusbar
 */
void display_stat_message(char *emessage)

{

    printf("%s\n", emessage);

}


void display_message(char *emessage)

{

    printf("%s\n", emessage);


}

void display_error(char *emessage)

{

    printf("ERROR:%s\n", emessage);


}

void display_warning(char *emessage)

{

    printf("WARNING:%s\n", emessage);


}

void update_rendevolution(int mode)
{

}

void update_rendnetwork(int cycle)
{

}

void update_rendrobot()
{

}

void update_events(){}
#endif

/*
 * ininitialize all define modules and load parameters from evorobot.cf
 */
void
init_modules()
{


    int p;
    int team;
    struct  individual *pind;


    // 1. we create the Robot, Network, and Evolution classes and parameters
	create_epuck_par();
    create_network_par();
    create_evolution_par();
    create_environment_par();

    create_iparameter("userlevel", "display",&userlevel, 0, 5, "user's expertize 0=beginner, 5=expert");
    create_iparameter("wincontroller", "display",&wincontroller, 0, 0, "controller window opens automatically");
    create_iparameter("winadaptation", "display",&winadaptation, 0, 0, "adaptation window opens automatically");
    create_iparameter("winrobotenv", "display",&winrobotenv, 0, 0, "robot-environment 2D-window opens automatically");
    create_iparameter("winrobotenv3D", "display",&winrobotenv3d, 0, 0, "robot-environment 3d-window opens automatically");
    create_iparameter("winparameters", "display",&winparameters, 0, 0, "parameters window opens automatically");
    create_iparameter("winx", "display",&winx, 0, 0, "x coordinate of controller window");
    create_iparameter("winy", "display",&winy, 0, 0, "y coordinate of controller window");
    create_iparameter("windx", "display",&windx, 0, 0, "dx of controller window");
    create_iparameter("windy", "display",&windy, 0, 0, "dy of controller window");

    // 2. we load parameters
    loadparameters();

    // 3. we configure the Robot, Network, and Evolution classes
    init_environment();
    init_epuck();
    init_network();
    init_evolution();
    (*pinit_robot_pos)(); // this should be moved in init_robot_env();
    //initialize_robot_position(); // this should be moved in init_robot_env();

        // 4. we open the secondary windows if requested
        if (wincontroller > 0)
         {
          mainWin->show_network();
          //init_rendnetwork(2);
          //mainrendnetwork = 1;
         }
        if (winrobotenv > 0)
         {
          mainWin->show_robot();
          //init_rendrobot(2);
          //mainrendrobot = 1;
         }
        if (winrobotenv3d > 0)
         {
          mainWin->show_robot3d();
         }
        if (winadaptation > 0)
         {
          mainWin->show_evolution();

          //init_rendevolution(2);
          //mainrendevolution = 1;
         }
        if (winparameters > 0)
         {
          mainWin->show_param();
          //init_rendparam();
          //mainrendparam = 1;
         }

}

