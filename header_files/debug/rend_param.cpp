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
#include "rend_param.h"

#ifdef EVOGRAPHICS  

#include <QtGui>        // Widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog

extern struct Parameter {
    char   *name;                 // the name of the parameter
    char   *folder;               // the folder of the parameter
    int     type;		  // the type, 0=int 
    int     n1;                   // the minimum value (for int only)
    int     n2;	   	          // the maximum value (for int only)
    int    *vpointer;             // the pointer to the parameter
    struct Parameter *next;       // the next parameter of the same folder
    struct Parameter *nextfolder; // the first parameter of the next folder
    char   *description;          // parameter description
  };

extern struct Parameter *parlist;

RendParam *rendParam;        // The param rendering widget
ParamDialog *paramDialog;    // The network Dialog
int wparam=0;                // whether the window has been already opened or not


//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

/*
 * mode=1, create the network widget
 * mode=2  create the network dialog and associate the widget to it
 */

void
init_rendparam()
{

    if (wparam == 0)
    {
     rendParam = new RendParam;
     paramDialog = new ParamDialog(NULL);
     paramDialog->setWindowTitle("Parameters");
     paramDialog->setWidget(rendParam, 0, 0);
     paramDialog->show();
     update_rendparam();
     wparam = 1;
    }

    if (wparam == 1)
    {
     paramDialog->show();
     update_rendparam();
    }
}

void
hide_rendparam()
{
    if (wparam == 1)
      paramDialog->hide();
}


void
update_rendparam()


{

     rendParam->repaint();
     QCoreApplication::processEvents();

}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  ------------------------------
//----------------------------------------------------------------------

// -----------------------------------------------------
// Dialog and Toolbar
// -----------------------------------------------------

void ParamDialog::createToolBars()
{
    QToolBar *paramtoolbar;

    paramtoolbar = toolBar();


}

void ParamDialog::createActions()
{


}



// -----------------------------------------------------
// Widget
// -----------------------------------------------------

RendParam::RendParam(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
}

QSize RendParam::minimumSizeHint() const
{
    return QSize(550, 500);
}

QSize RendParam::sizeHint() const
{
    return QSize(550, 500);
}

void RendParam::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendParam::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RendParam::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendParam::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendParam::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}


void RendParam::paintEvent(QPaintEvent *)
{

   char   message[64];
   struct Parameter *cfolder;
   struct Parameter *cpar;
   int    b;
   QPoint pxy;

   QPainter painter(this);
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);

   pxy.setY(20);
   pxy.setX(0);
   sprintf(message,"parametes:");
   painter.drawText(pxy, message);
   painter.setPen(Qt::black);

   b = 1;
   cfolder = parlist;
   cpar = cfolder;
   while(cpar != NULL)
	 {
           sprintf(message,"%d) folder %s name %s value %d descp %s", b, cpar->folder, cpar->name, *cpar->vpointer, cpar->description);
	   pxy.setY(b*20+40);
	   pxy.setX(0);
	   painter.drawText(pxy, message);
	   b += 1;
	   cpar = cpar->next; 
	 }
   if (cfolder->nextfolder != NULL)
	 {
	  cfolder = cfolder->nextfolder;
	  cpar = cfolder;
          while(cpar != NULL)
	   {
             sprintf(message,"%d) folder %s name %s value %d descp %s", b, cpar->folder, cpar->name, *cpar->vpointer, cpar->description);
	     pxy.setY(b*20+40);
	     pxy.setX(0);
	     painter.drawText(pxy, message);
	     b += 1;
	     cpar = cpar->next; 
	   }
          
	 }



}





// -----------------------------------------------------
// Dialog
// -----------------------------------------------------

ParamDialog::ParamDialog(QWidget *parent) :
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

ParamDialog::~ParamDialog()
{
}

void ParamDialog::setWidget(QWidget *w, int r, int c)
{
	m_layout->addWidget(w, r, c);
}

QToolBar* ParamDialog::toolBar()
{
	if (m_toolBar == NULL) {
		// Creating toolbar and adding it to the main layout
		m_toolBar = new QToolBar(this);
		m_mainLayout->addWidget(m_toolBar);
	}

	return m_toolBar;
}

void ParamDialog::closeEvent(QCloseEvent *)
{
	emit dialogClosed();
}

void ParamDialog::keyReleaseEvent(QKeyEvent* event)
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

void ParamDialog::shotWidget()
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



#endif
