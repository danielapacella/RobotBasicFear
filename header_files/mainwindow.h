/* 
 * Evorobot* - mainwindow.h
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

#include "mode.h"



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef EVOWINDOWS 
#include <QMainWindow>
#include <QWidget>
#include <QIcon>
#else //Linux mode
#include <QtGui/qmainwindow.h>
#include <QtGui/qwidget.h>
#endif


class QAction;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void error(char *emessage);
    void warning(char *emessage);

protected:

private slots:
    void about();
    void qt_evolution();
    void qt_adapt();
    void qt_test_individual();
    void qt_test_best_individual();
    void qt_test_all_individuals();
    void qt_userbreak();

public slots:
	void show_network();
    void show_robot();
    void show_robot3d();
    void show_param();
    void show_evolution();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QString curFile;

    QMenu *mMenu;
    QMenu *helpMenu;
    QToolBar *mToolBar;
    QAction *newAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *runevoAct;
    QAction *adaptAct;
    QAction *testbiAct;
    QAction *testallAct;
    QAction *userbreakAct;
    QAction *shownetworkAct;
    QAction *showrobotAct;
    QAction *showrobot3dAct;
    QAction *showparamAct;
    QAction *showevolutionAct;
    QAction *itestAct;
    QAction *iadaptAct;
    QAction *ievolveAct;
    QAction *istopAct;



};



class Window : public QWidget
{
    Q_OBJECT

public:
    Window( MainWindow* main);

private slots:

private:

};



#endif

