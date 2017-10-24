/* 
 * Evorobot* - ncontroller.cpp
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

/*
 * network.c
 *
 * There are some improvements to make:
 * The variable nhidden should be allocated here. To do that, however,
 * we have to divide the vector containing the label of the neurons in different groups 
 * (sensory, internal, and motors)
 * We have to allocate the vector for neurons state and the vector for free
 * parameters here dynamically
 * We should include the block description into the individual structure
 * so to allow the use of non-homogeneous individuals
 */

#include "public.h"
#include "ncontroller.h"
#include <QFileInfo>

int ninputs;                // n. input units                          
int nhiddens;               // n. hidden units                    
int noutputs;               // n. output units
int nneurons;               // total n. of neurons
int net_nblocks;            // network number of connection blocks
int net_block[MAXN][5];     // blocks: 0=type (w,b,d), from 1 per 2 receive from 3 per 4
int neuronbias[MAXN];       // whether neurons have bias
int neurondelta[MAXN];      // whether neurons have time constants
int neurongain[MAXN];       // whether neurons have a gain
int neuronxy[MAXN][2];      // neurons xy position in display space
int neurondisplay[MAXN];    // whether neurons should be displayed or not
float *neuronlesion;        // lesioned neurons are frozen on fixed value 
int neuronlesions=0;        // whether we have at least one neuron leasioned
int dynrechid=0;            // recurrent connections on the hiddens
int dynrecout=0;            // recurrent connections on the outputs
int dynioconn=0;            // input output connections
int dyndeltai=0;            // delta on the input units
int dyndeltah=0;            // delta on the hidden units
int dyndeltao=0;            // delta on the output units
int drawnstyle=0;           // style of draws (1=bars, 2=curves)
int drawn_alltogether=0;    // draw individuals' value on the same graph  
int drawneuronsdx=500;      // width of windows area for displaying neurons state  
int drawnxmax=100;          // max display-x position
int drawnymax=100;          // max display-y position
float wrange=5.0;           // range of weights and biases parameters                 
float grange=5.0;           // range of gain parameters                 
float *phenotype;        // phenotype's fixed parameters  
int pheno_loaded=0;         // whether phenotype parameters have been loaded

int     wincontrollerx = -1;   // x coordinate of controller windows (0 = undefined)
int     wincontrollery = -1;   // y coordinate of controller windows (0 = undefined)
int     wincontrollerdx = 0;   // dx of controller windows (0 = undefined)
int     wincontrollerdy = 0;   // dy of controller windows (0 = undefined)


extern float wrange;        // weights and biases range                 

struct ncontroller *ncontrollers=NULL;          // neural controllers                


/*
 * create network parameters
 */
void
create_network_par()

{

   create_iparameter("rec_hiddens", "network",&dynrechid, 0, 1, "whether internal neurons have recurrent connections or not");
   create_iparameter("rec_outputs", "network",&dynrecout, 0, 1, "whether motor neurons have recurrent connections or not");
   create_iparameter("input_output_c", "network",&dynioconn, 0, 1, "whether sensory neurons are connected to motor neurons or not");
   create_iparameter("delta_inputs", "network",&dyndeltai, 0, 1, "whether sensory neurons are leaky neurons or not");
   create_iparameter("delta_hiddens", "network",&dyndeltah, 0, 1, "whether internal neurons are leaky neurons or not");
   create_iparameter("delta_outputs", "network",&dyndeltao, 0, 1, "whether motor neurons are leaky neurons or not");
   create_iparameter("drawnstyle", "network",&drawnstyle, 0, 1, "draw neurons state by using bars or curves");
   create_iparameter("drawn_allt", "network",&drawn_alltogether, 0, 1, "draw the state of the neurons of different individuals on the same graph");
   create_iparameter("drawneuronsdx", "display",&drawneuronsdx, 0, 1000, "width of drawing window");
   create_fparameter("wrange","network",&wrange, 0, 0, "range of weights and biases parameters");
   create_fparameter("grange","network",&grange, 0, 0, "range of gain parameters");
   create_iparameter("wincontrollerx", "display",&wincontrollerx, 0, 0, "x coordinate of controller window");
   create_iparameter("wincontrollery", "display",&wincontrollery, 0, 0, "y coordinate of controller window");
   create_iparameter("wincontrollerdx", "display",&wincontrollerdx, 0, 0, "dx of controller window");
   create_iparameter("wincontrollerdy", "display",&wincontrollerdy, 0, 0, "dy of controller window");

}



/*
 * initialize ncontrollers
 */
void
init_network()

{

    float *nl;
    int   i;
    struct ncontroller *cncontroller;
    int nf;
    float *phe;

    // we create the neural controllers structure
    ncontrollers = (struct ncontroller *) malloc((nteam) * sizeof(struct ncontroller));
    for(i=0, cncontroller = ncontrollers; i < nteam; i++, cncontroller++)
     {
       read_robot_units(i, &cncontroller->ninputs, &cncontroller->nhiddens, &cncontroller->noutputs);
       cncontroller->nneurons = cncontroller->ninputs + cncontroller->nhiddens + cncontroller->noutputs;        
       cncontroller->activation = (float *)malloc(cncontroller->nneurons * sizeof(float));                        
     }

    // create the net blocks
    if (load_net_blocks("evorobot.net", 0) == 0)
      create_net_block(ncontrollers);
    // compute the number of free parameters
    nf = compute_nfreep(ncontrollers->nneurons);

    // allocate space for free parameters
    for(i=0, cncontroller = ncontrollers; i < nteam; i++, cncontroller++)
     {
       cncontroller->nfreep   = nf;
       cncontroller->freep = (float *)malloc(nf * sizeof(float));                        
     }

    phenotype = (float *)malloc(ncontrollers->nfreep * sizeof(float));
    for (i=0,phe=phenotype; i < ncontrollers->nfreep; i++, phe++)
       *phe = -999.0;

    neuronlesion = (float *)malloc(ncontrollers->nneurons * sizeof(float));
    for (i=0, nl=neuronlesion; i < ncontrollers->nneurons; i++, nl++)
      *nl = -9.9;

    load_phenotype_data("robot.phe", ncontrollers->freep, ncontrollers->nfreep);

}

/*
 * return the number of free parameters
 */
void
ncontroller_read_nfreep(int *numfreep)

{
   *numfreep = ncontrollers->nfreep;
}


/*
 * perform the genotype to phenotype mapping for controller i 
 */
void
genotype_to_phenotype(int i, int *geno, int nbits, int homogeneous)

{

    int     j;    
    float   *w;
    int     *gi;
    float   value;
    struct  ncontroller *cncontroller;
    float   *phe;


    cncontroller = (ncontrollers + i);
    if (homogeneous == 0)
      gi = (geno + (cncontroller->nfreep * i));
    else
      gi = geno;
    
  
    for (j=0, w = cncontroller->freep; j < cncontroller->nfreep; j++, w++, gi++)
     {
    value = (float) (*gi + 1) / (float) 256.0;
    *w = wrange - (value * (wrange * (float) 2.0));
     }
    
    if (pheno_loaded == 1) 
      for (j=0, w = cncontroller->freep, phe = phenotype; j < cncontroller->nfreep; j++, w++, phe++)
       {
         if (*phe != -999.0)
      *w = *phe;
       }
}


/*
 * transform phenotype parameters into genotype parameters 
 * values outside the [-wrange,wrange] interval are truncated to the limits
 */
void
phenotype_to_genotype(int i, int *geno, int nbits, int homogeneous)

{

    int     j;    
    float   *w;
    float   v;
    int     *gi;
    float   value;
    struct  ncontroller *cncontroller;
    float   *phe;


    cncontroller = (ncontrollers + i);
    if (homogeneous == 0)
      gi = (geno + (cncontroller->nfreep * i));
    else
      gi = geno;
    

    for (j=0, w = cncontroller->freep; j < cncontroller->nfreep; j++, w++, gi++)
     {
    v = *w; 
    if (v < (0 - wrange))
      v = (0 - wrange);
        if (v > (wrange - ((wrange * 2.0f) / 256.0f)))
      v = (wrange - ((wrange * 2.0f) / 256.0f));
        *gi = (int) ((((wrange * 2.0f)-(v + wrange))* 256.0f) - 10.0f) / 10.0f;
     }
    
}


/*
 * create the block structure for a standard architecture
 * create x-y coordinate for network architecture display
 * store in local variable the number of neurons
 */

void
create_net_block(struct ncontroller *cncontroller) 
{

    int n;
    int i;
    int ng;
    int startx;
    int dx;
    int team;

    ninputs = cncontroller->ninputs;
    noutputs = cncontroller->noutputs;
    nneurons = cncontroller->nneurons;
    nhiddens = nneurons - (ninputs + noutputs);

    // time constants 
    for(i=0; i < ninputs; i++)
      if (dyndeltai > 0)
    neurondelta[i]= 1;
       else
    neurondelta[i]= 0;
    for(i=ninputs; i < (nneurons - noutputs); i++)
      if (dyndeltah > 0)
    neurondelta[i]= 1;
       else
    neurondelta[i]= 0;
    for(i = (nneurons - noutputs); i < nneurons; i++)
      if (dyndeltao > 0)
    neurondelta[i]= 1;
       else
    neurondelta[i]= 0;

    // biases
    for(i=0; i < nneurons; i++)
      if (i < ninputs)
    neuronbias[i]= 0;
       else
    neuronbias[i]= 1;

    // gain
    for(i=0; i < nneurons; i++)
     neurongain[i]= 0;

    net_nblocks = 0;
    // input update block

    net_block[net_nblocks][0] = 1;
    net_block[net_nblocks][1] = 0;
    net_block[net_nblocks][2] = ninputs;
    net_block[net_nblocks][3] = 0;
    net_block[net_nblocks][4] = 0;
    net_nblocks++;

    // input-hidden connections
    if (nhiddens > 0)
      {
    net_block[net_nblocks][0] = 0;
    net_block[net_nblocks][1] = ninputs;
    net_block[net_nblocks][2] = nhiddens;
    net_block[net_nblocks][3] = 0;
    net_block[net_nblocks][4] = ninputs;
    net_nblocks++;
      }

    // hidden-hidden connections
    if (dynrechid > 0)
      {
    net_block[net_nblocks][0] = 0;
    net_block[net_nblocks][1] = ninputs;
    net_block[net_nblocks][2] = nhiddens;
    net_block[net_nblocks][3] = ninputs;
    net_block[net_nblocks][4] = nhiddens;
    net_nblocks++;
      }

    // hidden update block
    if (nhiddens > 0)
      {
       net_block[net_nblocks][0] = 1;
       net_block[net_nblocks][1] = ninputs;
       net_block[net_nblocks][2] = nhiddens;
       net_block[net_nblocks][3] = 0;
       net_block[net_nblocks][4] = 0;
       net_nblocks++;
      }

    // input-output connections
    if (nhiddens == 0 || dynioconn > 0)
      {
    net_block[net_nblocks][0] = 0;
    net_block[net_nblocks][1] = ninputs + nhiddens;
    net_block[net_nblocks][2] = noutputs;
    net_block[net_nblocks][3] = 0;
    net_block[net_nblocks][4] = ninputs;
    net_nblocks++;
      }

    // hidden-output connections
    if (nhiddens > 0)
      {
    net_block[net_nblocks][0] = 0;
    net_block[net_nblocks][1] = ninputs + nhiddens;
    net_block[net_nblocks][2] = noutputs;
    net_block[net_nblocks][3] = ninputs;
    net_block[net_nblocks][4] = nhiddens;
    net_nblocks++;
      }

    // output-output connections
    if (dynrecout > 0)
      {
    net_block[net_nblocks][0] = 0;
    net_block[net_nblocks][1] = ninputs + nhiddens;
    net_block[net_nblocks][2] = noutputs;
    net_block[net_nblocks][3] = ninputs + nhiddens;
    net_block[net_nblocks][4] = noutputs;
    net_nblocks++;
      }

    // output update block
    net_block[net_nblocks][0] = 1;
    net_block[net_nblocks][1] = ninputs + nhiddens;
    net_block[net_nblocks][2] = noutputs;
    net_block[net_nblocks][3] = 0;
    net_block[net_nblocks][4] = 0;
    net_nblocks++;

   // set neuron xy coordinate for display 
   dx = 25;

   n = 0;
   for(i=0; i < ninputs; i++, n++)
   {
     neuronxy[n][0] = (i * dx) + dx;
     neuronxy[n][1] = 400;
   }

   if (ninputs > (nneurons - (ninputs + noutputs)))
     startx = ninputs * dx;

   for(i=0; i < (nneurons - (ninputs + noutputs)); i++, n++)
   {
     neuronxy[n][0] = startx + (i * dx) + dx;
     neuronxy[n][1] = 225;
   }

   if (ninputs > noutputs)
     startx = ((ninputs - noutputs) / 2) * dx;

   for(i=0; i < noutputs; i++, n++)
   {
     neuronxy[n][0] = startx + (i * dx) + dx;
     neuronxy[n][1] = 50;
   }

   // set neurons whose activation should be displayed
   for(i=0; i < nneurons; i++)
     neurondisplay[i] = 1;

   // set neurons whose activation should be displayed
   drawnymax = 400 + 30;
   i = ninputs;
   if (ninputs > i)
     i = noutputs;
   if ((nneurons - noutputs) > i)
     i = (nneurons - noutputs);
   drawnxmax = (i * dx) + dx + 30;

}


/*
 * Update the neural controller
 * and return a pointer to the activation vector 
 */
float *
update_ncontroller(int id, float *input)

{

    int i;
    int t;
    int b;
    float *p;
    float delta;
    float netinput[MAXN];
    float gain[MAXN];
    float *nl;
    float *act;
    struct ncontroller *cncontroller;

    cncontroller = (ncontrollers + id);

    p   = cncontroller->freep;
    act = cncontroller->activation;
    nl  = neuronlesion; 

    // gain
    for(i=0;i < cncontroller->nneurons;i++)
     {
       if (neurongain[i] == 1)
       {
         gain[i] = (float) (fabs((double) *p) / wrange) * grange;
         p++;
       }
        else
       {
         gain[i] = 1.0f;
       }
     }
    // biases
    for(i=0;i < cncontroller->nneurons;i++)
     {
       if (neuronbias[i] == 1)
       {
         netinput[i] = *p;
         p++;
       }
        else
       {
         netinput[i] = 0.0f;
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
        netinput[t] += act[i] * gain[i] * *p;
            p++;
          }
       }
       // gain block (gain of neuron a-b set equal to gain of neuron a)
       if (net_block[b][0] == 2)
       {
         for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
          {
        gain[t] = gain[net_block[b][1]];
          }
       }
       // gain block (gain of neuron a-b set equal to act[c])
       if (net_block[b][0] == 3)
       {
         for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
          {
        gain[t] = act[net_block[b][3]];
          }
       }
       // update block
       if (net_block[b][0] == 1)
       {
         for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
          {
        if (t < cncontroller->ninputs)
        {
         switch(neurondelta[t])
         {
          case 0: // simple rely units
           act[t] = input[t];
           break;
          case 1: // delta neurons
           delta = (float) (fabs((double) *p) / wrange);
           p++;
                   act[t] = (act[t] * delta)  + (input[t] * (1.0f - delta));
           break;
         }
        }
        else
        {
         switch(neurondelta[t])
         {
          case 0: // simple logistic
          act[t] = logistic(netinput[t]);
          delta = 0.0;
              break;
          case 1: // delta neurons
          delta = (float) (fabs((double) *p) / wrange);
          p++;
                  act[t] = (act[t] * delta)  + (logistic(netinput[t]) * (1.0f - delta));
              break;
          case 2: // binary neurons
                  if (netinput[t] >= 0.0)
                    act[t] = 1.0;
           else
                    act[t] = 0.0;
              break;
         }
        }
        if (neuronlesions == 1)
                 {
          if (*nl >= 0.0 && *nl <= 1.0)
                    act[t] = *nl;
          nl++;
             }
          }
       }
     }
   return(act);
}



/*
 * set to 0.0 the activation value of all units
 */
void
reset_controller(int id)
{

  int    i;
  struct ncontroller *cncontroller;

  cncontroller = (ncontrollers + id);

  for(i=0; i < cncontroller->nneurons; i++)
    cncontroller->activation[i] = 0.0f;
}


/*
 * This function compute and return the number of free parameters
 * on the basis of the network architectures
 */
int
compute_nfreep(int nneurons)

{
    int i;
    int t;
    int b;
    float delta;
    int   updated[MAXN]; 
    int   ng;
    int   warning;
    int   mode;
    char  warnst[256];

    // if mode == 0, free parameters has not been created yet
    // therefore we do not read them
    //mode = ddelta + dweight + dbias;

    ng  = 0;
    warning = 0;
    for(i=0;i < nneurons;i++)
      updated[i] = 0;
    // gain
    for(i=0;i < nneurons;i++)
     {
       if (neurongain[i] == 1)
         ng++;
     }
    // biases
    for(i=0;i < nneurons;i++)
     {
       if (neuronbias[i] == 1)
         ng++;
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
            ng++;
           }
          }
       }
     }

        for(i=0;i < nneurons;i++)
    {
          if (updated[i] < 1 && warning == 0)
      {
            sprintf(warnst,"neuron %d will never be activated according to the current architecture", i);
        display_warning(warnst);
        warning++;
      }
          if (updated[i] > 1 && warning == 0)
      {
            sprintf(warnst,"neuron %d will be activated more than once according to the current architecture", i);
        display_warning(warnst);
        warning++;
      }
    }
return(ng);

}



/*
 * save the architecture and the free parameters of a controller 
 */
void
save_net_blocks(char *filename)


{

   FILE    *fp;
   int  b;
   int  n;
   int  i;
   int  t;
   float *p;
   char warnst[256];


   if ((fp = fopen(filename,"w")) != NULL)
    {
     fprintf(fp,"ARCHITECTURE\n");
     fprintf(fp,"nneurons %d\n", nneurons);
     fprintf(fp,"nsensors %d\n", ninputs);
     fprintf(fp,"nmotors %d\n", noutputs);
     fprintf(fp,"nblocks %d\n", net_nblocks);
     for (b=0; b < net_nblocks; b++)
     {
           fprintf(fp,"%d %d %d %d %d", net_block[b][0],net_block[b][1],net_block[b][2],net_block[b][3],net_block[b][4]);
           if (net_block[b][0] == 0)
              fprintf(fp," // connections block\n");
           if (net_block[b][0] == 1)
              fprintf(fp," // block to be updated\n");
           if (net_block[b][0] == 2)
              fprintf(fp," // gain block\n");
           if (net_block[b][0] == 3)
              fprintf(fp," // modulated gain block\n");
     }
     fprintf(fp,"neurons bias, delta, gain, xy position, display\n");
     for(n=0; n < nneurons; n++)
        fprintf(fp,"%d %d %d %d %d %d\n", neuronbias[n], neurondelta[n], neurongain[n], neuronxy[n][0], neuronxy[n][1], neurondisplay[n]);

     p = ncontrollers->freep;
     fprintf(fp,"FREE PARAMETERS %d\n", ncontrollers->nfreep);
     for(i=0;i < nneurons;i++)
     {
       if (neurongain[i] == 1)
       {
         fprintf(fp,"%f \tgain %s\n",*p, neuronl[i]);
         p++;
       }
     }
     for(i=0;i < nneurons;i++)
     {
       if (neuronbias[i] == 1)
       {
         fprintf(fp,"%f \tbias %s\n",*p, neuronl[i]);
         p++;
       }
     }
     for (b=0; b < net_nblocks; b++)
     {
       if (net_block[b][0] == 0)
       {
         for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
          {
           for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++)
           {
        fprintf(fp,"%f \tweight %s from %s\n",*p, neuronl[t], neuronl[i]);
            p++;
           }
          }
       }
       if (net_block[b][0] == 1)
       {
         for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
          {
        if (neurondelta[t] == 1)
           {
                    fprintf(fp,"%f \ttimeconstant %s (%.2f)\n", *p, neuronl[t],(fabs((double) *p) / wrange));
            p++;
           }  
         }
       }       
     } 
     
     fprintf(fp,"END\n");

     sprintf(warnst, "controller saved on file %s", filename);
     display_stat_message(warnst);
     }
    else
     {
     display_warning("unable to create the file evorobot.net");
     }
   fclose(fp);
}


/*
 * load the architecture and the free parameters of a controller
 * mode=0, load only the architecture
 * mode=1, load also the parameters
 */
int
load_net_blocks(char *filename, int mode)


{

   FILE    *fp;
   int   b;
   int   n;
   int   team;
   int   i;
   float *p;
   int   np;
   char  cbuffer[128];
   char  warnst[256];

   if ((fp = fopen(filename,"r")) != NULL)
    {
         fscanf(fp,"ARCHITECTURE\n");
         fscanf(fp,"nneurons %d\n", &nneurons);
         fscanf(fp,"nsensors %d\n", &ninputs);
         fscanf(fp,"nmotors %d\n", &noutputs);
         nhiddens = nneurons - (ninputs + noutputs);
         fscanf(fp,"nblocks %d\n", &net_nblocks);
     for (b=0; b < net_nblocks; b++)
     {
           fscanf(fp,"%d %d %d %d %d", &net_block[b][0],&net_block[b][1],&net_block[b][2],&net_block[b][3],&net_block[b][4]);
           if (net_block[b][0] == 0)
              fscanf(fp," // connections block\n");
           if (net_block[b][0] == 1)
              fscanf(fp," // block to be updated\n");
           if (net_block[b][0] == 2)
              fscanf(fp," // gain block\n");
           if (net_block[b][0] == 3)
              fscanf(fp," // modulated gain block\n");

     }
        fscanf(fp,"neurons bias, delta, gain, xy position, display\n");
        drawnxmax = 0;
        drawnymax = 0;
    for(n=0; n < nneurons; n++)
     {
           fscanf(fp,"%d %d %d %d %d %d\n", &neuronbias[n], &neurondelta[n], &neurongain[n], &neuronxy[n][0], &neuronxy[n][1], &neurondisplay[n]);
       if (drawnxmax < neuronxy[n][0])
         drawnxmax = neuronxy[n][0];
       if (drawnymax < neuronxy[n][1])
         drawnymax = neuronxy[n][1];
         }
        drawnxmax += 30;
        drawnymax += 30;
    if (mode == 1)
    {
         fscanf(fp,"FREE PARAMETERS %d\n", &np);
         i = 0;
         p = ncontrollers->freep;
         while (fgets(cbuffer,128,fp) != NULL && i < np)
          {
       sscanf(cbuffer,"%f",p);
       i++;
       p++;
          }
         }

    fclose(fp);

    sprintf(warnst,"loaded file %s", filename);
    display_stat_message(warnst);
    update_rendnetwork(0);
    return(1);
    }
      else
       {
    return(0);
       }
}

/*
 * load the free parameters (and the architecture description)
 * from the file evorobot.net, if present
 */
void 
load_controller_parameters()

{
     load_net_blocks("evorobot.net",1);
}

/*
 * Save the free parameters *p into a file .phe
 */
void
save_phenotype_data(char *filen, float *p, int np)

{
    int i;
    int t;
    int b;
    char  errorm[64];
    FILE *fp;
	
	QFileInfo *info =new QFileInfo(QString(filen));
	QString fn=info->fileName();
	delete info;

	//if(fn=="blank.phe") exit(1);
	
	display_error(fn.toAscii().data());

    if ((fp=fopen(filen, "w+")) == NULL) 
    {
    sprintf(errorm,"cannot open %s file",filen);
    display_error(errorm);
    }

    // biases
    for(i=0;i < nneurons;i++)
     {
       if (neuronbias[i] == 1)
       {
		   if(fn=="blank.phe")
			 fprintf(fp,"* \tbias %s\n", neuronl[i]);
		   else
         fprintf(fp,"%f \tbias %s\n",*p, neuronl[i]);
        
		 p++;
       }
     }
     // blocks
     for (b=0; b < net_nblocks; b++)
     {
       if (net_block[b][0] == 0)
       {
         for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
          {
           for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++)
           {
			   if(fn=="blank.phe")
				   fprintf(fp,"* \tweight %s from %s\n", neuronl[t], neuronl[i]);
			   else
        fprintf(fp,"%f \tweight %s from %s\n",*p, neuronl[t], neuronl[i]);
            p++;
           }
          }
       }
       if (net_block[b][0] == 1)
       {
         for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
          {
        if (neurondelta[t] == 1)
           {
                     if(fn=="blank.phe")
						 fprintf(fp,"* \ttimeconstant %s (%.2f)\n", neuronl[t],(fabs((double) *p) / wrange));
					 else
			   fprintf(fp,"%f \ttimeconstant %s (%.2f)\n", *p, neuronl[t],(fabs((double) *p) / wrange));
            p++;
           }  
         }
       }       
     } 
     
     fprintf(fp,"END\n");
     fflush(fp);           
     fclose(fp);
}

/*
 * Load the free parameters from a .phe file
 */
int
load_phenotype_data(char *filen, float *p, int np)

{
    int i;
    int t;
    int b;
    int iv;
    char  cbuffer[128];
    char  cbufferb[128];
    float *phe;
    FILE *fp;

    if ((fp=fopen(filen, "r")) == NULL) 
    {
    if (strcmp(filen, "robot.phe") != 0)
         {
      sprintf(cbuffer,"cannot open %s file",filen);
      display_error(cbuffer);
         }
    return(0);
    }

   i = 0;
   phe = phenotype;
   while (fgets(cbuffer,128,fp) != NULL && i < np)
     {
      if (*cbuffer == '*')
        *p = -999.0;
       else
        sscanf(cbuffer,"%f",p);
      *phe = *p;
      i++;
      p++;
      phe++;
     }
   pheno_loaded = 1;

   if (i < np)
      display_warning("the file contain an insufficient number of data");
   //fgets(cbuffer,128,fp);
   //if (*cbuffer != 'E')
   //   display_warning("the file does not contain the expected number of values");

   fclose(fp);
   return(1);

}




//-----------------------------------------------------------------
//------------ Private Functions ----------------------------------
//-----------------------------------------------------------------


/*
 * standard logistic function
 */
float
logistic(float f)

{
    
    return((float) (1.0 / (1.0 + exp(0.0 - f))));
 
}
//logistic with positive domain. Approximate y=0 when x=0
float
logisticpos(float f)

{
    
    return((float) (1.0 / (1.0 + exp(- f + 7.0))));
 
}
float
clocklogisticpos(float f)

{
    
    return((float) (1.0 / (1.0 + exp(- f + 4.0))));
 
}
float
foodlogisticpos(float f)

{
    
    return((float) (1.0 / (1.0 + exp(- f + 3.0))));
 
}


