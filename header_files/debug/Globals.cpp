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
 * globals.c
 */

#include "defs.h"
#include "public.h"
/*
 * parameters
 */
int	real;
int     testindividual_is_running=0; // one individual is currently tested
int     evolution_is_running=0;      // evolution is currently running
int	seed=0;                /* random seed                              */
int     amoebaepochs=0;        /*  number of epochs                        */
int     nteam = 4;	       // numero di individui per team: cacciatori		
int     nprey = 0;	       // numero di prede - TO BE DELETED	
int     generations=0;         /* number of generations                    */
float   wrange=5.0;            /* weights and biases range                 */
int     nkids=0;               /* number of kids for father                */
int     bestfathers=0;         /* number of fathers that make kids         */
int     startgeneration = 0;   /* start generation (>=0 means load weig.)  */
int     percmut=2;             /* percentage of mutations                  */
int     elitism=0;             /* one copy of each best is preserved intact, if 2 we save previous fitness data */
int     savebest=1;            /* save only the best N individual each gen.   */
int     saveifit=0;            /* save individuals' fitness                */
float   sensornoise=(float)0.0;/* noise on infrared sensors                */
int     testsamep=0;           /* test, start from the same position       */
int     timelaps=1;            /*  timelaps x 100 milliseconds             */
int     timeadjust=0;          /*  overtime due to computation time        */
int     nobreak=0;             /*  no artificial break on epochs           */
int     dindividual=0;         /*  current individual of the population    */
int     cindividual=0;         /*  current individual of the team          */
int     masterturnament=0;     /*  master turnament                        */
int     stop_when_crash=1;     /*  we stop lifetime if a crash occurred    */
int     nworlds=1;             /*  number of different environments        */
int     cworld=0;              /*  idn of current world			   */    
int     random_foodzones=0;    /*  randomize foodzone positions            */
int     random_sround=0;       /*  randomize sround positions              */
int     random_round=0;        /*  randomize round positions               */
float   background_color=(float)0.0;  /*  the background color                    */
int     renderdx=1200;         //  width of the render area
int     renderdy=768;          //  height of the render area 
int     drawneurons=0;         /*  display sensors activation              */
int     drawnstyle=0;          /*  style of draws (1=bars, 2=curves)       */
int     drawn_alltogether=0;   /*  draw individuals' value on the same graph */  
int     drawneuronsdx=500;     //  width of windows area for displaying neurons state  
int     draw_window1_width=300; // width of the portion of the window used to display the world
int     drawindividual = 0;    /* display the individual and the environment */
int     drawnetwork=0;         /* draw network architecture                  */
int     drawevolution=0;       // drawevolution rendering switch
int     drawtrace=0;           /* draw the trajectory trace                  */ 
int     drawsample=0;          // draw sample data
int     verbose=2;             // verbose level (0-1-2)
int     nodisplaydata=0;       // skip graphic display of help data
float   max_dist_attivazione = 0.0f;  //range di attivazione del sensore other
int     ninput_other = 0;
int     play_sound=0;          // controlla la presenza dell'audio
int     console=0;             // print statistics on the console
int     drawstat=0;            // draw statistics (1=current pop,2=all pop) 
float   scale;                 // scale of world display
int     mousex=4000;           // mouse left-clicked x position
int     mousey=4000;           // mouse left-clicked y position
int     manual_start=0;        // robots' position has been set by the user
int     pop_seed=0;            // whether seed is reinitialized for individuals of the same generation
int     grid=0;                // the step of the gred (if any)
int     pheno_loaded=0;        // whether phenotype parameters have been loaded
long int learningc=0;          // learning cycles
int     homogeneous=1;         // whether the team is formed by huniform individuals or not
int     startfarfrom=0;        // the robot start far from target areas and round obstacles
/*
 * the population and the individual phenotype
 * and the environments
 */

struct  iparameters *ipar=NULL; /* individual parameters                  */
struct  individual *ind=NULL;   /* individual phenotype                   */
int	**genome;		/* genome of the population               */
int	**bestgenome;		/* genome of the best individuals         */
struct  environment env[5];     /* environments                           */
float	*phenotype;		/* phenotype's fixed parameters           */

/*
 * others
 */

int     nindividuals;           /* number of individuals                  */
int     npopulations=1;         /* number of populations                  */
long int sweeps = 0;            /* number of life sweeps                  */
long int run = 0;               /* current cycle                          */
double  tfitness[2][MAXINDIVIDUALS];/*  populations fitness                   */
double  pfitness[MAXINDIVIDUALS]; // fitness gathered in previous generations
int     plifes[MAXINDIVIDUALS];   // previous life periods in which fitness was gathered 
double  bpfitness[MAXINDIVIDUALS];// best individuals - fitness gathered in previous generations
int     bplifes[MAXINDIVIDUALS];  // best individuals - previous life periods in which fitness was gathered 
double  statfit[MAXGENERATIONS][4]; /*  best and mean fit for each gen. & pop */  
double  statfit_min;            // minimum fitness
double  statfit_max;            // maximum fitness
int     statfit_n;              // n fitness data
double  astatfit[MAXGENERATIONS][4];/*  statfit averaged for replications */
float   ave_mot;                /* average motors                         */
float   dif_m;                  /* differential motors                    */
float   maxs;                   /* max activated sensor                   */
int     oselected;              /* selected output                        */
int     individual_crash;       /* a crash occurred between two individuals */
int     additional_ind=0;       /* n. additional genome to be allocated */
int     cwindow=0;              /* current window                          */
int     cwindowdx=0;            /* current dx dimension of the window      */
int     cwindowdy=0;            /* current dy dimension of the window      */
int     userbreak=0;            /* user want to stop what's going on       */
int     cepoch=0;                 /* current epoch                           */
int     testindividual=0;       /* test one individual                     */
int     run_evolution=0;        /* run evolution                           */
int     run_masterturnament=0;  /* run masterturnament                     */
int     nreplications=0;        /* n replication seeds                     */
int     fitmode=0;              /* 1=fitness/lifecycles 2=fitness/epochs   */
char    drawdata_st[512];       /* the string to be displayed as draw data */ 
float   trace[1000][2][10];     // trace data to be displayed for 10 robots and 1000 cycles				
char    helpt[40][255];         // parameters list to be displayed on the main window
int     helptn=0;               // number of lines to be displayed on the main window
char    ffitness_descp[255];    // description of the current fitness function
char    filename[2048];         // filename

/*
 * simulato.c
 */
float **motor;              /* 12 rows each with 12*2(ang and dist) columnsi*/
int   **wall;               /* wall (180 degrees and 38 distances)          */
int   **obst;               /* round obstacle (180 d. and 38 distances      */
int   **sobst;              /* round obstacle (180 d. and 20 distances      */
int   **light;              /* light (180 degrees and 27 distances)         */
int   *wallcf;              /* wall  configuration                          */
int   *obstcf;              /* obst  configuration                          */
int   *sobstcf;             /* sobst configuration                          */
int   *lightcf;             /* light configuration                          */
double  P90,P180,P360;      // Pgreco constants
