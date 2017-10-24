/* 
 * Evorobot* - ncontroller.h
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

#define MAXN 200   // maximum number of neurons

void save_net_blocks(char *filename);
float logistic(float f);
float logisticpos(float f);
float clocklogisticpos(float f);
float foodlogisticpos(float f);
void create_net_block(struct ncontroller *cncontroller);
int load_net_blocks(char *filename, int mode);

extern int  drawneuronsdx;         //  width of windows area for displaying neurons state
extern int  drawnstyle;            //  style of draws (1=bars, 2=curves) 
extern int ninputs;                // n. input units                          
extern int nhiddens;               // n. hidden units                    
extern int noutputs;               // n. output units
extern int nneurons;               // total n. of neurons
extern int net_nblocks;            // network number of connection blocks
extern int net_block[MAXN][5];     // blocks: 0=type (w,b,d), from 1 per 2 receive from 3 per 4
extern int neuronbias[MAXN];       // whether neurons have bias
extern int neurondelta[MAXN];      // whether neurons have time constants
extern int neurongain[MAXN];       // whether neurons have a gain
extern int neuronxy[MAXN][2];      // neurons xy position in display space
extern int neurondisplay[MAXN];    // whether neurons should be displayed or not
extern float *neuronlesion;        // whether neurons are lesioned 
extern int neuronlesions;          // whether we have at least one neuron leasioned
extern int drawn_alltogether;      // whether we superimpose the state of different individuals on the same graph
extern float wrange;               // weights and biases range                 
extern float grange;               // range of gain parameters                 
extern int drawnxmax;              // max display-x position
extern int drawnymax;              // max display-y position
extern int wincontrollerx;         // x coordinate of controller windows (0 = undefined)
extern int wincontrollery;         // y coordinate of controller windows (0 = undefined)
extern int wincontrollerdx;        // dx of controller windows (0 = undefined)
extern int wincontrollerdy;        // dy of controller windows (0 = undefined)

struct ncontroller

{

  int   nfreep;                                 // number of free parameters
  float	*freep;				        // the free parameters                      
  int   ninputs;                                // n. input units                           
  int   nhiddens;                               // n. hidden units                          
  int   noutputs;                               // n. output units                          
  int   nneurons;                               // total n. of neurons                      
  float *activation;                            // activation state of the neurons          
};

extern struct ncontroller *ncontrollers;        // pointer to neural controllers                
