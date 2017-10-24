/* 
 * Evorobot* - rend_robot.h
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

#ifdef EVOGRAPHICS  

#ifndef RENDROBOT_H
#define RENDROBOT_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

#include <QDialog>    // for RobotDialog
#include <QGridLayout>
#include <QToolBar>
#include <QBoxLayout>


/*
 * The robot widget
 */
class RendRobot : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundRect, Ellipse, Arc,
                 Chord, Pie, Path, Text, Pixmap };

    RendRobot(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void draw_neurons_update();
    int ndu;


public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
};

/*
 * The dialog to show the widget within a window
 */
class RobotDialog : public QDialog
{
	Q_OBJECT

public:
	// Constructor
	// takes as parameter the parent widget
	RobotDialog(QWidget *parent = NULL);

	// destructor
	virtual ~RobotDialog();

	// brief Sets the widget to show
	// param w the widget to show
	// param r the row in which the widget is to be shown
	// param c the column in which the widget is to be shown
	void setWidget(QWidget *w, int r, int c);

	// Returns the tool bar for this dialog
	// The first call to this function creates the toolbar
	// return The toolbar of this dialog
	QToolBar* toolBar();

signals:
	// Emitted when the window is closed
	void dialogClosed();

private:
	 // Called when the window is closed
	 // param event description of the event
	void closeEvent(QCloseEvent *event);

	/**
	 * \brief Called when a key is pressed
	 *
	 * \param event description of the event
	 */
	void keyReleaseEvent(QKeyEvent* event);

	/**
	 * \brief Renders the inner widget inside a pixmap and then asks the
	 *        user a filename to save it
	 */
	void shotWidget();

	/**
	 * \brief The widget shown by this dialog
	 */
	QWidget *m_widget;

	/**
	 * \brief The main layout of the dialog. Contains the toolbar (if
	 *        present) and m_layout
	 */
	QBoxLayout *m_mainLayout;

	/**
	 * \brief The layout for this dialog (the one managing child widgets
	 *        added with the setWidget function)
	 */
	QGridLayout *m_layout;

	/**
	 * \brief The toolbar
	 *
	 * This is NULL until the first call to toolBar()
	 */
	QToolBar *m_toolBar;

	// create the toolbar
	void createToolBars();
	void createActions();

	QAction *load_envAct;
	QAction *save_envAct;
	QAction *rotate_robotAct;
	QAction *speed_downAct;
	QAction *speed_upAct;
	QAction *add_wallAct;
	QAction *add_roundAct;
	QAction *add_sroundAct;
	QAction *add_lightAct;
	QAction *add_tareaAct;
	QAction *delete_objAct;
	QAction *conrobotsAct;

	private slots:

        void load_env();
        void save_env();
        void rotate_robot();
        void speed_down();
        void speed_up();
        void add_wall();
        void add_round();
        void add_sround();
        void add_light();
        void add_tarea();
        void delete_obj();
        void conrobots();

};



#endif

#endif
