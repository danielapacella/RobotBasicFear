/* 
 * Evorobot* - robot-env.h
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

#ifdef EVOGRAPHICS // QImage required for image environmental objects
#include <QImage>
#endif


struct envobject

{

int type;                   // type of the object (1=wall,2=cylinder,3=ground,4=light,5=landmark,6=image)
int subtype;                // subclass (cylinder 1=r12.5,2=r25, ground 1=circle,2=square, image0=grey-level image1=rgb)
int id;                     // the id of the object
float x,y;                  // x and y coordinate
float X,Y;                  // x and y end coordinate  
float r;                    // radius
float h;                    // height
float c[3];                 // rgb color 
float *i;                   // pointer to float image
int   iw;                   // image width
int   ih;                   // image height
int   checked;				// to be use in particular task
int   visited;
#ifdef EVOGRAPHICS 
QImage *qi;                 // pointer to Qimage 
#endif
};

extern struct envobject **envobjs;    // environment, pointer to object lists
extern int    envnobjs[];             // number of objects for each type
extern int    nobjects;               // max number of objects for each categorgy
extern int    envdx;                  // environment dx
extern int    envdy;                  // environment dy
#define NOBJS    8
#define SWALL    0
#define SROUND   1
#define ROUND    2
#define GROUND   3
#define LIGHT    4
#define ROBOT    5
#define IMAGE    6

extern void   load_world(char *filename);
extern void   save_world(char *filename);
extern void   load_image(struct envobject *obj, char *filename);

extern int   nworlds;              // number of different environments     
extern int   amoebaepochs;         // number of epochs                        
extern long int sweeps;            // number of step for each trial  
extern int   timelaps;             // timelaps x 100 milliseconds             
extern int   timeadjust;           // used to slowdown the update on the robots or the rendering in simulation       
extern int   stop;                 // stop the test/adaptation (when 1)

extern int   random_groundareas;   // randomize foodzone positions
extern int   random_sround;        // randomize sround positions
extern int   random_round;         // randomize round positions   
extern int   grid;                 // the step of the grid for the render of the environment

extern int  displaysensors;        // graphically display sensors
extern int  winrobotenvx;          // x coordinate of robot-env window (-1 = undefined)
extern int  winrobotenvy;          // y coordinate of robot-env window (-1 = undefined)
extern int  winrobotenvdx;         // dx of controller robot-env window (0 = undefined)
extern int  winrobotenvdy;         // dy of controller robot-env window (0 = undefined)
extern int  logData;			   // when 1 during test we log robot position and network data (sensors hidden and motors)