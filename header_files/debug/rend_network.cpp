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
#include "rend_network.h"

#ifdef EVOGRAPHICS  

#include <QtGui>        // Widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog

RendNetwork *rendNetwork;      // The network rendering widget
NetworkDialog *networkDialog;  // The network Dialog

int drawblocks=0;              // whether we display or not connections and update blocks 
int wnetwork=0;                // whether the window has been already opened or not
float   ngraph[1000][40];      // value to be displayed for neurons
int     ngraphid[40];          // id of the neuron to be displayed
char    neuronl[40][8];        // neurons labels
int     cneuron[4];            // selected neurons
int     cneuronn=0;            // n. of selected cneurons
int     neurondmode=0;         // display 1=delta, 2=weight, 3=biases instead of labels

extern int ninputs;                // n. input units                          
extern int nhiddens;               // n. hidden units                    
extern int noutputs;               // n. output units
extern int nneurons;               // total n. of neurons
extern int net_nblocks;            // network number of connection blocks
extern int net_block[40][5];       // blocks: 0=type (w,b,d), from 1 per 2 receive from 3 per 4
extern int neuronbias[40];         // whether neurons have bias
extern int neurondelta[40];        // whether neurons have time constants
extern int neuronxy[40][2];        // neurons xy position in display space
extern int neurondisplay[40];      // whether neurons should be displayed or not
extern int neuronlesion[40];       // whether neurons are lesioned 
extern int neuronlesions;          // whether we have at least one neuron leasioned
//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

/*
 * mode=1, create the network widget
 * mode=2  create the network dialog and associate the widget to it
 */

void
init_rendnetwork(int mode)
{

    if (mode == 1 && wnetwork == 0)
      {
       rendNetwork = new RendNetwork;
       wnetwork = 1;
      }

    if (mode == 2 && wnetwork == 1)
    {
     networkDialog = new NetworkDialog(NULL);
     networkDialog->setWindowTitle("Network");
     networkDialog->setWidget(rendNetwork, 0, 0);
     networkDialog->show();
     update_rendnetwork();
     wnetwork = 2;
    }
    if (mode == 2 && wnetwork == 2)
    {
     networkDialog->show();
     update_rendnetwork();
    }
}

void
hide_rendnetwork()
{
    if (wnetwork == 2)
      networkDialog->hide();
}

void
add_network_widget(QGridLayout *mainLayout, int row, int column)
{
  mainLayout->addWidget(rendNetwork, row, column);
}


void
update_rendnetwork()


{

     drawnetwork = 1;
     rendNetwork->repaint();
     QCoreApplication::processEvents();
     drawnetwork = 0;

}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  ------------------------------
//----------------------------------------------------------------------

// -----------------------------------------------------
// Dialog and Toolbar
// -----------------------------------------------------

void NetworkDialog::createToolBars()
{
    QToolBar *networktoolbar;

    networktoolbar = toolBar();

    networktoolbar->addAction(set_neurondisplayAct);
    networktoolbar->addAction(set_neurondeltaAct);
    networktoolbar->addAction(set_neuronbiasAct);
    networktoolbar->addAction(set_lesionAct);
    networktoolbar->addAction(display_weightAct);
    networktoolbar->addAction(display_deltaAct);
    networktoolbar->addAction(display_biasAct);
    networktoolbar->addAction(display_blocksAct);
    networktoolbar->addAction(erase_netAct);
    networktoolbar->addAction(add_blockAct);
    networktoolbar->addAction(save_netAct);


}

void NetworkDialog::createActions()
{

    set_neurondisplayAct = new QAction(QIcon(":/images/show.png"), tr("&Set/Unset neurons to be displayed as graph"), this);
    set_neurondisplayAct->setShortcut(tr("Ctrl+N"));
    set_neurondisplayAct->setStatusTip(tr("Select/Deselect neurons to be displayed as graph"));
    connect(set_neurondisplayAct, SIGNAL(triggered()), this, SLOT(set_neurondisplay()));

    set_neurondeltaAct = new QAction(QIcon(":/images/delta.png"), tr("&Add/Remove neurons deltas"), this);
    set_neurondeltaAct->setShortcut(tr("Ctrl+W"));
    set_neurondeltaAct->setStatusTip(tr("Add/Remove neurons delta"));
    connect(set_neurondeltaAct, SIGNAL(triggered()), this, SLOT(set_neurondelta()));

    set_neuronbiasAct = new QAction(QIcon(":/images/bias.png"), tr("&Add/Remove neurons biases"), this);
    set_neuronbiasAct->setShortcut(tr("Ctrl+B"));
    set_neuronbiasAct->setStatusTip(tr("Add/Remove neurons bias"));
    connect(set_neuronbiasAct, SIGNAL(triggered()), this, SLOT(set_neuronbias()));

    set_lesionAct = new QAction(QIcon(":/images/lesion.png"), tr("&Lesion/Restore neurons"), this);
    set_lesionAct->setShortcut(tr("Ctrl+B"));
    set_lesionAct->setStatusTip(tr("Lesion/Restore neurons"));
    connect(set_lesionAct, SIGNAL(triggered()), this, SLOT(set_lesion()));

    display_weightAct = new QAction(QIcon(":/images/weight.png"), tr("&Display/Undisplay weights"), this);
    display_weightAct->setShortcut(tr("Ctrl+W"));
    display_weightAct->setStatusTip(tr("Display the incoming and outcoming weight of a neuron"));
    connect(display_weightAct, SIGNAL(triggered()), this, SLOT(display_weight()));

    display_deltaAct = new QAction(QIcon(":/images/ddelta.png"), tr("&Display/Undisplay neurons delta"), this);
    display_deltaAct->setShortcut(tr("Ctrl+Y"));
    display_deltaAct->setStatusTip(tr("Display neurons delta"));
    connect(display_deltaAct, SIGNAL(triggered()), this, SLOT(display_delta()));

    display_biasAct = new QAction(QIcon(":/images/dbias.png"), tr("&Display/Undisplay biases"), this);
    display_biasAct->setShortcut(tr("Ctrl+Z"));
    display_biasAct->setStatusTip(tr("Display neurons biases"));
    connect(display_biasAct, SIGNAL(triggered()), this, SLOT(display_bias()));

    display_blocksAct = new QAction(QIcon(":/images/drawblocks.png"), tr("&Display/Undisplay blocks"), this);
    display_blocksAct->setShortcut(tr("Ctrl+B"));
    display_blocksAct->setStatusTip(tr("Display blocks description"));
    connect(display_blocksAct, SIGNAL(triggered()), this, SLOT(display_blocks()));

    erase_netAct = new QAction(QIcon(":/images/erasenet.png"), tr("&Erase net"), this);
    erase_netAct->setShortcut(tr("Ctrl+E"));
    erase_netAct->setStatusTip(tr("Erase all connections and update blocks"));
    connect(erase_netAct, SIGNAL(triggered()), this, SLOT(erase_net()));

    add_blockAct = new QAction(QIcon(":/images/addblock.png"), tr("&Add Block"), this);
    add_blockAct->setShortcut(tr("Ctrl+A"));
    add_blockAct->setStatusTip(tr("add a connection or update block"));
    connect(add_blockAct, SIGNAL(triggered()), this, SLOT(add_block()));

    save_netAct = new QAction(QIcon(":/images/savenet.png"), tr("&Save net"), this);
    save_netAct->setShortcut(tr("Ctrl+W"));
    save_netAct->setStatusTip(tr("Save the connections and update blocks in the evorobot.net file"));
    connect(save_netAct, SIGNAL(triggered()), this, SLOT(save_net()));
}


// define or undefine neurons to be displayed
void NetworkDialog::set_neurondisplay()
{
   int i;

   if (cneuronn == 1)
   {
     if (neurondisplay[cneuron[0]] == 1)
       neurondisplay[cneuron[0]] = 0;
      else
       neurondisplay[cneuron[0]] = 1;
   }
   if (cneuronn == 2)
   {
     if (neurondisplay[cneuron[0]] == 1)
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurondisplay[i] = 0;
     else
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurondisplay[i] = 1;
   }
   if (cneuronn < 1 || cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons first");

   cneuronn = 0;
   update_rendnetwork();

}

// define or undefine neurons delta
void NetworkDialog::set_neurondelta()
{
   int i;

   if (cneuronn == 1)
   {
     if (neurondelta[cneuron[0]] == 1)
       neurondelta[cneuron[0]] = 0;
      else
       neurondelta[cneuron[0]] = 1;
   }
   if (cneuronn == 2)
   {
     if (neurondelta[cneuron[0]] == 1)
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurondelta[i] = 0;
     else
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurondelta[i] = 1;
   }
   if (cneuronn < 1 || cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons first");

    cneuronn = 0;
    update_rendnetwork();

}

// define or undefine neurons bias
void NetworkDialog::set_neuronbias()
{
   int i;

   if (cneuronn == 1)
   {
     if (neuronbias[cneuron[0]] == 1)
       neuronbias[cneuron[0]] = 0;
      else
       neuronbias[cneuron[0]] = 1;
   }
   if (cneuronn == 2)
   {
     if (neuronbias[cneuron[0]] == 1)
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neuronbias[i] = 0;
     else
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neuronbias[i] = 1;
   }
   if (cneuronn < 1 || cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons first");

   cneuronn = 0;
   update_rendnetwork();
}

// Lesion/Restore neurons
void NetworkDialog::set_lesion()
{
   int i;

   // Restore the functionality of all neurons
   if (cneuronn == 0)
   {
     for(i=0; i < ind->nneurons; i++)
	neuronlesion[i] = 0;
     neuronlesions = 0;
     create_neurons_labels();
   }
   if (cneuronn == 1 || cneuronn == 2)
   {
     if (neuronlesions == 0)
      for(i=0; i < ind->nneurons; i++)
	neuronlesion[i] = 0;
      neuronlesions = 1;
      if (cneuronn == 1)
	{
          sprintf(neuronl[cneuron[0]],"XX");
	  neuronlesion[cneuron[0]] = 1;
	}
      if (cneuronn == 2)
       for(i=cneuron[0]; i <= cneuron[1]; i++)
	{
          sprintf(neuronl[i],"xx");
	  neuronlesion[i] = 1;
	}
      cneuronn = 0;
   }
   if (cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons");

   cneuronn = 0;
   update_rendnetwork();
}




// Display/Undisplay neurons delta
void NetworkDialog::display_delta()
{
   struct  individual *pind;

   if (neurondmode > 0)
   {
     create_neurons_labels();
     neurondmode = 0;
   }
   else
   {
     neurondmode = 1;
     pind = (ind + cindividual);
     pseudo_activate_net(pind->freep, pind->nneurons,1,0,0);
   }

   update_rendnetwork();
}


// Display/Undisplay the weights of a neuron
void NetworkDialog::display_weight()
{
   struct  individual *pind;

   if (neurondmode > 0)
   {
     create_neurons_labels();
     neurondmode = 0;
   }
   else
   {
     if (cneuronn == 1)
     {
      neurondmode = 2;
      pind = (ind + cindividual);
      pseudo_activate_net(pind->freep, pind->nneurons,0,1,0);
     }
     else
     {
      if (cneuronn != 1)
        display_warning("you should select one first");
     }
   }
   update_rendnetwork();
}

// Display/Undisplay neurons biases
void NetworkDialog::display_bias()
{
   struct  individual *pind;

   if (neurondmode > 0)
   {
     create_neurons_labels();
     neurondmode = 0;
   }
   else
   {
     neurondmode = 3;
     pind = (ind + cindividual);
     pseudo_activate_net(pind->freep, pind->nneurons,0,0,1);
   }

   update_rendnetwork();
}

// Display/Undisplay blocks description
void NetworkDialog::display_blocks()
{

  if (drawblocks == 0)
    drawblocks = 1;
  else
    drawblocks = 0;
  update_rendnetwork();

}

// Erase all connections and update blocks
void NetworkDialog::erase_net()
{

   int i;

   net_nblocks = 0;
   for(i=0;i < ind->nneurons; i++)
	   {
             neuronbias[i] = 0;
             neurondelta[i] = 0;
	     neurondisplay[i] = 1;
	   }
   update_rendnetwork();
}

// Add a connection or update block
void NetworkDialog::add_block()
{

  int i;
  char message[64];

  i = net_nblocks;

  if (cneuronn != 2 && cneuronn != 4)
    {
      sprintf(message, "this command require to select 2 or 4 neurons");
      display_error(message);
    }

  if (cneuronn == 2)
    {
	net_block[i][0] = 1;
        net_block[i][1] = cneuron[0];
	net_block[i][2] = cneuron[1] - cneuron[0] + 1;
	net_block[i][3] = 0;
	net_block[i][4] = 0;
	net_nblocks++;
	cneuronn = 0;
    }
  if (cneuronn == 4)
    {
	net_block[i][0] = 0;
	net_block[i][1] = cneuron[0];
        net_block[i][2] = cneuron[1] - cneuron[0] + 1;
	net_block[i][3] = cneuron[2];
        net_block[i][4] = cneuron[3] - cneuron[2] + 1;
	net_nblocks++;
	cneuronn = 0;
    }

   update_rendnetwork();
}


// save current connections and update blocks in an evorobot.net file
void NetworkDialog::save_net()
{
    save_net_blocks("evorobot.net");

}


// -----------------------------------------------------
// Widget
// -----------------------------------------------------

RendNetwork::RendNetwork(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
}

QSize RendNetwork::minimumSizeHint() const
{
    return QSize(550, 500);
}

QSize RendNetwork::sizeHint() const
{
    return QSize(550, 500);
}

void RendNetwork::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendNetwork::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RendNetwork::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendNetwork::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendNetwork::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}


void RendNetwork::paintEvent(QPaintEvent *)
{
 

   int    i;
   int    u;
   int    c;
   int    t,b;
   int    counter;
   int    n_start, n_max;
   int    h1,h2;
   int    sx,sy,dx,dy;
   int    offx,offy;
   char   message[64];
   
   QRect labelxy(0,0,30,20);          // neuron labels 
   QPoint pxy;

   QPainter painter(this);
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);

   if (drawnetwork == 1)
   {
   // draw neurons state
   if (testindividual_is_running == 1 && drawneuronsdx >= 50)
   {
     draw_neurons_update();
     offx = 50; //draw_window1_width + 50;
     if (drawn_alltogether == 1)
       h2 = height() / (ndu / nteam);
      else
       h2 = height() / ndu;
     h1 = h2 - 3;
     n_start = 0;
     n_max   = run;
     if (n_max > drawneuronsdx)
      n_max = drawneuronsdx;
     if (run > drawneuronsdx)
       n_start = (run-drawneuronsdx) % drawneuronsdx; 
      else
       n_start = 0; 
     c = n_start;
     counter = 0;
     while(counter < n_max)
       {
         offy = h2;
         for(u=0;u < ndu; u++)
	 {

	  if (counter == 0)
	  {
            painter.setPen(Qt::black);
            labelxy.setRect(offx-30, offy-(h2/2), 30, 30);
	    painter.drawText(labelxy, neuronl[ngraphid[u]]);
            painter.drawLine(offx - 5, offy - h1, offx - 5, offy);
	  }

	  if (drawn_alltogether == 0)
	  {
	  if (ngraphid[u] < ind->ninputs)
            painter.setPen(QColor(255,0,0,255));
	  if (ngraphid[u] >= ind->ninputs && ngraphid[u] < (ind->nneurons - ind->noutputs))
            painter.setPen(QColor(180,180,180,255));
	  if (ngraphid[u] >= (ind->nneurons - ind->noutputs))
            painter.setPen(QColor(0,0,255,255));
	  if (ngraphid[u] == ind->nneurons)
            painter.setPen(QColor(0,255,0,255));
	  }
	  else
	  {
	  if (((u + 1) / (ndu / nteam)) == 0)
            painter.setPen(QColor(255,0,0,255));
	  if (((u + 1) / (ndu / nteam)) == 1)
	    painter.setPen(QColor(0,0,255,255));
	  if (((u + 1) / (ndu / nteam)) == 2)
            painter.setPen(QColor(0,255,0,255));
	  if (((u + 1) / (ndu / nteam)) == 3)
            painter.setPen(QColor(160,0,160,255));
	  }

	  if (drawnstyle == 0 && drawn_alltogether == 0)
            painter.drawLine(counter+offx, offy - (ngraph[c][u]*h1), counter+offx, offy);
	   else
	   if (c > 0)
             painter.drawLine(counter+offx-1, offy - (ngraph[c-1][u]*h1), counter+offx, offy - (ngraph[c][u]*h1));
	  offy += h2;
	  if (drawn_alltogether == 1 && ((u + 1) % (ndu / nteam)) == 0)
	    offy = h2;
	 }
	c++;
	if (c == drawneuronsdx)
	 c = 0;
	counter++;
       }
   }

   // draw neural architecture

   if (testindividual_is_running == 0)
   {
    painter.setPen(Qt::gray);
    for (i=0; i < net_nblocks; i++)
    {
      if (net_block[i][0] == 0)
        for (t=net_block[i][1]; t < (net_block[i][1] + net_block[i][2]); t++)
          for (b=net_block[i][3]; b < (net_block[i][3] + net_block[i][4]); b++)
	  {
            if (abs(neuronxy[b][1] - neuronxy[t][1]) > 20) 
              painter.drawLine(neuronxy[t][0],neuronxy[t][1],neuronxy[b][0], neuronxy[b][1]);
	     else
	     {
	      dx = abs(neuronxy[t][0] - neuronxy[b][0]);
	      dy = abs(neuronxy[t][1] - neuronxy[b][1]);
              if (neuronxy[t][0] < neuronxy[b][0])
		sx = neuronxy[t][0];
	       else
		sx = neuronxy[b][0];
              if (neuronxy[t][1] < neuronxy[b][1])
		sy = neuronxy[t][1];
	       else
		sy = neuronxy[b][1];
	      painter.drawArc(sx,sy-20,dx,dy+40, 0 * 16, 180 * 16); 
	     }
	  }
    }
    for (i=0; i < ind->nneurons; i++)
	  {
           painter.setBrush(Qt::SolidPattern);

	   if (neuronbias[i] == 1)
	      painter.setPen(Qt::black);
	     else
	      painter.setPen(Qt::red);

	   if (neurondelta[i] == 1)
              painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
	     else
              painter.setBrush(QBrush(Qt::red, Qt::Dense5Pattern));

	   painter.drawEllipse(neuronxy[i][0] - 5, neuronxy[i][1] - 5, 10, 10);

	  }
    painter.setPen(Qt::black);
    for (i=0; i < ind->nneurons; i++)
	  {
	    if (neurondisplay[i] == 1)
	      painter.setPen(Qt::black);
	     else
	      painter.setPen(Qt::red);
	    if (i < ind->ninputs)
              labelxy.setRect(neuronxy[i][0] - 5, neuronxy[i][1] + 5, 30, 30);
	     else
              labelxy.setRect(neuronxy[i][0] - 5, neuronxy[i][1] - 18, 30, 30);
	    painter.drawText(labelxy, neuronl[i]);
	  }

    painter.setBrush(Qt::black);
    if (cneuronn == 1)
          painter.drawEllipse(neuronxy[cneuron[0]][0] - 5, neuronxy[cneuron[0]][1] - 5, 10, 10);
    if (cneuronn >= 2 && cneuron[0] <= cneuron[1])
      for (i=cneuron[0]; i <= cneuron[1]; i++)
          painter.drawEllipse(neuronxy[i][0] - 5, neuronxy[i][1] - 5, 10, 10);
    painter.setBrush(Qt::blue);
    if (cneuronn == 3)
          painter.drawEllipse(neuronxy[cneuron[2]][0] - 5, neuronxy[cneuron[2]][1] - 5, 10, 10);
    if (cneuronn == 4 && cneuron[2] <= cneuron[3])
      for (i=cneuron[2]; i <= cneuron[3]; i++)
          painter.drawEllipse(neuronxy[i][0] - 5, neuronxy[i][1] - 5, 10, 10);

   // display connection and update blocks
   if (drawblocks > 0)
   {
     pxy.setY(20);
     pxy.setX(0);
     sprintf(message,"%d neurons (%d %d %d), blocks:", nneurons, ninputs, nhiddens, noutputs);
     painter.drawText(pxy, message);
     painter.setPen(Qt::black);
     for (b=0; b < net_nblocks; b++)
	 {
	   if (net_block[b][0] == 0)
             sprintf(message,"%d) %d %d %d %d",
	       b, net_block[b][1], net_block[b][2],net_block[b][3],net_block[b][4]);
           else
             sprintf(message,"%d) %d %d",
	       b, net_block[b][1], net_block[b][2]);
	   pxy.setY(b*20+40);
	   pxy.setX(0);
	   painter.drawText(pxy, message);
	 }


     }

   }
   }

}

/*
 * compute and return the number of neurons to be drawn
 */ 
void
RendNetwork::draw_neurons_update()
{

        struct  iparameters *pipar;
	int	team;
	struct  individual *pind;
	int     u;
	int     nlabel;
	
	ndu = 0;
	nlabel = 0;
	pipar = ipar;
	// update the matrix of value with the current value
	// and compute the number of neurons to be drawn
	for (team=0,pind=ind; team<nteam; team++, pind++)
	{
	  if (team == cindividual || drawn_alltogether == 1)
	  {
	    for(u=(pind->nneurons - pind->noutputs); u < pind->nneurons; u++)
		  if (neurondisplay[u] == 1 && ndu < 40)
		  {	 
                    ngraph[run%drawneuronsdx][ndu] = pind->activation[u];
		    ngraphid[ndu] = u;
		    ndu++;
		  }
	    for(u=pind->ninputs; u < (pind->nneurons - pind->noutputs); u++)
		  if (neurondisplay[u] == 1 && ndu < 40)
		  {	 
                    ngraph[run%drawneuronsdx][ndu] = pind->activation[u];
		    ngraphid[ndu] = u;
		    ndu++;
		  }
	    for(u=0; u < pind->ninputs; u++)
		  if (neurondisplay[u] == 1 && ndu < 40)
		  {	 
                    ngraph[run%drawneuronsdx][ndu] = pind->activation[u];
		    ngraphid[ndu] = u;
		    ndu++;
		  }
	  }
	}
}




/*
 * handle mouse buttons
 */
void 
RendNetwork::mousePressEvent(QMouseEvent *event)
{
		
	int x,y,b;
	int olddrawi;
	int i;
	int mode;
	
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
	  mousex = x;
	  mousey = y;

	  //   on a neuron: select the neuron (eventually as part of a block)
	  if (mode == 0)
	   for(i=0; i < ind->nneurons; i++)
	   {
	    if (mdist((float) x,(float) y, neuronxy[i][0], neuronxy[i][1]) < 10.0)
	    {
	      if (cneuronn < 4)
	      {
	       cneuron[cneuronn] = i;
	       cneuronn++;
	      }
	      else
	      {
	       cneuronn = 0;
	      }
	      mode = 2;
	    }
	   }
	  
	  // otherwise remove selected neurons
	  if (mode == 0)
	    cneuronn = 0;
	  olddrawi = drawindividual; 
	  drawindividual = 1;
	  if (testindividual_is_running == 0)
	     drawnetwork = 1;
	  repaint();
	  drawindividual = olddrawi;
	  drawnetwork = 0;
	 }
	}


}


/*
 * handle mouse move events
 */
void 
RendNetwork::mouseMoveEvent(QMouseEvent *event)
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

	// move current neuron
        cneuronn = 0;
	if (mode == 0)
         {
	 for(i=0, cn=-1; i < ind->nneurons; i++)
	   if (mdist((float) mousex,(float) mousey, neuronxy[i][0], neuronxy[i][1]) < 10.0)
	     cn = i;
	 if (cn >= 0)
	      {
	        neuronxy[cn][0] = x;
	        neuronxy[cn][1] = y;
	        drawindividual = 1;
	        drawnetwork = 1;
	        repaint();
	        drawindividual = 0;
	        drawnetwork = 0;
		mousex = x;
		mousey = y;
		mode = 1;
		sprintf(drawdata_st,"neuron #%d moved to %d %d", cn, neuronxy[cn][0], neuronxy[cn][1]);
		display_stat_message(drawdata_st);
	      }
	 	 }

	}


}


/*
 * handle mouse release events by constraints movements to the grid (if any)
 */
void 
RendNetwork::mouseReleaseEvent(QMouseEvent *event)
{

	int i;
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

	// move current neuron
	
	if (mode == 0)
	 for(i=0, cn=-1; i < ind->nneurons; i++)
	   if (mdist((float) mousex,(float) mousey, neuronxy[i][0], neuronxy[i][1]) < 10.0)
	     cn = i;
	 if (cn >= 0)
	      {
		oldp1 = neuronxy[cn][0];
		oldp2 = neuronxy[cn][1];
		neuronxy[cn][0] = (float) (((int) neuronxy[cn][0]) / grid * grid);
		neuronxy[cn][1] = (float) (((int) neuronxy[cn][1]) / grid * grid);
		if ((oldp1 - neuronxy[cn][0]) > (grid / 2))
		  neuronxy[cn][0] += grid;
		if ((oldp2 - neuronxy[cn][1]) > (grid / 2))
		  neuronxy[cn][1] += grid;
	        drawindividual = 1;
	        drawnetwork = 1;
	        repaint();
	        drawindividual = 0;
	        drawnetwork = 0;
		sprintf(drawdata_st,"neuron #%d released to %d %d", cn, neuronxy[cn][0], neuronxy[cn][1]);
		display_stat_message(drawdata_st);
		mode = 1;
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
	        if (testindividual_is_running == 0)
	          drawnetwork = 1;
		sprintf(drawdata_st,"object %s #%d released to %.1f %.1f", nobject, idobject, *o, *(o + 1));
		display_stat_message(drawdata_st);
	        repaint();
	        drawindividual = 0;
	        drawnetwork = 0;
		}

	}


}

/*
 * handle mouse buttons
 */
void 
RendNetwork::mouseDoubleClickEvent(QMouseEvent *event)
{
	
}




/*
 * create the labels of the neurons
 * labels for sensory neurons should be created with the same order 
 *   with which they are updated in set_input()
 */
void
create_neurons_labels()

{

        struct  iparameters *pipar;
	struct individual *pind;
	int     i,ii;
	int     nlabel;
	
	nlabel = 0;
	pipar = ipar;	
	pind = ind;

        // sensors
	if (pipar->motmemory     == 1)	   
	   for (i=0; i < 2; i++, nlabel++)
	       sprintf(neuronl[nlabel],"M%d",i);
	for (i=0; i < pipar->nifsensors; i++, nlabel++)
	   sprintf(neuronl[nlabel],"I%d",i); 
	for (i=0; i < pipar->lightsensors; i++, nlabel++)
	   sprintf(neuronl[nlabel],"L%d",i);		   
	for (i=0; i < (4 * ninput_other); i++, nlabel++)
	   sprintf(neuronl[nlabel],"C%d",i);
	for (i=0; i < (pipar->signalss * pipar->signalso); i++)
	   {
	   sprintf(neuronl[nlabel],"s%d",i);
	   nlabel++;
	   for (ii=0; ii < pipar->signalssf; ii++)
	    {
	     sprintf(neuronl[nlabel],"s%d",i);
	     nlabel++;
	    }
	   }
	for (i=0; i < pipar->signalso; i++, nlabel++)
	   sprintf(neuronl[nlabel],"S%d",i);	
	for (i=0; i < pipar->cameraunits; i++, nlabel++)
	   sprintf(neuronl[nlabel],"V%d",i); 
	if (pipar->elman_net     == 1)  
	   for (i=0; i < pipar->nhiddens; i++, nlabel++)
	       sprintf(neuronl[nlabel],"E%d",i);
	if (pipar->simplevision   == 1)
		   {	   
	             sprintf(neuronl[nlabel],"V1");
	             sprintf(neuronl[nlabel+1],"V2");
	             sprintf(neuronl[nlabel+2],"V3");
	             sprintf(neuronl[nlabel+3],"V4");
	             sprintf(neuronl[nlabel+4],"V5");
		     nlabel += 5;
		   }
	if (pipar->simplevision   == 2)
		   {	   
	             sprintf(neuronl[nlabel],"V1");
	             sprintf(neuronl[nlabel+1],"V2");
	             sprintf(neuronl[nlabel+2],"V3");
		     nlabel += 3;
		   }
	for (i=0; i < pipar->a_groundsensor2; i++, nlabel++)
	   sprintf(neuronl[nlabel],"g%d",i);
	for (i=0; i < pipar->groundsensor; i++, nlabel++)
	   sprintf(neuronl[nlabel],"G%d",i);
	for (i=0; i < pipar->a_groundsensor; i++, nlabel++)
	   sprintf(neuronl[nlabel],"g%d",i);
	for (i=0; i < pipar->input_outputs; i++, nlabel++)
	      sprintf(neuronl[nlabel],"O%d",i);
	if (pipar->nutrients   > 0)
		   {	   
	             sprintf(neuronl[nlabel],"N1");
	             sprintf(neuronl[nlabel+1],"N2");
		     nlabel += 2;
		   }
	if (pipar->compass   == 1)
		   {	   
	             sprintf(neuronl[nlabel],"C1");
	             sprintf(neuronl[nlabel+1],"C2");
		     nlabel += 2;
		   }
	if (pipar->energy   == 1)
		   {	   
	             sprintf(neuronl[nlabel],"E");
		     nlabel += 1;
		   }
	for (i=0; i < pipar->arbitration; i++, nlabel++)
	      sprintf(neuronl[nlabel],"A%d",i);
        
	// hidden neurons	   
        for (i=0; i < pipar->nhiddens; i++, nlabel++)
               sprintf(neuronl[nlabel],"H%d",i);

	// motor neurons
	if (pipar->wheelmotors > 0)
		 {	
		    for (i=0; i < pipar->wheelmotors; i++, nlabel++)
		       sprintf(neuronl[nlabel],"M%d",i);
		 }
	for (i=0; i < pipar->signalso; i++, nlabel++)
		   sprintf(neuronl[nlabel],"S%d",i);
	for (i=0; i < pipar->input_outputs; i++, nlabel++)
		   sprintf(neuronl[nlabel],"O%d",i);
	for (i=0; i < pipar->arbitration; i++, nlabel++)
		   sprintf(neuronl[nlabel],"A%d",i);
	
        // fitness 
        sprintf(neuronl[nlabel],"F");
        nlabel++;

}

/*
 * This function is used to extract delta or weight values from the free parameters
 * It also check whether all neurons are activated
 * Return the number of free parameters (genes)
 */
void
pseudo_activate_net(float *p, int nneurons, int ddelta, int dweight, int dbias)

{
    int i;
    int t;
    int b;
    float delta;
    int   updated[40]; 
    int   ng;
    int   warning;
    int   mode;

    // if mode == 0, free parameters has not been created yet
    // therefore we do not read them
    mode = ddelta + dweight + dbias;

    ng  = 0;
    warning = 0;
    for(i=0;i < nneurons;i++)
      updated[i] = 0;
    // biases
    for(i=0;i < nneurons;i++)
	 {
	   if (neuronbias[i] == 1)
	   {
	     if (dbias > 0)
               sprintf(neuronl[i],"%.1f",*p);
	     if (mode > 0)
	       p++;
	     ng++;
	   }
	 }

     // blocks
     for (b=0; b < net_nblocks; b++)
	 {	  
	   // connection block
	   if (net_block[b][0] == 0)
	   {
	     for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
	      for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++)
	      {
		// extract the weights
                if (dweight == 1 && cneuron[0] == t) 
                  sprintf(neuronl[i],"%.1f",*p);
                if (dweight == 1 && cneuron[0] == i) 
                  sprintf(neuronl[t],"%.1f",*p);
		if (mode > 0)
	          p++;
		ng++;
	      }
	     
	   } 
	   // update block
	   if (net_block[b][0] == 1)
	   {
	     for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
	      {
                  updated[t] +=1;
		  if (neurondelta[t] == 0)
		   {
		    ;
		   }
		   else
		   {
		    if (mode > 0)
		      delta = (float) (fabs((double) *p) / wrange);
		    if (ddelta > 0)
                      sprintf(neuronl[t],"%.1f",delta);
		    if (mode > 0)
		      p++;
		    ng++;
		   }
	      }
	   }
	 }

}

// -----------------------------------------------------
// Dialog
// -----------------------------------------------------

NetworkDialog::NetworkDialog(QWidget *parent) :
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

NetworkDialog::~NetworkDialog()
{
}

void NetworkDialog::setWidget(QWidget *w, int r, int c)
{
	m_layout->addWidget(w, r, c);
}

QToolBar* NetworkDialog::toolBar()
{
	if (m_toolBar == NULL) {
		// Creating toolbar and adding it to the main layout
		m_toolBar = new QToolBar(this);
		m_mainLayout->addWidget(m_toolBar);
	}

	return m_toolBar;
}

void NetworkDialog::closeEvent(QCloseEvent *)
{
	emit dialogClosed();
}

void NetworkDialog::keyReleaseEvent(QKeyEvent* event)
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

void NetworkDialog::shotWidget()
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
