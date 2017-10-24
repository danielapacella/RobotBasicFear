/* 
 * Evorobot* - rend_evolution.cpp
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

#ifdef EVOGRAPHICS  

#include <QtGui>        // Widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog
#include "rend_evolution.h"
#include "evolution.h"


RendEvolution *rendEvolution;      // The evolution rendering widget
EvolutionDialog *evolutionDialog;  // The evolution Dialog

int wevolution = 0;              // record whether the widget and/or the dialog has been created
int evwidgetdx = 400;            // x-size of the widget
int evwidgetdy = 400;            // y-size of the widget
int rendevolutionmode;           // whether we plot best or best and average performace or the best of all replications

//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

void
init_rendevolution(int mode)
{

    if (mode == 1 && wevolution == 0)
      {
       rendEvolution = new RendEvolution;
       wevolution = 1;
      }

    if (mode == 2 && wevolution == 1)
    {
     evolutionDialog = new EvolutionDialog(NULL);

     evolutionDialog->setWindowTitle("Adaptation");
#ifdef EVOLANGUAGEIT 
     evolutionDialog->setWindowTitle("Adattamento");
#endif
     evolutionDialog->setWidget(rendEvolution, 0, 0);
     if (winadaptationx >= 0 && winadaptationy >= 0)
        evolutionDialog->move(winadaptationx, winadaptationy);
     if (winadaptationdx > 0 && winadaptationdy > 0)
        evolutionDialog->resize(winadaptationdx, winadaptationdy);
     evolutionDialog->show();
     update_rendevolution(1);
     wevolution = 2;
    }
    if (mode == 2 && wevolution == 2)
    {
     evolutionDialog->show();
     update_rendevolution(1);
    }
}

void
hide_rendevolution()
{
    if (wevolution == 2)
      evolutionDialog->hide();
}

void
update_rendevolution(int mode)


{

     rendevolutionmode = mode;
     rendEvolution->repaint();
     QCoreApplication::processEvents();

}


void
add_evolution_widget(QGridLayout *mainLayout, int row, int column)
{
  mainLayout->addWidget(rendEvolution, row, column);
}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

// -----------------------------------------------------
// Dialog and Toolbar
// -----------------------------------------------------

void EvolutionDialog::createToolBars()
{
    QToolBar *evolutiontoolbar;

    evolutiontoolbar = toolBar();

    evolutiontoolbar->addAction(openAct);
    evolutiontoolbar->addAction(displaystatAct);
    evolutiontoolbar->addAction(displayallstatAct);
    evolutiontoolbar->addAction(createlineageAct);
    evolutiontoolbar->addAction(randomizeparametersAct);

}

void EvolutionDialog::createActions()
{

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open a file"), this);
    openAct->setShortcut(tr("Ctrl+N"));
    openAct->setStatusTip(tr("Open a file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    displaystatAct = new QAction(QIcon(":/images/fitness.png"), tr("&show fitness"), this);
    displaystatAct->setShortcut(tr("Ctrl+F"));
    displaystatAct->setStatusTip(tr("show fitness statistics"));
    connect(displaystatAct, SIGNAL(triggered()), this, SLOT(displaystat()));

    displayallstatAct = new QAction(QIcon(":/images/fitnessall.png"), tr("&show all fitness"), this);
    displayallstatAct->setShortcut(tr("Ctrl+F"));
    displayallstatAct->setStatusTip(tr("show fitness statistics for all replications"));
    connect(displayallstatAct, SIGNAL(triggered()), this, SLOT(displayallstat()));

    createlineageAct = new QAction(QIcon(":/images/lineage.png"), tr("&create lineage"), this);
    createlineageAct->setShortcut(tr("Ctrl+L"));
    createlineageAct->setStatusTip(tr("create the lineage file"));
    connect(createlineageAct, SIGNAL(triggered()), this, SLOT(createlineage()));

    randomizeparametersAct = new QAction(QIcon(":/images/light.png"), tr("&randomize free parameters"), this);
    randomizeparametersAct->setShortcut(tr("Ctrl+L"));
    randomizeparametersAct->setStatusTip(tr("randomize free parameters"));
    connect(randomizeparametersAct, SIGNAL(triggered()), this, SLOT(randomizeparameters()));


}

/*
 * open a .gen .fit or .phe file
 */
void EvolutionDialog::open()
{

    char *f;
    QByteArray filen;
    char filename[256];


        QString fileName = QFileDialog::getOpenFileName(this,
            "Choose a filename to save under",
                    "",
                    "*.gen *.fit world?.env *.phe");


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

}

/*
 * display the fitness statistics of all replications
 */
void EvolutionDialog::displaystat()
{
       update_rendevolution(1);
}

/*
 * display the fitness statistics of all replications
 */
void EvolutionDialog::displayallstat()
{
  display_all_stat();
}


/*
 * create the lineage file
 */
void EvolutionDialog::createlineage()
{
 create_lineage();
}


/*
 * randomize free parameters
 */
void EvolutionDialog::randomizeparameters()
{

 int team;

 randomize_pop();
 for(team=0; team<nteam; team++)
   getgenome(0,team);
}

// -----------------------------------------------------
// Widget
// -----------------------------------------------------


RendEvolution::RendEvolution(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
}

QSize RendEvolution::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize RendEvolution::sizeHint() const
{
    return QSize(evwidgetdx, evwidgetdy);
}

void RendEvolution::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendEvolution::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RendEvolution::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendEvolution::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendEvolution::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}


/*
 * we update the widget when the user click on the window

void 
RendEvolution::mousePressEvent(QMouseEvent *event)
{
    
  update_rendevolution(1);

} */

void RendEvolution::paintEvent(QPaintEvent *)
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
   int    g,n;
   int    offx,offy;
   int    maxx;
   float  x_scale, y_scale;
   float  scalex, scaley;
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
   dx = evwidgetdx - 60;
   dy = evwidgetdy - 60;

   // draw statistics
   if (statfit_n > 0)
    {

          scaley = height() / (float) 500.0;
          scalex = width() / (float) 500.0;
          painter.scale(scalex, scaley);
 
      offx = 40;
      offy = 40;

      // graph dimension 500X500 + offx & offy
      statfit_int = statfit_max - statfit_min;
      if (statfit_n > 1)
        x_scale = (dx - offx) / (float) (statfit_n - 1);

      y_scale = 1.0;
      maxx = dx;
      if (statfit_n > dx)
          maxx = statfit_n; 

      if (statfit_int > 0)
         y_scale = (dy - offy) / statfit_int;
        else
             y_scale = 1.0;

      painter.drawLine(0 + offx, dy + offy, maxx + offx, dy + offy);
      painter.drawLine(0 + offx, 0 + offy, 0 + offx, dy + offy);
      painter.drawLine(offx - 5, ((dy /4) * 1) + offy, 5 + offx, ((dy /4) * 1) + offy);
      painter.drawLine(offx - 5, ((dy /4) * 2) + offy, 5 + offx, ((dy /4) * 2) + offy);
      painter.drawLine(offx - 5, ((dy /4) * 3) + offy, 5 + offx, ((dy /4) * 3) + offy);
      painter.drawLine(offx - 5, ((dy /4) * 4) + offy, 5 + offx, ((dy /4) * 4) + offy);
      sprintf(label,"%d",statfit_n);
      painter.drawText(maxx+40,dy+40, label);
      sprintf(label,"%.1f",statfit_min);
      painter.drawText(0, dy+40, label);
      sprintf(label,"%.1f",statfit_max);
      painter.drawText(0, 40, label);
      if (rendevolutionmode == 1)
       {
        if (statfit_n == 1)
               painter.drawEllipse(0 * x_scale + offx - 5, (statfit_int - statfit[0][1]) * y_scale + offy - 5, 10, 10);

        for(n=0;n < 2; n++)
        {
         for(g=0;g < (statfit_n - 1); g++)
         {
           painter.drawLine(g * x_scale + offx, (statfit_int - statfit[g][n]) * y_scale + offy, (g + 1) * x_scale + offx, (statfit_int - statfit[g+1][n]) * y_scale + offy);
         }
        }
      }  
      if (rendevolutionmode == 2)
       {
        for(n=0;n < nreplications; n++)
        {
             sprintf(filen,"statS%d.fit",seed + n);
             loadstatistics(filen,0);
             switch(n) 
              {
               case 0:
                painter.setPen(QColor(0,0,0,255)); // black
                break;
               case 1:
                painter.setPen(QColor(255,0,0,255)); // red
                break;
               case 2:
                painter.setPen(QColor(0,255,0,255)); // green
                break;
               case 3:
                painter.setPen(QColor(0,0,255,255)); // blue
                break;
               case 4:
                painter.setPen(QColor(255,255,0,255)); // yellow
                break;
               case 5:
                painter.setPen(QColor(255,0,255,255)); // magenta
                break;
               case 6:
                painter.setPen(QColor(0,255,255,255)); // cyan
                break;
               case 7:
                painter.setPen(QColor(255,140,0,255)); // orange
                break;
               case 8:
                painter.setPen(QColor(255,0,255,255)); // magenta
                break;
               case 9:
                painter.setPen(QColor(153,50,204,255)); // brown
                break;
               default:
                painter.setPen(QColor(125,125,125,255)); // gray
                break;
              }
         for(g=0;g < (statfit_n - 1); g++)
         {
           painter.drawLine(g * x_scale + offx, (statfit_int - statfit[g][0]) * y_scale + offy, (g + 1) * x_scale + offx, (statfit_int - statfit[g+1][0]) * y_scale + offy);
         }
         sprintf(label,"%d",n + seed);
         painter.drawText(offx + (n * 20),dy+60, label);
        }
      }   
    }   


}

// -----------------------------------------------------
// Dialog
// -----------------------------------------------------

EvolutionDialog::EvolutionDialog(QWidget *parent) :
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

EvolutionDialog::~EvolutionDialog()
{
}

void EvolutionDialog::setWidget(QWidget *w, int r, int c)
{
    m_layout->addWidget(w, r, c);
}

QToolBar* EvolutionDialog::toolBar()
{
    if (m_toolBar == NULL) {
        // Creating toolbar and adding it to the main layout
        m_toolBar = new QToolBar(this);
        m_mainLayout->addWidget(m_toolBar, 0);
    }

    return m_toolBar;
}

void EvolutionDialog::closeEvent(QCloseEvent *)
{
    emit dialogClosed();
}

void EvolutionDialog::keyReleaseEvent(QKeyEvent* event)
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

void EvolutionDialog::shotWidget()
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
