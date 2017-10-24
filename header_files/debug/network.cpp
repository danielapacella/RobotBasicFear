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

/*
 * activate.c
 */

#include "defs.h"
#include "public.h"

int ninputs;                // n. input units                          
int nhiddens;               // n. hidden units                    
int noutputs;               // n. output units
int nneurons;               // total n. of neurons
int net_nblocks;            // network number of connection blocks
int net_block[40][5];       // blocks: 0=type (w,b,d), from 1 per 2 receive from 3 per 4
int neuronbias[40];         // whether neurons have bias
int neurondelta[40];        // whether neurons have time constants
int neuronxy[40][2];        // neurons xy position in display space
int neurondisplay[40];      // whether neurons should be displayed or not
int neuronlesion[40];       // whether neurons are lesioned 
int neuronlesions=0;        // whether we have at least one neuron leasioned
/*
 * Update the network
 */
void
activate_net(float *p, int nneurons, float *activation, int nsensors, float *sensors)

{
    int i;
    int t;
    int b;
    float delta;
    float netinput[40]; 

    // biases
    for(i=0;i < nneurons;i++)
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
		netinput[t] += activation[i] * *p;
	        p++;
	      }
	     
	   } 
	   // update block
	   if (net_block[b][0] == 1)
	   {
	     for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
	      {
		if (t < nsensors)
		{
		  if (neurondelta[t] == 0)
		   {
		    activation[t] = sensors[t];
		   }
		   else
		   {
		    delta = (float) (fabs((double) *p) / wrange);
		    p++;
                    activation[t] = (activation[t] * delta)  + (sensors[t] * (1.0f - delta));
		   }
		}
		else
		{
		  if (neurondelta[t] == 0)
		   {
		    activation[t] = logistic(netinput[t]);
		    delta = 0;
		   }
		   else
		   {
		    delta = (float) (fabs((double) *p) / wrange);
		    p++;
                    activation[t] = (activation[t] * delta)  + (logistic(netinput[t]) * (1.0f - delta));
		   }
		  
		}
		if (neuronlesion[t] == 1)
                  activation[t] = 0.0f;
	      }
	   }
	 }

}

/*
 * set to 0.0 the activation value of all units
 */
void
reset_net(int nneurons, float *activation, int nsensors, float *sensors)
{


  int    i;

  for(i=0; i < nsensors; i++)
	sensors[i] = 0.0f;
  for(i=0; i < nneurons; i++)
	activation[i] = 0.0f;

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
    int   updated[40]; 
    int   ng;
    int   warning;
    int   mode;

    // if mode == 0, free parameters has not been created yet
    // therefore we do not read them
    //mode = ddelta + dweight + dbias;

    ng  = 0;
    warning = 0;
    for(i=0;i < nneurons;i++)
      updated[i] = 0;
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
            sprintf(drawdata_st,"neuron %d will never be activated according to the current architecture", i);
	    display_warning(drawdata_st);
	    warning++;
	  }
          if (updated[i] > 1 && warning == 0)
	  {
            sprintf(drawdata_st,"neuron %d will be activated more than once according to the current architecture", i);
	    display_warning(drawdata_st);
	    warning++;
	  }
	}
return(ng);

}


/*
 * create the block structure for a standard architecture
 * create x-y coordinate for network architecture display
 * store in local variable the number of neurons
 */
void
create_net_blocks(int ninp, int nout, int nn, int dyndeltai, int dyndeltah, int dyndeltao, int dynioconn, int dynrechid, int dynrecout)
{

    int n;
    int i;
    int ng;
    int startx;
    int dx;
    int team;

    ninputs = ninp;
    noutputs = nout;
    nneurons = nn;
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
}

/*
 * save the description of a network architecture in blocks
 */
void
save_net_blocks(char *filename)


{

   FILE	   *fp;
   int     b;
   int     n;

   if ((fp = fopen(filename,"w")) != NULL)
	{
         fprintf(fp,"nneurons %d\n", nneurons);
         fprintf(fp,"nsensors %d\n", ninputs);
         fprintf(fp,"nmotors %d\n", noutputs);
         fprintf(fp,"nblocks %d\n", net_nblocks);
	 for (b=0; b < net_nblocks; b++)
	 {
           fprintf(fp,"%d  %d %d %d %d", net_block[b][0],net_block[b][1],net_block[b][2],net_block[b][3],net_block[b][4]);
           if (net_block[b][0] == 0)
              fprintf(fp," // connections block\n");
           if (net_block[b][0] == 1)
              fprintf(fp," // block to be updated\n");
	 }

        fprintf(fp,"neurons xy coordinate and display mode\n");
	for(n=0; n < nneurons; n++)
           fprintf(fp,"%d %d %d %d %d\n", neuronbias[n], neurondelta[n], neuronxy[n][0], neuronxy[n][1], neurondisplay[n]);

	sprintf(drawdata_st, "network architecture data saved on file %s", filename);
	display_stat_message(drawdata_st);
	}
      else
       {
	  display_warning("unable to create the file evorobot.net");
       }
   fclose(fp);
}


/*
 * load the description of a network architecture in blocks
 */
int
load_net_blocks(char *filename)


{

   FILE	   *fp;
   int     b;
   int     n;
   int     team;

   if ((fp = fopen(filename,"r")) != NULL)
	{
         fscanf(fp,"nneurons %d\n", &nneurons);
         fscanf(fp,"nsensors %d\n", &ninputs);
         fscanf(fp,"nmotors %d\n", &noutputs);
         nhiddens = nneurons - (ninputs + noutputs);
         fscanf(fp,"nblocks %d\n", &net_nblocks);
	 for (b=0; b < net_nblocks; b++)
	 {
           fscanf(fp,"%d  %d %d %d %d", &net_block[b][0],&net_block[b][1],&net_block[b][2],&net_block[b][3],&net_block[b][4]);
           if (net_block[b][0] == 0)
              fscanf(fp," // connections block\n");
           if (net_block[b][0] == 1)
              fscanf(fp," // block to be updated\n");
	 }
        fscanf(fp,"neurons xy coordinate and display mode\n");
	for(n=0; n < nneurons; n++)
           fscanf(fp,"%d %d %d %d %d\n", &neuronbias[n], &neurondelta[n], &neuronxy[n][0], &neuronxy[n][1], &neurondisplay[n]);
	fclose(fp);

	sprintf(drawdata_st,"loaded file %s", filename);
	display_stat_message(drawdata_st);
	update_rendnetwork();
	return(1);
	}
      else
       {
	return(0);
       }
}

/*
 * Save the free parameters *p into a file .phe
 */
void
save_phenotype_data(char *filen, float *p)

{
    int i;
    int t;
    int b;
    char  errorm[64];
    FILE *fp;

    if ((fp=fopen(filen, "w+")) == NULL) 
    {
	sprintf(errorm,"cannot open %s file",filen);
	display_error(errorm);
    }

    // n. neurons
    fprintf(fp,"nneurons %d\n", nneurons);
    // biases
    fprintf(fp,"biases\n");
    for(i=0;i < nneurons;i++)
	 {
	   if (neuronbias[i] == 1)
	   {
	     fprintf(fp,"%f ",*p);
	     p++;
	   }
	    else
	   {
	     fprintf(fp,"0.0 ");
	   }
	 }
     fprintf(fp,"\n");
     // blocks
     for (b=0; b < net_nblocks; b++)
	 {
	   if (net_block[b][0] == 0)
	   {
	     fprintf(fp,"block %d %d %d %d\n", net_block[b][1], net_block[b][2], net_block[b][3], net_block[b][4]);
	     for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
	      {
	       for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++)
	       {
		fprintf(fp,"%f ",*p);
	        p++;
	       }
	       fprintf(fp,"\n");
	      }
	   }
	   if (net_block[b][0] == 1)
	   {
             fprintf(fp,"delta %d %d\n", net_block[b][1], net_block[b][2]);
	     for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
	      {
		if (neurondelta[t] == 0)
		   {
                    fprintf(fp,"0.0 ");
		   }
		   else
		   {
                    fprintf(fp,"%f ", *p);
		    p++;
		   }  
	     }
             fprintf(fp,"\n");
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
load_phenotype_data(char *filen, float *p)

{
    int i;
    int t;
    int b;
    int iv;
    float fv;
    char  errorm[64];
    FILE *fp;

    if ((fp=fopen(filen, "r")) == NULL) 
    {
	sprintf(errorm,"cannot open %s file",filen);
	display_error(errorm);
	return(0);
    }

    // n. neurons
    fscanf(fp,"nneurons %d\n", &iv);
    // biases 
    fscanf(fp,"biases\n");
    for(i=0;i < ind->nneurons;i++)
	 {
	   if (neuronbias[i] == 1)
	   {
	     fscanf(fp,"%f ",p);
	     p++;
	   }
	    else
	   {
	     fscanf(fp,"0.0 ");
	   }
	 }
     fscanf(fp,"\n");

     // blocks
     for (b=0; b < net_nblocks; b++)
	 {
	   if (net_block[b][0] == 0)
	   {
	     fscanf(fp,"block %d %d %d %d\n", &iv, &iv, &iv, &iv);
	     for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++)
	      {
	       for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++)
	       {
		fscanf(fp,"%f ",p);
	        p++;
	       }
	       fscanf(fp,"\n");
	      }
	   }
	   if (net_block[b][0] == 1)
	   {
             fscanf(fp,"delta %d %d\n", &iv, &iv);
	     for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++)
	      {
		if (neurondelta[t] == 0)
		   {
                    fscanf(fp,"0.0 ");
		   }
		   else
		   {
                    fscanf(fp,"%f ", p);
		    p++;
		   }  
	     }
             fscanf(fp,"\n");
	   }	   
	 } 
	 
     fscanf(fp,"%s\n",errorm);	       
     fclose(fp);

     if (strcmp(errorm, "END") != 0)
	 {
     	  display_error("the file.phe is inconsistent with respect to current parameters");
	  return(0);
         }

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


