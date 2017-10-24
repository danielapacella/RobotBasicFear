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

#include "defs.h"
#include "public.h"

#ifdef EVOGRAPHICS  

#include <QtGui>
#include <QGridLayout>
#include "rend_evolution.h"

RendEvolution *rendEvolution;      // The evolution rendering widget

int evwidgetdx = 300;            // x-size of the widget
int evwidgetdy = 300;            // y-size of the widget

//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

/*
 * Create the rendevolution widget
 */

void
init_rendevolution()
{
  rendEvolution = new RendEvolution;
}

void
update_rendevolution(int mode)


{

     drawevolution = mode;
     rendEvolution->repaint();
     QCoreApplication::processEvents();
     drawevolution = 0;

}

void
add_evolution_widget(QGridLayout *mainLayout, int row, int column)
{
  mainLayout->addWidget(rendEvolution, row, column);
}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  -------------------------------
//----------------------------------------------------------------------


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
    return QSize(evwidgetdx, evwidgetdy);
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
   // draw statistics
   if (drawevolution > 0 && statfit_n > 0)
	{
	  offx = 40;
	  offy = 40;
	  // graph dimension 500X500 + offx & offy
	  statfit_int = statfit_max - statfit_min;
	  if (statfit_n > 1 && statfit_n < 500)
	    x_scale = (evwidgetdx - offx) / (statfit_n - 1);
	   else
	    x_scale = 1.0;
	  maxx = evwidgetdx;
	  if (statfit_n > evwidgetdx)
		  maxx = statfit_n; 

	  if (statfit_int > 0)
	    y_scale = (evwidgetdy - offy) / statfit_int;
	   else
        y_scale = 1.0;

      painter.drawLine(0 + offx, evwidgetdy + offy, maxx + offx, evwidgetdy + offy);
      painter.drawLine(0 + offx, 0 + offy, 0 + offx, evwidgetdy + offy);
      sprintf(label,"%d",statfit_n);
      painter.drawText(maxx+40,evwidgetdy+40, label);
      sprintf(label,"%.1f",statfit_min);
      painter.drawText(0, evwidgetdy+40, label);
      sprintf(label,"%.1f",statfit_max);
      painter.drawText(0, 40, label);
      if (drawevolution == 1)
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
	  if (drawevolution == 2)
	   {
	    for(n=0;n < nreplications; n++)
	    {
             sprintf(filen,"statS%d.fit",seed + n);
             loadstatistics(filen,0);
	     for(g=0;g < (statfit_n - 1); g++)
	     {
	       painter.drawLine(g * x_scale + offx, (statfit_int - statfit[g][0]) * y_scale + offy, (g + 1) * x_scale + offx, (statfit_int - statfit[g+1][0]) * y_scale + offy);
	     }
	    }
	  }   
	}   


}


#endif
