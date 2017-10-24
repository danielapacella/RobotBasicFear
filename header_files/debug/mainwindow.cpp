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

#ifdef EVOGRAPHICS

#ifdef EVOWINDOWS
#include <QtGui>
#include <QApplication>
#include <QScrollArea>
#include <QCheckBox>
#else //Linux mode
#include <Qt/qapplication.h>
#include <Qt/qscrollarea.h>
#include <Qt/qgridlayout.h>
#include <Qt/qfiledialog.h>
#include <Qt/qmessagebox.h>
#include <Qt/qicon.h>
#include <Qt/qmenubar.h>
#include <Qt/qaction.h>
#include <Qt/qtoolbar.h>
#include <Qt/qstatusbar.h>
#include <Qt/qsettings.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qlineedit.h>
#endif

#include "mainwindow.h"
#include "public.h"

#ifdef EVOWINDOWS
const int IdRole = Qt::UserRole;
#endif

MainWindow *mainWin;             // The main window
QLineEdit  *lineedit;            // The command line widget

int mainrendparam = 0;           // Whether the param window is on or off
int mainrendnetwork = 0;         // Whether the network window is on or off
int mainrendrobot = 0;           // Whether the robot window is on or off

//------------------------------------------------------------------------
//---------------- Public Functions --------------------------------------
//------------------------------------------------------------------------

/*
 * display a message on the statusbar
 */
void display_stat_message(char *emessage)

{
    if (console == 0)
    {
     mainWin->statusBar()->showMessage(emessage);
     mainWin->statusBar()->repaint();
    }
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
 *
 */
void display_message(char *emessage)

{
   if (helptn > 0 && helptn < 40)
    {
       sprintf(helpt[helptn - 1],"%s", emessage);
       helptn++;
    }
}

//------------------------------------------------------------------------
//---------------- Private Functions -------------------------------------
//------------------------------------------------------------------------


int main(int argc, char *argv[])
{


     Q_INIT_RESOURCE(application);
     QApplication app(argc, argv);
     mainWin = new MainWindow;
     init_khepsim();
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

}


/*
 * Create a gridlayout that include a graphic
 * and a lineedit widget
 */
Window::Window(MainWindow* main)
{
    QGridLayout *mainLayout = new QGridLayout;

    init_rendevolution();
    add_evolution_widget(mainLayout, 0, 0);

    init_rendrobot(1);
    add_robot_widget(mainLayout, 0, 0);

    init_rendnetwork(1);
    add_network_widget(mainLayout, 0, 1);

    setLayout(mainLayout);

    lineedit = new QLineEdit( "", this );
    connect( lineedit, SIGNAL( returnPressed () ), main, SLOT( qt_exec_command() ) );
    mainLayout->addWidget( lineedit, 1, 0 );

}



void MainWindow::save_f()
{

    char *f;
    QByteArray filen;


        QString fileName = QFileDialog::getSaveFileName(this,
		    "Choose a filename to save under",
                    "",
                    "*.cf *.net world?.env *.phe");

        if (fileName.endsWith("net"))
	{
          filen = fileName.toAscii();
          f = filen.data();
	  strcpy(filename, f);
          save_net_blocks(filename);
	}
	else
	 if (fileName.endsWith("env"))
	 {
          filen = fileName.toAscii();
          f = filen.data();
	  save_world();
	 }
	 else
	  if (fileName.endsWith("cf"))
	  {
           filen = fileName.toAscii();
           f = filen.data();
	   strcpy(filename, f);
	   save_parameters(filename);
	  }
	 else
	  if (fileName.endsWith("phe"))
	  {
           filen = fileName.toAscii();
           f = filen.data();
	   strcpy(filename, f);
	   save_phenotype(filename);
	  }

}

void MainWindow::open()
{

    char *f;
    QByteArray filen;


        QString fileName = QFileDialog::getOpenFileName(this,
		    "Choose a filename to save under",
                    "",
                    "*.gen *.cf *.net *.fit world?.env *.phe");


        if (fileName.endsWith("gen"))
	{
          filen = fileName.toAscii();
          f = filen.data();
	  strcpy(filename, f);
	  loadallg(-1, filename);
	}
	else
	 if (fileName.endsWith("fit"))
	 {
          filen = fileName.toAscii();
          f = filen.data();
	  strcpy(filename, f);
	  loadstatistics(filename, 1);
	 }
	 else
	  if (fileName.endsWith("cf"))
	  {
           filen = fileName.toAscii();
           f = filen.data();
	   strcpy(filename, f);
	   load_new_parameters(filename);
	  }
	  else
	   if (fileName.endsWith("net"))
	   {
            filen = fileName.toAscii();
            f = filen.data();
	    strcpy(filename, f);
	    if (load_net_blocks(filename) == 0)
	      display_warning("unable to load the file");
	   }
	  else
	   if (fileName.endsWith("env"))
	   {
            filen = fileName.toAscii();
            f = filen.data();
	    load_world();
	   }
	  else
	   if (fileName.endsWith("phe"))
	   {
            filen = fileName.toAscii();
            f = filen.data();
	    strcpy(filename, f);
	    load_phenotype(filename);
	   }
}






void MainWindow::about()
{
   QMessageBox::about(this, tr("About Evorobot*"),

            tr("<b>Evorobot* 2.0</b> is a tool for developing adaptive robots"
               "<br>Copyright (C) 2009 "
               "<br>Stefano Nolfi, LARAL, ISTC-CNR, Roma, Italy, http://laral.istc.cnr.it"));
}




void MainWindow::createActions()
{


    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save data on a file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save_f()));

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

    runlearnAct = new QAction(tr("&Learning"), this);
    runlearnAct->setStatusTip(tr("Run the learning process"));
    connect(runlearnAct, SIGNAL(triggered()), this, SLOT(qt_learning()));

    testiAct = new QAction(tr("&Test"), this);
    testiAct->setStatusTip(tr("Test one individual"));
    connect(testiAct, SIGNAL(triggered()), this, SLOT(qt_test_one_individual()));

    testbiAct = new QAction(tr("&Test best"), this);
    testbiAct->setStatusTip(tr("Test the best individual"));
    connect(testbiAct, SIGNAL(triggered()), this, SLOT(qt_test_best_individual()));

    testallAct = new QAction(tr("&Test all"), this);
    testallAct->setStatusTip(tr("Test all individuals"));
    connect(testallAct, SIGNAL(triggered()), this, SLOT(qt_test_all_individuals()));

    runcreatelAct = new QAction(tr("&Create lineage"), this);
    runcreatelAct->setStatusTip(tr("Create lineage.gen file"));
    connect(runcreatelAct, SIGNAL(triggered()), this, SLOT(qt_create_lineage()));

    userbreakAct = new QAction(tr("&Stop"), this);
    userbreakAct->setStatusTip(tr("Stop evolution or Test"));
    connect(userbreakAct, SIGNAL(triggered()), this, SLOT(qt_userbreak()));

    displayallstatAct = new QAction(tr("&Display statistics"), this);
    displayallstatAct->setStatusTip(tr("Display statistics"));
    connect(displayallstatAct, SIGNAL(triggered()), this, SLOT(qt_displayallstat()));

    display_sampleAct = new QAction(tr("&Display sampled data"), this);
    display_sampleAct->setStatusTip(tr("Display sample data"));
    connect(display_sampleAct, SIGNAL(triggered()), this, SLOT(qt_display_sample()));

    displaypa_evoAct = new QAction(tr("&Display evolution par."), this);
    displaypa_evoAct->setStatusTip(tr("Display evolution parameters"));
    connect(displaypa_evoAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_evo()));

    displaypa_indAct = new QAction(tr("&Display individual par."), this);
    displaypa_indAct->setStatusTip(tr("Display individual parameters"));
    connect(displaypa_indAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_ind()));

    displaypa_lifAct = new QAction(tr("&Display lifetime par."), this);
    displaypa_lifAct->setStatusTip(tr("Display lifetime parameters"));
    connect(displaypa_lifAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_lif()));

    displaypa_disAct = new QAction(tr("&Display display par."), this);
    displaypa_disAct->setStatusTip(tr("Display display parameters"));
    connect(displaypa_disAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_dis()));

    displaypa_envAct = new QAction(tr("&Display environ. par."), this);
    displaypa_envAct->setStatusTip(tr("Display environmental parameters"));
    connect(displaypa_envAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_env()));

    displaypa_netAct = new QAction(tr("&Display net. par."), this);
    displaypa_netAct->setStatusTip(tr("Display network architecture parameters"));
    connect(displaypa_netAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_net()));

    displaypa_fitAct = new QAction(tr("&Display fit. par."), this);
    displaypa_fitAct->setStatusTip(tr("Display available fitness functions"));
    connect(displaypa_fitAct, SIGNAL(triggered()), this, SLOT(qt_displaypa_fit()));

    showparamAct = new QAction(QIcon(":/images/parameters.png"), tr("Parameters window"), this);
    showparamAct->setStatusTip(tr("Open/close parameters window"));
    connect(showparamAct, SIGNAL(triggered()), this, SLOT(show_param()));

    showrobotAct = new QAction(QIcon(":/images/robot-env.png"), tr("Robot window"), this);
    showrobotAct->setStatusTip(tr("Open/close robot window"));
    connect(showrobotAct, SIGNAL(triggered()), this, SLOT(show_robot()));

    shownetworkAct = new QAction(QIcon(":/images/showNetwork.png"), tr("Nervous-system window"), this);
    shownetworkAct->setStatusTip(tr("Open/close nervous-system window"));
    connect(shownetworkAct, SIGNAL(triggered()), this, SLOT(show_network()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    runMenu = menuBar()->addMenu(tr("&Run"));
    runMenu->addAction(runevoAct);
    runMenu->addAction(runlearnAct);
    runMenu->addAction(testiAct);
    runMenu->addAction(testbiAct);
    runMenu->addAction(testallAct);
    runMenu->addAction(runcreatelAct);
    runMenu->addAction(userbreakAct);

    displayMenu = menuBar()->addMenu(tr("&Display"));
    displayMenu->addAction(displaypa_evoAct);
    displayMenu->addAction(displaypa_indAct);
    displayMenu->addAction(displaypa_lifAct);
    displayMenu->addAction(displaypa_disAct);
    displayMenu->addAction(displaypa_envAct);
    displayMenu->addAction(displaypa_netAct);
    displayMenu->addAction(displaypa_fitAct);
    fileMenu->addSeparator();
    displayMenu->addAction(displayallstatAct);
    displayMenu->addAction(display_sampleAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    fileToolBar = addToolBar(tr("Show"));
    fileToolBar->addAction(showparamAct);
    fileToolBar->addAction(showrobotAct);
    fileToolBar->addAction(shownetworkAct);

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
     }
    else
     {
      hide_rendrobot();
      mainrendrobot = 0;
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


void MainWindow::qt_exec_command()

{

    char *cst;
    QByteArray carray;
    char message[128];

    QString c = lineedit->text();
    carray = c.toAscii();
    cst = carray.data();
    sprintf(message,"executed command: %s", cst);
    exec_commandline(cst);
    display_stat_message(message);
    lineedit->setText("");

}



void MainWindow::qt_evolution()
{

    evolution_is_running=1;
    runevolution();
    evolution_is_running=0;

}

void MainWindow::qt_learning()
{

    evolution_is_running=1;
    runlearning();
    evolution_is_running=0;

}

void MainWindow::qt_test_one_individual()
{

    testindividual_is_running=1;
    test_one_individual();
    testindividual_is_running=0;

}

void MainWindow::qt_test_best_individual()
{

    testindividual_is_running=1;
    select_besti();
    test_one_individual();
    testindividual_is_running=0;

}

void MainWindow::qt_test_all_individuals()
{

    evolution_is_running=1;
    test_all_individuals();
    evolution_is_running=0;

}

void MainWindow::qt_create_lineage()
{
    create_lineage();
}


void MainWindow::qt_userbreak()
{

    userbreak = 1;
}

void MainWindow::qt_displayallstat()
{

    display_all_stat();
}


void MainWindow::qt_displaypa_evo()
{
    display_evolution_parameters();
}

void MainWindow::qt_displaypa_ind()
{
    display_ind_parameters(0);
}

void MainWindow::qt_displaypa_lif()
{
    display_lifetime_parameters();
}

void MainWindow::qt_displaypa_dis()
{
    display_display_parameters();
}

void MainWindow::qt_displaypa_env()
{
    display_env_parameters();
}

void MainWindow::qt_displaypa_net()
{
    display_net_parameters();
}

void MainWindow::qt_displaypa_fit()
{
    display_fit_parameters(0);
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
 console = 1;

 init_khepsim();
 runevolution();
 return 0;
}

void
update_rendnetwork()

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

  if (verbose >= 2)
    printf("%s\n", emessage);

}


void display_message(char *emessage)

{

  if (verbose >= 1)
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

#endif
