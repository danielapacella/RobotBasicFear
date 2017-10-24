/* 
 * Evorobot* - rend_controllers.cpp
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
#include "ncontroller.h"
#include "rend_controller.h"

#ifdef EVOGRAPHICS  

#include <QtGui>        // Widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog
#include <QImage>       // the background image of the controller

RendNetwork *rendNetwork;      // The network rendering widget
NetworkDialog *networkDialog;  // The network Dialog
RendControlstate *rendControlstate; // The state of controller widget

QImage *backgimage;     // the background image of the controller


int drawblocks=0;              // whether we display or not connections and update blocks 
int wnetwork=0;                // whether the window has been already opened or not
float   ngraph[1000][MAXN];      // value to be displayed for neurons
int     ngraphid[MAXN];          // id of the neuron to be displayed
int     cneuron[4];            // selected neurons
int     cneuronn=0;            // n. of selected cneurons
int     rnrun;                 // the current cycle
int     rnmousex;              // mouse left-clicked x position
int     rnmousey;              // mouse left-clicked y position
//float   *vectorv;              // vector of selected parameters 
float   **graphicp;            // vector of parameter displayed through the graphic interface (max length = nneurons)
int     ngp=0;                 // number of cirrently extracted parameters
float   cscalex=1.0;           // x scale for the controller rendering
float   cscaley=1.0;           // y scale for the controller rendering
int     pseudomode = 0;        // the display modality (0=label 1=delta, 2=weight, 3=biases, 4=gain)
void pseudo_activate_net(float *p, int nneurons, int dmode);

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
       rendControlstate = new RendControlstate;
       backgimage = NULL;
       backgimage = new QImage("controller.bmp");
       wnetwork = 1;
      }

    if (mode == 2 && wnetwork == 1)
    {
     networkDialog = new NetworkDialog(NULL);

     networkDialog->setWindowTitle("Nervous System");
#ifdef EVOLANGUAGEIT 
     networkDialog->setWindowTitle("Sistema Nervoso");
#endif
     networkDialog->setWidget(rendNetwork, 0, 0);
     networkDialog->show();
     update_rendnetwork(0);
     //vectorv = (float *) malloc(nneurons * sizeof(float *));
     graphicp = (float **) malloc(nneurons * sizeof(float **));
     //create_fvparameter("vectorv", "network",vectorv, 3, 0, "selected free parameters");
     create_fvpparameter("graphicp", "#interface",graphicp, nneurons, 0, "selected free parameters");
     create_fvpparameter("graphicp", "#interface",graphicp, 0, 0, "");
     create_fvparameter("neuronlesion", "#interface",neuronlesion, nneurons, 0, "activation values of frozen neurons");
     wnetwork = 2;
    }
    if (mode == 2 && wnetwork == 2)
    {
     if (wincontrollerx >= 0 && wincontrollery >= 0)
      networkDialog->move(wincontrollerx, wincontrollery);
     if (wincontrollerdx > 0 && wincontrollerdy > 0)
      networkDialog->resize(wincontrollerdx, wincontrollerdy);
     networkDialog->show();
     update_rendnetwork(0);
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
update_rendnetwork(int cycle)


{

     rnrun = cycle;

     rendNetwork->repaint();
     QCoreApplication::processEvents();

}

void
add_controlstate_widget(QGridLayout *mainLayout, int row, int column)
{
  mainLayout->addWidget(rendControlstate, row, column);
}

void
update_rendcontrolstate(int cycle)


{

     rnrun = cycle;

     rendControlstate->repaint();
     QCoreApplication::processEvents();

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
    networktoolbar->addAction(openAct);
    networktoolbar->addAction(saveAct); 
    networktoolbar->addAction(randomfreepAct); 
    if (userlevel > 3)
    {
     networktoolbar->addAction(set_neurondeltaAct);
     networktoolbar->addAction(set_neurongainAct);
     networktoolbar->addAction(set_neuronbiasAct);
     networktoolbar->addAction(set_lesionAct);
     networktoolbar->addAction(set_neurondisplayAct);
    }
    if (userlevel > 0)
    {
     networktoolbar->addAction(display_labelAct);
     networktoolbar->addAction(display_weightAct);
     networktoolbar->addAction(display_deltaAct);
     networktoolbar->addAction(display_biasAct);
     networktoolbar->addAction(display_gainAct);
    }
    if (userlevel > 3)
    {
     networktoolbar->addAction(display_blocksAct);
     networktoolbar->addAction(erase_netAct);
     networktoolbar->addAction(add_ublockAct);
     networktoolbar->addAction(add_cblockAct);
     networktoolbar->addAction(add_gblockAct);
    }
    networktoolbar->addAction(downloadcAct);

}

void NetworkDialog::createActions()
{

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAct->setStatusTip(tr("Open a network architecture or a phenotype file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setStatusTip(tr("Save the network architecture or the phenotype from a file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    randomfreepAct = new QAction(QIcon(":/images/light.png"), tr("&Randomize"), this);
    randomfreepAct->setStatusTip(tr("Randomize thee free parameters (population and individual)"));
    connect(randomfreepAct, SIGNAL(triggered()), this, SLOT(randomfreep()));

    set_neurondisplayAct = new QAction(QIcon(":/images/show.png"), tr("&Set/Unset neurons to be displayed as graph"), this);
    set_neurondisplayAct->setShortcut(tr("Ctrl+N"));
    set_neurondisplayAct->setStatusTip(tr("Select/Deselect neurons to be displayed as graph"));
    connect(set_neurondisplayAct, SIGNAL(triggered()), this, SLOT(set_neurondisplay()));

    set_neurondeltaAct = new QAction(QIcon(":/images/delta.png"), tr("&Add/Remove neurons deltas"), this);
    set_neurondeltaAct->setShortcut(tr("Ctrl+W"));
    set_neurondeltaAct->setStatusTip(tr("Add/Remove neurons delta"));
    connect(set_neurondeltaAct, SIGNAL(triggered()), this, SLOT(set_neurondelta()));

    set_neurongainAct = new QAction(QIcon(":/images/gain.png"), tr("&Add/Remove neurons gain"), this);
    set_neurongainAct->setShortcut(tr("Ctrl+G"));
    set_neurongainAct->setStatusTip(tr("Add/Remove neurons gain"));
    connect(set_neurongainAct, SIGNAL(triggered()), this, SLOT(set_neurongain()));

    set_neuronbiasAct = new QAction(QIcon(":/images/bias.png"), tr("&Add/Remove neurons biases"), this);
    set_neuronbiasAct->setShortcut(tr("Ctrl+B"));
    set_neuronbiasAct->setStatusTip(tr("Add/Remove neurons bias"));
    connect(set_neuronbiasAct, SIGNAL(triggered()), this, SLOT(set_neuronbias()));

    set_lesionAct = new QAction(QIcon(":/images/lesion.png"), tr("&Lesion/Restore neurons"), this);
    set_lesionAct->setShortcut(tr("Ctrl+B"));
    set_lesionAct->setStatusTip(tr("Lesion/Restore neurons"));
    connect(set_lesionAct, SIGNAL(triggered()), this, SLOT(set_lesion()));

    display_labelAct = new QAction(QIcon(":/images/label.png"), tr("&Display/Undisplay neuron labels"), this);
    display_labelAct->setShortcut(tr("Ctrl+L"));
    display_labelAct->setStatusTip(tr("Display neuron labels"));
    connect(display_labelAct, SIGNAL(triggered()), this, SLOT(display_label()));

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

    display_gainAct = new QAction(QIcon(":/images/dgain.png"), tr("&Display/Undisplay gains"), this);
    display_gainAct->setShortcut(tr("Ctrl+G"));
    display_gainAct->setStatusTip(tr("Display neurons gains"));
    connect(display_gainAct, SIGNAL(triggered()), this, SLOT(display_gain()));

    display_blocksAct = new QAction(QIcon(":/images/displayblocks.png"), tr("&Display/Undisplay blocks"), this);
    display_blocksAct->setShortcut(tr("Ctrl+B"));
    display_blocksAct->setStatusTip(tr("Display blocks description"));
    connect(display_blocksAct, SIGNAL(triggered()), this, SLOT(display_blocks()));

    erase_netAct = new QAction(QIcon(":/images/erase.png"), tr("&Erase network"), this);
    erase_netAct->setShortcut(tr("Ctrl+E"));
    erase_netAct->setStatusTip(tr("Erase selected/all connections and update blocks"));
    connect(erase_netAct, SIGNAL(triggered()), this, SLOT(erase_net()));

    add_ublockAct = new QAction(QIcon(":/images/addublock.png"), tr("&Add Update Block"), this);
    add_ublockAct->setShortcut(tr("Ctrl+A"));
    add_ublockAct->setStatusTip(tr("add an update block"));
    connect(add_ublockAct, SIGNAL(triggered()), this, SLOT(add_ublock()));

    add_cblockAct = new QAction(QIcon(":/images/addblock.png"), tr("&Add Connection Block"), this);
    add_cblockAct->setShortcut(tr("Ctrl+A"));
    add_cblockAct->setStatusTip(tr("add a connection block"));
    connect(add_cblockAct, SIGNAL(triggered()), this, SLOT(add_cblock()));

    add_gblockAct = new QAction(QIcon(":/images/gainblock.png"), tr("&Add a gain block"), this);
    add_gblockAct->setShortcut(tr("Ctrl+G"));
    add_gblockAct->setStatusTip(tr("add a gain block"));
    connect(add_gblockAct, SIGNAL(triggered()), this, SLOT(add_gblock()));
    
    increasevAct = new QShortcut(tr("+"), this); 
    connect(increasevAct, SIGNAL(activated()), this, SLOT(increasev()));
    
    decreasevAct = new QShortcut(tr("-"), this);
    connect(decreasevAct, SIGNAL(activated()), this, SLOT(decreasev()));

    downloadcAct = new QAction(QIcon(":/images/download.png"), tr("&Download"), this);
    downloadcAct->setStatusTip(tr("Download the controller on the robot"));
    connect(downloadcAct, SIGNAL(triggered()), this, SLOT(downloadc()));

}


// Randomize the free parameters (population an dindividual/team)
void NetworkDialog::randomfreep()
{

    randomizefreep();
    cneuronn = 0;
    update_rendnetwork(0);

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
   update_rendnetwork(0);

}
// change neurons' type
void NetworkDialog::set_neurondelta()
{
    int i;
    
    if (cneuronn == 1)
    {
        neurondelta[cneuron[0]] += 1;
        if (neurondelta[cneuron[0]] > 1)
            neurondelta[cneuron[0]] = 0;
    }
    if (cneuronn == 2)
    {
        for (i=cneuron[0]; i <= cneuron[1]; i++)
        {  
            neurondelta[i] += 1;
            if (neurondelta[i] > 1) 
                neurondelta[i] = 0;
        }
    }
    if (cneuronn < 1 || cneuronn > 2)
        display_warning("you should select one neuron or one block of neurons first");
    
    update_rendnetwork(0);

}


// define or undefine neurons gain
void NetworkDialog::set_neurongain()
{
   int i;

   if (cneuronn == 1)
   {
     if (neurongain[cneuron[0]] == 1)
       neurongain[cneuron[0]] = 0;
      else
       neurongain[cneuron[0]] = 1;
   }
   if (cneuronn == 2)
   {
     if (neurongain[cneuron[0]] == 1)
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurongain[i] = 0;
     else
      for (i=cneuron[0]; i <= cneuron[1]; i++)
       neurongain[i] = 1;
   }
   if (cneuronn < 1 || cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons first");

    cneuronn = 0;
    update_rendnetwork(0);

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
   update_rendnetwork(0);
}

/*
 * Lesion/Restore neurons
 */
void NetworkDialog::set_lesion()
{
   int i;
   float *nl;

   // Restore the functionality of all neurons
   if (cneuronn == 0)
   {
     for(i=0, nl=neuronlesion; i < ncontrollers->nneurons; i++, nl++)
    *nl = -9.9;
     neuronlesions = 0;
   }
   // lesion one neuron 
   if (cneuronn == 1)
    {
      nl = (neuronlesion + cneuron[0]);
      *nl = 0.0;
    }

   // lesion a block of neurons
   if (cneuronn == 2)
     for(i=0, nl=neuronlesion; i < ncontrollers->nneurons; i++, nl++)
    {
      if (i >= cneuron[0] && i <= cneuron[1])
          *nl = 0.0;
    }

   if (cneuronn > 2)
     display_warning("you should select one neuron or one block of neurons");

   cneuronn = 0;
   neuronlesions = 0;
   for(i=0, nl=neuronlesion; i < ncontrollers->nneurons; i++, nl++)
    {
      if (*nl < -1.0)
       {
            strcpy(neuroncl[i],neuronl[i]);
       }
      else
       {
            sprintf(neuroncl[i],"%.1f", *nl);
        neuronlesions = 1;
       }
    }
   update_rendnetwork(0);
}


// Display neurons label
void NetworkDialog::display_label()
{
   struct ncontroller *cncontroller;

   pseudomode = 0;
   cncontroller = (ncontrollers + cindividual);
   pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
   update_rendnetwork(0);

}



// Display/Undisplay neurons delta
void NetworkDialog::display_delta()
{
   struct ncontroller *cncontroller;

   pseudomode = 1;
   cncontroller = (ncontrollers + cindividual);
   pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
   update_rendnetwork(0);

}


// Display/Undisplay the weights of a neuron
void NetworkDialog::display_weight()
{

   struct ncontroller *cncontroller;

   if (cneuronn == 1 || cneuronn == 2)
     {
      pseudomode = 2;
      cncontroller = (ncontrollers + cindividual);
      pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
      update_rendnetwork(0);
     }
     else
     {
      display_warning("you should first select one or two (receiving and sending) neurons");
     }
}

// Display/Undisplay neurons biases
void NetworkDialog::display_bias()
{
   struct ncontroller *cncontroller;

   pseudomode = 3;
   cncontroller = (ncontrollers + cindividual);
   pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
   update_rendnetwork(0);

}

// Display/Undisplay neurons gains
void NetworkDialog::display_gain()
{
   struct ncontroller *cncontroller;

   pseudomode = 4;
   cncontroller = (ncontrollers + cindividual);
   pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
   update_rendnetwork(0);

}

// Display/Undisplay blocks description
void NetworkDialog::display_blocks()
{

  if (drawblocks == 0)
    drawblocks = 1;
  else
    drawblocks = 0;
  update_rendnetwork(0);

}

// Erase selected/all connections and update blocks
// gain-block removal not implemented yet
void NetworkDialog::erase_net()
{

   int i,ii;
   int b,bb;

   if (cneuronn == 0)
   {
    net_nblocks = 0;
    for(i=0;i < ncontrollers->nneurons; i++)
       {
             neuronbias[i] = 0;
             neurondelta[i] = 0;
         neurondisplay[i] = 1;
       }
    }
   if (cneuronn == 4)
   {
     for (b=0; b < net_nblocks; b++)
      {
       if (net_block[b][0] == 0 && net_block[b][1] == cneuron[0]  && net_block[b][2] == (cneuron[1] - cneuron[0] + 1) && net_block[b][3] == cneuron[2] && net_block[b][4] == (cneuron[3] - cneuron[2] + 1) )
        {
         for (bb=b; bb < (net_nblocks - 1); bb++)
           for (ii=0; ii < 5; ii++)
             net_block[bb][ii] = net_block[bb+1][ii];
     net_nblocks--;
    }
      }
    }
   if (cneuronn == 2)
   {
     for (b=0; b < net_nblocks; b++)
      {
       if (net_block[b][0] == 1 && net_block[b][1] == cneuron[0] && net_block[b][2] == (cneuron[1] - cneuron[0] + 1))
        {
         for (bb=b; bb < (net_nblocks - 1); bb++)
           for (ii=0; ii < 5; ii++)
             net_block[bb][ii] = net_block[bb+1][ii];
     net_nblocks--;
    }
       if (net_block[b][0] == 0 && net_block[b][1] == cneuron[0] && net_block[b][2] == 1 && net_block[b][3] == cneuron[1] && net_block[b][4] == 1)
        {
         for (bb=b; bb < (net_nblocks - 1); bb++)
           for (ii=0; ii < 5; ii++)
             net_block[bb][ii] = net_block[bb+1][ii];
     net_nblocks--;
    }
      }
    }

   update_rendnetwork(0);
}


// Add an update block
void NetworkDialog::add_ublock()
{

  int i;
  char message[64];

  i = net_nblocks;

  if (cneuronn != 1 && cneuronn != 2)
    {
      sprintf(message, "this command require to select 1 or 2 neurons");
      display_error(message);
    }

  if (cneuronn == 1)
    {
    net_block[i][0] = 1;
        net_block[i][1] = cneuron[0];
    net_block[i][2] = 1;
    net_block[i][3] = 0;
    net_block[i][4] = 0;
    net_nblocks++;
    cneuronn = 0;
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

   update_rendnetwork(0);
}


// Add a connection block (at the end of the list but before the block that activate the corresponding neurons, if present)
void NetworkDialog::add_cblock()
{

  int i;
  int ii;
  int v;
  int b;
  char message[64];

  if (cneuronn != 2 && cneuronn != 4)
    {
      sprintf(message, "this command require to select 2 or 4 neurons");
      display_error(message);
      return;
    }

  b = net_nblocks;
  for(i=0; i < net_nblocks; i++)
    {
      if (net_block[i][0] == 1 && (cneuron[0] >= net_block[i][1]) && (cneuron[0] <= (net_block[i][1] + net_block[i][2] - 1)) )
       {
        b = i;
        for(ii=net_nblocks; ii > b; ii--)
         {
          for(v=0; v < 5; v++)
           net_block[ii][v] = net_block[ii-1][v];
         }
       }
    }

  if (cneuronn == 2)
    {
    net_block[b][0] = 0;
    net_block[b][1] = cneuron[0];
        net_block[b][2] = 1;
    net_block[b][3] = cneuron[1];
        net_block[b][4] = 1;
    net_nblocks++;
    cneuronn = 0;
    }

  if (cneuronn == 4)
    {
    net_block[b][0] = 0;
    net_block[b][1] = cneuron[0];
        net_block[b][2] = cneuron[1] - cneuron[0] + 1;
    net_block[b][3] = cneuron[2];
        net_block[b][4] = cneuron[3] - cneuron[2] + 1;
    net_nblocks++;
    cneuronn = 0;
    }

   update_rendnetwork(0);
}

// Add a gain block
void NetworkDialog::add_gblock()
{

  int i;
  char message[64];

  i = net_nblocks;

  if (cneuronn != 2 && cneuronn != 3)
    {
      sprintf(message, "this command require to select 2 or 3 neurons");
      display_error(message);
    }

  // a type-2 block operate by making the gain of a block of neurons
  // all equal to the gain of the first neuron
  if (cneuronn == 2)
    {
    net_block[i][0] = 2;
    net_block[i][1] = cneuron[0];
        net_block[i][2] = cneuron[1] - cneuron[0] + 1;
    net_block[i][3] = 0;
        net_block[i][4] = 0;
    net_nblocks++;
    cneuronn = 0;
    }
  // a type-3 block operate by making the gain of a block of neurons
  // all equal to activation state of the 3rd neuron
  if (cneuronn == 3)
    {
    net_block[i][0] = 3;
    net_block[i][1] = cneuron[0];
        net_block[i][2] = cneuron[1] - cneuron[0] + 1;
    net_block[i][3] = cneuron[2];
        net_block[i][4] = 0;
    net_nblocks++;
    cneuronn = 0;
    }

   update_rendnetwork(0);
}


/*
 * open a .net or .phe file
 */
void NetworkDialog::open()
{

    char *f;
    QByteArray filen;
    char filename[256];


        QString fileName = QFileDialog::getOpenFileName(this,
            "Choose a filename to open",
                    "",
                    "*.net *.phe");


        if (fileName.endsWith("net"))
    {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      load_net_blocks(filename, 1);
    }
    else
     if (fileName.endsWith("phe"))
     {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      load_phenotype_data(filename, ncontrollers->freep, ncontrollers->nfreep);
     }

}

/*
 * save a .net or .phe file
 */
void NetworkDialog::save()
{

    char *f;
    QByteArray filen;
    char filename[256];


        QString fileName = QFileDialog::getSaveFileName(this,
            "Choose a filename to save",
                    "",
                    "*.net *.phe");
		//display_error(fileName.toAscii().data());

        if (fileName.endsWith("net"))
    {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      save_net_blocks(filename);
    }
    else
     if (fileName.endsWith("phe"))
     {
          filen = fileName.toAscii();
          f = filen.data();
      strcpy(filename, f);
      save_phenotype_data(filename, ncontrollers->freep, ncontrollers->nfreep);
     }

}

// Increase a selected value
void NetworkDialog::increasev()
{
   float **gp;
   float *v;
   struct ncontroller *cncontroller;


   // when 2 neurons are selected we assume that the user want to change a connection weight
   if (cneuronn == 2)
     {
      pseudomode = 2;
      cncontroller = (ncontrollers + cindividual);
      pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
      if (ngp == 1)
      {
       gp = graphicp;
       v = *gp;
       *v += ((wrange * 2.0f) / 256.0f);
       pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
       update_rendnetwork(0);
      }
     }
   // when 1 neuron is selected we modify the bias of the neuron, 
   // or the delta or the gain of the neuron (if the user previously pushed the delta or gain buttons, respectively) 
   if (cneuronn == 1)
     {
      if (pseudomode != 1 && pseudomode != 4)
        pseudomode = 3;
      cncontroller = (ncontrollers + cindividual);
      pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
      if (ngp == 1)
      {
       gp = graphicp;
       v = *gp;
       *v += ((wrange * 2.0f) / 256.0f);
       pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
       update_rendnetwork(0);
      }
     }

}

// Decrease a selected value
void NetworkDialog::decreasev()
{

    float **gp;
    float *v;
    struct ncontroller *cncontroller;


   // when 2 neurons are selected we assume that the user want to change a connection weight
   if (cneuronn == 2)
     {
      pseudomode = 2;
      cncontroller = (ncontrollers + cindividual);
      pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
      if (ngp == 1)
      {
       gp = graphicp;
       v = *gp;
       *v -= ((wrange * 2.0f) / 256.0f);
       pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
       update_rendnetwork(0);
      }
     }
   // when 1 neuron is selected we modify the bias of the neuron, 
   // or the delta or the gain of the neuron (if the user previously pushed the delta or gain buttons, respectively) 
   if (cneuronn == 1)
     {
      if (pseudomode != 1 && pseudomode != 4)
        pseudomode = 3;
      cncontroller = (ncontrollers + cindividual);
      pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
      if (ngp == 1)
      {
       gp = graphicp;
       v = *gp;
       *v -= ((wrange * 2.0f) / 256.0f);
       pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,-1);
       update_rendnetwork(0);
      }
     }

}



// -----------------------------------------------------
// Widget RendNetwork
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
    return QSize(250, 250);
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
   struct ncontroller *cncontroller; 
   float  *p;
   int     cr,cb,cg;
   int     wid;
   float   w;
   float   vbiases[500];
    

   QPainter painter(this);

   QRect labelxy(0,0,30,20);          // neuron labels 
   QPoint pxy;
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);

   // background image
   painter.drawImage(0,0,*backgimage);

   // draw neural architecture
   if (drawnxmax > 0 && drawnymax > 0)
    {
     cscaley = height() / (float) drawnymax;
     cscalex = width() / (float) drawnxmax;
     painter.scale(cscalex, cscaley);
    }
    else
    {
     painter.scale(1.0,1.0);
    }
    painter.setPen(Qt::gray);
    cncontroller = ncontrollers;
    p   = cncontroller->freep;
    for(i=0;i < cncontroller->nneurons;i++)
     {
      if (neurongain[i] == 1)
       p++;
     }
    for(i=0;i < cncontroller->nneurons;i++)
     {
      if (neuronbias[i] == 1)
       {
    vbiases[i] = *p;
        p++;
       }
     }
    for (i=0; i < net_nblocks; i++)
    {
      if (net_block[i][0] == 0)
        for (t=net_block[i][1]; t < (net_block[i][1] + net_block[i][2]); t++)
          for (b=net_block[i][3]; b < (net_block[i][3] + net_block[i][4]); b++)
       {
         if (cneuronn == 2 && cneuron[0] == t && cneuron[1] == b)
               wid = 2;
         else
           wid = 1;
         w = *p;
         if (w > wrange)
         w = wrange;
         if (w < (0.0 - wrange))
         w = 0.0 - wrange;
             cr = cg = cb = 255;
             if (w > 0)
              {
               cg -= ((w / wrange) * 255);
               cb -= ((w / wrange) * 255);
              } 
             else
              {
               cg += ((w / wrange) * 255);
               cr += ((w / wrange) * 255);
              } 
 
            painter.setPen(QPen(QColor(cr,cg,cb,255), wid, Qt::SolidLine));
        p++;
            if (abs(neuronxy[b][1] - neuronxy[t][1]) > 20) 
              painter.drawLine((float) neuronxy[t][0], (float) neuronxy[t][1], (float) neuronxy[b][0], (float) neuronxy[b][1]);
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
          painter.drawArc((float) sx,(float) (sy-20),(float) dx, (float) (dy+40), 0 * 16, 180 * 16); 
          }
       }
      if (net_block[i][0] == 1)
       {
         for(t=net_block[i][1]; t < (net_block[i][1] + net_block[i][2]); t++)
              if (neurondelta[t] == 1)
           p++;
           }
    }
    for (i=0; i < ncontrollers->nneurons; i++)
      {
      if (neuronbias[i] == 1)
      {
           wid = 2;
       w = vbiases[i];
       if (w > wrange)
        w = wrange;
       if (w < (0.0 - wrange))
        w = 0.0 - wrange;
           cr = cg = cb = 255;
           if (w > 0)
            {
             cg -= ((w / wrange) * 255);
             cb -= ((w / wrange) * 255);
            } 
           else
            {
             cg += ((w / wrange) * 255);
             cr += ((w / wrange) * 255);
            } 
           painter.setPen(QPen(QColor(cr,cg,cb,255), wid, Qt::SolidLine));
          }         

      else
          {
         painter.setPen(Qt::black);
          }        

       if (neurongain[i] == 0)
        {
        if (neurondelta[i] == 1)
              painter.setBrush(QBrush(QColor(125, 125, 125, 255), Qt::SolidPattern));
         else
              painter.setBrush(QBrush(QColor(125, 125, 125, 255), Qt::Dense5Pattern));
        }
        else
        {
        if (neurondelta[i] == 1)
              painter.setBrush(QBrush(QColor(0, 255, 0, 255), Qt::SolidPattern));
         else
              painter.setBrush(QBrush(QColor(0, 255, 0, 255), Qt::Dense5Pattern));
        }
       painter.drawEllipse((float) (neuronxy[i][0] - 5), (float) (neuronxy[i][1] - 5), 10.0, 10.0);

      }
    painter.setPen(Qt::black);
    for (i=0; i < ncontrollers->nneurons; i++)
      {
        if (neurondisplay[i] == 1)
          painter.setPen(Qt::black);
         else
          painter.setPen(Qt::red);
        if (i < ncontrollers->ninputs)
              labelxy.setRect((float) (neuronxy[i][0] - 5), (float) (neuronxy[i][1] + 5), 30, 30);
         else
              labelxy.setRect((float) (neuronxy[i][0] - 5), (float) (neuronxy[i][1] - 18), 30, 30);
        painter.drawText(labelxy, neuroncl[i]);
      }

    painter.setBrush(Qt::black);
    if (cneuronn == 1)
          painter.drawEllipse((float) (neuronxy[cneuron[0]][0] - 5), (float) (neuronxy[cneuron[0]][1] - 5), 10.0, 10.0);
    if (cneuronn >= 2)
     {
     if (cneuron[0] <= cneuron[1])
      {
       for (i=cneuron[0]; i <= cneuron[1]; i++)
          painter.drawEllipse((float) (neuronxy[i][0] - 5), (float) (neuronxy[i][1] - 5), 10.0, 10.0);
      }
      else
      {
       painter.drawEllipse((float) (neuronxy[cneuron[0]][0] - 5), (float) (neuronxy[cneuron[0]][1] - 5), 10.0, 10.0);
       painter.drawEllipse((float) (neuronxy[cneuron[1]][0] - 5), (float) (neuronxy[cneuron[1]][1] - 5), 10.0, 10.0);
      }
     }
    painter.setBrush(Qt::blue);
    if (cneuronn == 3)
          painter.drawEllipse((float) (neuronxy[cneuron[2]][0] - 5), (float) (neuronxy[cneuron[2]][1] - 5), 10.0, 10.0);
    if (cneuronn == 4 && cneuron[2] <= cneuron[3])
      for (i=cneuron[2]; i <= cneuron[3]; i++)
          painter.drawEllipse((float) (neuronxy[i][0] - 5), (float) (neuronxy[i][1] - 5), 10.0, 10.0);

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
             sprintf(message,"c) %s-%s  %s-%s", neuronl[net_block[b][1]], neuronl[net_block[b][1]+net_block[b][2]-1],neuronl[net_block[b][3]],neuronl[net_block[b][3]+net_block[b][4]-1]);
       if (net_block[b][0] == 1)
             sprintf(message,"u) %s-%s", neuronl[net_block[b][1]], neuronl[net_block[b][1]+net_block[b][2]-1]);
       if (net_block[b][0] == 2)
             sprintf(message,"g) %s-%s", neuronl[net_block[b][1]], neuronl[net_block[b][1]+net_block[b][2]-1]);
       if (net_block[b][0] == 3)
             sprintf(message,"g) %s-%s  %s", neuronl[net_block[b][1]], neuronl[net_block[b][1]+net_block[b][2]-1],neuronl[net_block[b][3]]);
       pxy.setY(b*20+40);
       pxy.setX(0);
       painter.drawText(pxy, message);
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
    int i,t;
    int mode;
        struct ncontroller *cncontroller;
    double cdist, dist;
    
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
      rnmousex = x;
      rnmousey = y;

      //   on a neuron: select the neuron (eventually as part of a block)
      if (mode == 0)
       for(i=0; i < ncontrollers->nneurons; i++)
       {
        if (mdist((float) x,(float) y, (float) neuronxy[i][0] * cscalex, (float) neuronxy[i][1] *cscaley) < 10.0)
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
      //   on a connection: select the receiving and sending neuron 
      if (mode == 0) 
       {
       dist = 999.99;
       for (i=0; i < net_nblocks; i++)
        {
         if (net_block[i][0] == 0)
           for (t=net_block[i][1]; t < (net_block[i][1] + net_block[i][2]); t++)
            for (b=net_block[i][3]; b < (net_block[i][3] + net_block[i][4]); b++)
         {
              cdist = segmentdist((float) x, (float) y, (float) neuronxy[t][0] * cscalex, (float) neuronxy[t][1] * cscaley, (float) neuronxy[b][0] * cscalex, (float) neuronxy[b][1] * cscaley);
          {
           if (cdist < 1 && cdist < dist)
            {
             dist = cdist;
             cneuron[0] = t;
             cneuron[1] = b;
             cneuronn = 2;
         mode = 1;
                }
              }
             }
        }
       }
      // otherwise remove selected neurons 
      // and reset the display modality to label
      if (mode == 0)
        {
     cneuronn = 0;
         pseudomode = 0; 
         cncontroller = (ncontrollers + cindividual);
         pseudo_activate_net(cncontroller->freep, cncontroller->nneurons,0);
        }
      repaint();
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
    int mode;
    QPoint xy;
    int   cn;
    char  sbuffer[128];


    if (evolution_is_running == 0)
    {

    x=event->x();
    y=event->y();

    mode = 0;

    // move current neuron
        cneuronn = 0;
    if (mode == 0)
         {
     for(i=0, cn=-1; i < ncontrollers->nneurons; i++)
       if (mdist((float) rnmousex,(float) rnmousey, (float) neuronxy[i][0] * cscalex, (float) neuronxy[i][1] * cscaley) < 10.0)
         cn = i;
     if (cn >= 0)
          {
            neuronxy[cn][0] = (float) x / cscalex;
            neuronxy[cn][1] = (float) y / cscaley;
            repaint();
            rnmousex = x;
            rnmousey = y;
            mode = 1;
            sprintf(sbuffer,"neuron #%d moved to %d %d", cn, neuronxy[cn][0], neuronxy[cn][1]);
            display_stat_message(sbuffer);
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
    QPoint xy;
    float *o;
    float oldp1,oldp2;
    int   cn;
    char  nobject[24];
    int   idobject;
    char  sbuffer[128];


    if (evolution_is_running == 0 && grid > 0 )
    {

    rnmousex=event->x();
    rnmousey=event->y();

    mode = 0;

    // move current neuron
    
    if (mode == 0)
     for(i=0, cn=-1; i < ncontrollers->nneurons; i++)
       if (mdist((float) rnmousex,(float) rnmousey, neuronxy[i][0], neuronxy[i][1]) < 10.0)
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
            repaint();
        sprintf(sbuffer,"neuron #%d released to %d %d", cn, neuronxy[cn][0], neuronxy[cn][1]);
        display_stat_message(sbuffer);
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
        sprintf(sbuffer,"object %s #%d released to %.1f %.1f", nobject, idobject, *o, *(o + 1));
        display_stat_message(sbuffer);
            repaint();
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

// -----------------------------------------------------
// Widget RendControlstate
// -----------------------------------------------------

RendControlstate::RendControlstate(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
}

QSize RendControlstate::minimumSizeHint() const
{
    return QSize(250, 250);
}

QSize RendControlstate::sizeHint() const
{
   return QSize(550, 500);
}

void RendControlstate::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendControlstate::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}



void RendControlstate::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendControlstate::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendControlstate::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}


void RendControlstate::paintEvent(QPaintEvent *)
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
   struct ncontroller *cncontroller; 
   float  *p;
   int     cr,cb,cg;
   int     wid;
   float   w;
   float   vbiases[500];
    

   QPainter painter(this);

   QRect labelxy(0,0,30,20);          // neuron labels 
   QPoint pxy;
   painter.setPen(pen);
   QPen pen(Qt::black, 1);                 // black solid line, 1 pixels wide
   painter.setPen(pen);

   // draw neurons state
   if (testindividual_is_running == 1)
   {
     painter.scale(1.0,1.0);
     draw_neurons_update();
     offx = 50;
     if (drawn_alltogether == 1)
       h2 = height() / (ndu / nteam);
      else
       h2 = height() / ndu;

	 
     if (h2 > 40)
      h2 = 40;
     h1 = h2 - 3;
     n_start = 0;
     n_max   = rnrun;
     if (n_max > drawneuronsdx)
      n_max = drawneuronsdx;
     if (rnrun > drawneuronsdx)
       n_start = (rnrun-drawneuronsdx) % drawneuronsdx; 
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
            sprintf(message, "%.1f" , ngraph[rnrun%drawneuronsdx][u]);
            labelxy.setRect(offx-30+drawneuronsdx+50, offy-(h2/2), 30+drawneuronsdx+50, 30);
         painter.drawText(labelxy, message);
      }

      if (drawn_alltogether == 0)
      {
      if (ngraphid[u] < ncontrollers->ninputs)
            painter.setPen(QColor(255,0,0,255));
      if (ngraphid[u] >= ncontrollers->ninputs && ngraphid[u] < (ncontrollers->nneurons - ncontrollers->noutputs))
            painter.setPen(QColor(180,180,180,255));
      if (ngraphid[u] >= (ncontrollers->nneurons - ncontrollers->noutputs))
            painter.setPen(QColor(0,0,255,255));
      if (ngraphid[u] == ncontrollers->nneurons)
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


}

/*
 * compute and return the number of neurons to be drawn
 */ 
void
RendControlstate::draw_neurons_update()
{

    int    team;
    struct  ncontroller *cncontroller;
    int     u;
    int     nlabel;
    
    ndu = 0;
    nlabel = 0;
    // update the matrix of value with the current value
    // and compute the number of neurons to be drawn
    for (team=0,cncontroller=ncontrollers; team<nteam; team++, cncontroller++)
    {
      if (team == cindividual || drawn_alltogether == 1)
      {
        for(u=(cncontroller->nneurons - cncontroller->noutputs); u < cncontroller->nneurons; u++)
          if (neurondisplay[u] == 1 && ndu < 200)
          {     
                    ngraph[rnrun%drawneuronsdx][ndu] = cncontroller->activation[u];
            ngraphid[ndu] = u;
            ndu++;
          }
        for(u=cncontroller->ninputs; u < (cncontroller->nneurons - cncontroller->noutputs); u++)
          if (neurondisplay[u] == 1 && ndu < 200)
          {     
                    ngraph[rnrun%drawneuronsdx][ndu] = cncontroller->activation[u];
            ngraphid[ndu] = u;
            ndu++;
          }
        for(u=0; u < cncontroller->ninputs; u++)
          if (neurondisplay[u] == 1 && ndu < 200)
          {     
                    ngraph[rnrun%drawneuronsdx][ndu] = cncontroller->activation[u];
            ngraphid[ndu] = u;
            ndu++;
          }
      }
    }
}





////////////////////////////////////////////////////////////////

/*
 * This function copy delta, or weight, or biases, or label into neurons clabels
 * and into the graphicp parameter vector
 */
void
pseudo_activate_net(float *p, int nneurons, int dmode)

{
    int i;
    int t;
    int b;
    float delta;
    float gain;
    int   updated[MAXN]; 
    float **gp;
    int dlabel=0;
    int ddelta=0;
    int dweight=0; 
    int dbias=0;
    int dgain=0;

    switch(pseudomode)
     {
       case 0:
     dlabel = 1;
         break;
       case 1:
     ddelta = 1;
         break;
       case 2:
     dweight = 1;
         break;
       case 3:
     dbias = 1;
         break;
       case 4:
     dgain = 1;
         break;
     }

    gp = graphicp;
    ngp = 0;

    for(i=0;i < nneurons;i++)
      strcpy(neuroncl[i],neuronl[i]); 

    if (dlabel == 1)
     {
      create_fvpparameter("graphicp", "#interface",graphicp, 0, 0, "");
     }
    else
     {
     for(i=0;i < nneurons;i++)
      updated[i] = 0;

     // gain
     for(i=0;i < nneurons;i++)
     {
       if (neurongain[i] == 1)
       {
         if (dgain > 0 && (cneuronn != 1 || cneuron[0] == i))
          {
           gain = (fabs((double) *p) / wrange) * grange;
           sprintf(neuroncl[i],"%.1f",gain);
           *gp = p;
           gp++;
           ngp++;
          }
          p++;
       }
     }

     // biases
     for(i=0;i < nneurons;i++)
     {
       if (neuronbias[i] == 1)
       {
         if (dbias > 0 && (cneuronn != 1 || cneuron[0] == i))
          {
               sprintf(neuroncl[i],"%.1f",*p);
           *gp = p;
           gp++;
           ngp++;
              }
          p++;
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
         if (dweight == 1 && ((cneuronn == 1 && cneuron[0] == t) || (cneuronn == 2 && cneuron[0] == t && cneuron[1] == i))) 
             {
          sprintf(neuroncl[i],"%.1f",*p);
          *gp = p;
              gp++;
              ngp++;
             }
            p++;
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
            delta = (float) (fabs((double) *p) / wrange);
            if (ddelta > 0 && (cneuronn != 1 || cneuron[0] == t))
             {
              sprintf(neuroncl[t],"%.1f",delta);
              *gp = p;
              gp++;
              ngp++;
             }
            p++;
           }
          }
       }
     }

      create_fvpparameter("graphicp", "#interface",graphicp, ngp, 0, "");
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
    m_mainLayout->addLayout(m_layout, 1);

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
        m_mainLayout->addWidget(m_toolBar, 0);
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


/*
 * generate a controller, compile it, and download it into the robot (for lego robot only)
 */
void NetworkDialog::downloadc()
{
    
    int i,r;
    FILE *fpevo;
    FILE *fpcmp; 
    char mystring[300];
    float *p;
    int  np;
    int  nxtcams;
    int  ifs;
    int  glis;
    int  ground;
    int  battery;

     fpcmp=fopen("EvoNetNXT.java","w");
     fpevo=fopen("EvoNetNXTS.dat","r");
     p = ncontrollers->freep;
     np = ncontrollers->nfreep;

     nxtcams = 0;
     ifs = 0;
     glis = 0;
     ground = 0;
     battery = 0;
     for(i=0;i<ncontrollers->ninputs; i++)
     {
       if (strcmp(neuronl[i], "V0") == 0)
            nxtcams = 1;
       if (strcmp(neuronl[i], "I0") == 0 || strcmp(neuronl[i], "I1") == 0  || strcmp(neuronl[i], "I2") == 0)
            ifs++;
       if (strcmp(neuronl[i], "L0") == 0 || strcmp(neuronl[i], "L1") == 0)
            glis++;
       if (strcmp(neuronl[i], "G0") == 0)
            ground = 1;
       if (strcmp(neuronl[i], "E") == 0)
            battery = 1;
     }

     if ((fpevo!= NULL) && (fpcmp!=NULL))
      {
    while(fgets(mystring,300,fpevo)!=NULL)
    {
     fprintf(fpcmp,"%s",mystring);
    }
    fprintf(fpcmp,"ninputs    =    %d;\n",ncontrollers->ninputs);    
    fprintf(fpcmp,"nhiddens=    %d;\n",ncontrollers->nhiddens);
    fprintf(fpcmp,"noutputs=    %d;\n",ncontrollers->noutputs);
    fprintf(fpcmp,"nneurons=    %d;\n",ncontrollers->nneurons);
    fprintf(fpcmp,"net_nblocks= %d;\n",net_nblocks);
    fprintf(fpcmp,"wrange    =    %f;\n",wrange);
    fprintf(fpcmp,"ndata    =    %d;\n",ncontrollers->nfreep);
    fprintf(fpcmp,"nifsensors    =    %d;\n",ifs);
    fprintf(fpcmp,"nxtcamsensor    =    %d;\n",nxtcams);
    fprintf(fpcmp,"glightsensors=    %d;\n",glis);
    fprintf(fpcmp,"groundsensor =    %d;\n",ground);
    fprintf(fpcmp,"battery =    %d;\n",battery);
    fprintf(fpcmp,"motorload    =    %d;\n",0); // motor load non implementato
    fprintf(fpcmp,"neuronbias=new int[nneurons];\n");
    fprintf(fpcmp,"neurondelta=new int[nneurons];\n");
    fprintf(fpcmp,"data=new double[ndata];\n");
    fprintf(fpcmp,"act=new double[nneurons];\n");
    fprintf(fpcmp,"input=new double[ninputs];\n");
    fprintf(fpcmp,"netinput=new double[nneurons];\n");
    fprintf(fpcmp,"net_block=new int[net_nblocks][5];\n");
    fprintf(fpcmp,"for(i=0;i<nneurons;i++) act[i]=0;\n");
    
    for(i=0;i<nneurons;i++)
     {
      fprintf(fpcmp,"neuronbias[%d]=%d;\n",i,neuronbias[i]);
      fprintf(fpcmp,"neurondelta[%d]=%d;\n",i,neurondelta[i]);
     }
    fprintf(fpcmp,"\n");
    for(i=0;i<np;i++)
     {
      fprintf(fpcmp,"data[%d] = %f;\n",i,*p);
      p++;
     }
    for(i=0;i<net_nblocks;i++)
      for(r=0;r<5;r++)
        fprintf(fpcmp,"net_block[%d][%d]=%d;\n",i,r,net_block[i][r]);            
    fprintf(fpcmp,"}\n");
    fprintf(fpcmp,"}\n");
    fclose(fpevo);
    fclose(fpcmp);
    system("nxjc EvoNetNXT.java"); //compiling
    system("nxj  EvoNetNXT");      //downloading
      }
      else
      {
       display_warning("I could not open file EvoNetNXT.java or file EvoNetNXT.dat");
      }

}



#endif
