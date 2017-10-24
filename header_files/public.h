/* 
 * Evorobot* - public.h
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
#ifndef PUBLIC_H
#define PUBLIC_H

#include "mode.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>  // malloc()
#include <string.h>  // strlen() strcmp()
#include "math.h"    // sqrt(), fabs(), sin(), cos(), exp()
#include "musclepair.h"
#define MAXN 200



// DEBUG:
// controllare in main.cpp e in simulato.cpp lo switch per il loading dei file .sam Windows/Unix

// mainwindow
void display_error(char *emessage);
void display_warning(char *emessage);
void display_stat_message(char *emessage);
void display_message(char *emessage);
void update_events();
#ifdef EVOGRAPHICS
class QGridLayout; 
#endif 
extern int userbreak;                // Whether the user press the stop buttom   

// parameters
extern void create_iparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void create_fparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void create_cparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void create_ivparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void create_fvparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void create_fvpparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description);
extern void set_iparameter(char *name, char *folder, int fvalue);
extern void set_fparameter(char *name, char *folder, float fvalue);
extern void set_cparameter(char *name, char *folder, char *string);
extern void set_fvparameter(char *name, char *folder, int index, float fvalue);
extern void set_fvpparameter(char *name, char *folder, int index, float fvalue);
extern void loadparameters();
extern int  getword(char *word, char *line, char stop);

// rend_param
#ifdef EVOGRAPHICS
class RendParam;                     // The param widget class
extern RendParam *rendParam;         // The param widget
void init_rendparam();               // initialize the widget (mode=1) and also the dialog box (mode=2)
void hide_rendparam();               // hide the dialog box
void update_rendparam();             // update the widget
void show_text(char *message);       // show text in the parameter window
#endif
extern int userlevel;                // the user-level (0=beginner, 5=expert)

// evolution
void create_evolution_par();         // create evolution parameters
void init_evolution();               // initialize the evolution module
void runevolution();                 // run the evolutionary process
void runadapt();                     // run the adaptive process
void test(int mode);                 // test individuals team
extern int nteam;		     // number of robots situated in the environment
extern int testindividual_is_running;// one individual is currently tested
extern int evolution_is_running;     // evolution is currently running
void randomizefreep();               // randomize the free parameters of the population and of the current individual/team

// rend_evolution
#ifdef EVOGRAPHICS
class RendEvolution;                 // The evolution widget class
extern RendEvolution *rendEvolution; // The evolution widget
void init_rendevolution(int mode);   // initialize the widget
void hide_rendevolution();           // hide the dialog box
void update_rendevolution(int mode); // update the widget
void add_evolution_widget(QGridLayout *mainLayout, int row, int column);  // add the evolution widget to the main window layout
#endif

// ncontroller
void create_network_par();
void init_network();
void ncontroller_read_nfreep(int *numfreep);
void genotype_to_phenotype(int i, int *geno, int nbits, int homogeneous);
void phenotype_to_genotype(int i, int *geno, int nbits, int homogeneous);
float *update_ncontroller(int id, float *input);
void reset_controller(int id);
int compute_nfreep(int nneurons);
void save_phenotype_data(char *filen, float *p, int np);
int load_phenotype_data(char *filen, float *p, int np);
void load_controller_parameters();

// bcontroller
void update_bcontroller(struct individual *pind, struct  iparameters *pipar);

// rend_network
#ifdef EVOGRAPHICS
class RendNetwork;                   // The network widget class
extern RendNetwork *rendNetwork;     // The network widget
void init_rendnetwork(int mode);     // initialize the widget (mode=1) and also the dialog box (mode=2)
void hide_rendnetwork();             // hide the dialog box
void update_rendnetwork(int cycle);  // update the widget
void update_rendcontrolstate(int cycle);  // update the widget
void add_network_widget(QGridLayout *mainLayout, int row, int column);  // add the network widget to the main window layout
void add_controlstate_widget(QGridLayout *mainLayout, int row, int column);  // add the network widget to the main window layout
#endif

// rend_robot
#ifdef EVOGRAPHICS
class RendRobot;                     // The robot-environmeny widget class
extern RendRobot *rendRobot;         // The robot-environment widget
void init_rendrobot(int mode);       // initializa the widget (mode=1) and also the dialog box (mode=2)
void hide_rendrobot();               // hide the dialog box
void update_rendrobot();             // update the widget
void add_robot_widget(QGridLayout *mainLayout, int row, int column);  // add the robot widget to the main window layout
void read_robot_units(int i, int *nsensors, int *ninternals, int *nmotors); // return the number of units for individual i
#endif

// environment
void create_environment_par();       // create environmental parameters
void init_environment();             // initialize inveronment

// rend_robot3d
#ifdef EVOGRAPHICS
#ifdef EVO3DGRAPHICS 
void update_rend3drobot();            // update 3d render of robot and environment
void init_rend3drobot();              // initializa the 3DViewer
#endif
#endif

// robot-env
extern void create_epuck_par();
extern void init_epuck();
extern double evaluate_epuck(double *varfit);
extern void initialize_robot_position();
extern void (*pinit_robot_pos)();
extern int amoebaepochs;                   

// special 
extern float rans(double n);      // returns a random floating point number in the range [-n, +n]
extern int mrand(int i);          // returns a random integer in the range [0, i-1]
void set_seed(int s);             // initializes the random number generator with a seed
void pause(int n);                // wait for n milliseconds
double mdist(float x, float y, float x1, float y1); // return the distance between two points
double gaussian(double value, double sigma); 

// public global variables
extern char neuronl[MAXN][8];       // neurons labels (created in the robot_env module and used by the rend_network module)
extern char neuroncl[MAXN][8];      // neurons current labels (updated by pseudo_activate_net())
extern int  cindividual;          // current individual of the team      
extern int  grid;                 // the step of the grid (if any)

#endif





