/*
 * Evorobot* - environment.cpp
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
 * environment.cpp
 */

#include "public.h"
#include "environment.h"
#define MAXN 200


struct envobject **envobjs;    // environment, pointer to object lists
int    envnobjs[10];           // current number of objects for each category
int    nobjects = 50;          // max number of objects for each categorgy
int    envdx=0;                // environment dx
int    envdy=0;                // environment dy

int   random_groundareas=0; // randomize foodzone positions
int   random_sround=0;      // randomize sround positions
int   random_round=0;       // randomize round positions   

extern float robotradius;   //the radius of the robot's body 


int   nworlds=1;            // number of different environments     
int   amoebaepochs=0;       // number of epochs                        
long int sweeps = 0;        // number of step for each trial  
int   timelaps=1;           // timelaps x 100 milliseconds             
int   timeadjust=0;         // used to slowdown the update on the robots or the rendering in simulation       
int   grid=0;               // the step of the grid for the render of the environment
int   stop;                 // stop the test/adaptation (when 1)

// display
int  displaysensors=0;      // graphically display sensors
int  winrobotenvx = -1;     // x coordinate of robot-env window (-1 = undefined)
int  winrobotenvy = -1;     // y coordinate of robot-env window (-1 = undefined)
int  winrobotenvdx = 0;     // dx of controller robot-env window (0 = undefined)
int  winrobotenvdy = 0;     // dy of controller robot-env window (0 = undefined)
char neuronl[MAXN][8];        // neurons labels
char neuroncl[MAXN][8];       // neurons labels
int logData;


/*
 * create environmental parameters
 */
void
create_environment_par()

{

   create_iparameter("nworlds","lifetime",&nworlds, 1, 5, "number of environments");
   create_iparameter("ntrials","lifetime",&amoebaepochs, 0, 0, "number of trials");
   create_iparameter("ncycles","lifetime",&sweeps, 0, 0, "number of step for each trial");
   create_iparameter("timelaps","lifetime",&timelaps, 0, 0, "duration of a cycle in hundreds of milliseconds");
   create_iparameter("pause","lifetime",&timeadjust, 0, 1000, "length of the pause in each step (in hardware)");
   create_iparameter("grid","display",&grid, 0, 0, "grid step affecting the world editing interface");
   create_iparameter("envdx","display",&envdx, 0, 0, "dx of the environment");
   create_iparameter("envdy","display",&envdy, 0, 0, "dy of the environment");
   create_iparameter("displaysensors","display",&displaysensors, 0, 1, "graphically display sensors");
   create_iparameter("winrobotenvx", "display",&winrobotenvx, 0, 0, "x coordinate of robot-env window");
   create_iparameter("winrobotenvy", "display",&winrobotenvy, 0, 0, "y coordinate of robot-env window");
   create_iparameter("winrobotenvdx", "display",&winrobotenvdx, 0, 0, "dx of robot-env window");
   create_iparameter("winrobotenvdy", "display",&winrobotenvdy, 0, 0, "dy of robot-env window");
   create_iparameter("logData", "display",&logData, 0, 4, "enable logging position and network activation for each trial(1) or generation(2)");
  


}

/*
 * create environmental parameters
 */
void
init_environment()

{

    int i;
    struct  envobject **eobj;


    envobjs = (struct envobject **) malloc(NOBJS * sizeof(struct envobject));
    for (i=0, eobj = envobjs; i < NOBJS; i++, eobj++)
       {
        *eobj = (struct envobject *) malloc(nobjects * sizeof(struct envobject));
        envnobjs[i] = 0;
       }
    load_world("evorobot.env");

}

/*
 * it load worlds description from file evorobot.env
 */
void 
load_world(char *filename)

{


     FILE   *fp;
     char   st[1024];
     char   message[512];
     char   word[256];
     char   *w;
     int    n;
     int    type;
     char   c;
     int    i;
     int    color;
     int    selected;
     struct envobject **cenvobjs;
     struct envobject *obj;

     cenvobjs = envobjs;
     for(i=0; i < NOBJS; i++)
       envnobjs[i] = 0;
     if ((fp=fopen(filename, "r")) != NULL)
    {
     n = 0;
     while (fgets(st,1024,fp) != NULL && n < 1000)
     {
      selected = 0;
      if (strlen(st) > 2)
       {
       getword(word,st,' ');
       if (strcmp(word, "swall") == 0 && envnobjs[SWALL] < nobjects)
        {
         selected = 1;
         obj = *cenvobjs;
         obj = (obj + envnobjs[SWALL]);
         obj->id = envnobjs[SWALL];
             envnobjs[SWALL] += 1;
         obj->type = SWALL;
        }
       if (strcmp(word, "sround") == 0 && envnobjs[SROUND] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + SROUND);
         obj = (obj + envnobjs[SROUND]);
         obj->id = envnobjs[SROUND];
             envnobjs[SROUND] += 1;
         obj->type = SROUND;
         obj->r = 12.5;
        }
       if (strcmp(word, "round") == 0 && envnobjs[ROUND] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + ROUND);
         obj = (obj + envnobjs[ROUND]);
         obj->id = envnobjs[ROUND];
             envnobjs[ROUND] += 1;
         obj->type = ROUND;
         obj->r = 27.0;
        }
       if (strcmp(word, "ground") == 0 && envnobjs[GROUND] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + GROUND);
         obj = (obj + envnobjs[GROUND]);
         obj->id = envnobjs[GROUND];
             envnobjs[GROUND] += 1;
         obj->type = GROUND;
         obj->subtype = 0;
        }
       if (strcmp(word, "light") == 0 && envnobjs[LIGHT] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + LIGHT);
         obj = (obj + envnobjs[LIGHT]);
         obj->id = envnobjs[LIGHT];
             envnobjs[LIGHT] += 1;
         obj->type = LIGHT;
        }
       if (strcmp(word, "robot") == 0 && envnobjs[ROBOT] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + ROBOT);
         obj = (obj + envnobjs[ROBOT]);
         obj->id = envnobjs[ROBOT];
             envnobjs[ROBOT] += 1;
         obj->type = ROBOT;
         obj->r = robotradius;
        }
#ifdef EVOGRAPHICS 
       if (strcmp(word, "image") == 0 && envnobjs[IMAGE] < nobjects)
        {
         selected = 1;
         obj = *(cenvobjs + IMAGE);
         obj = (obj + envnobjs[IMAGE]);
         obj->id = envnobjs[IMAGE];
             envnobjs[IMAGE] += 1;
         obj->type = IMAGE;
        }
#endif  
       if (selected == 1)
            {
          color = 0;
          obj->subtype = 0;
          obj->x = 100;
          obj->y = 100;
          obj->X = 200;
          obj->Y = 200;
          if (obj->type != 1 && obj->type != 2)
            obj->r = 0;
          obj->h = 20;
          obj->c[0] = 0.0;
          obj->c[1] = 0.0;
          obj->c[2] = 0.0;
              while (strlen(st) > 1)
               {
            getword(word,st,' ');
        c = *word;
        w = word;
        w++;
            switch(c)
             {
               case 't':
            obj->subtype = atoi(w);
                break;
               case 'x':
            obj->x = atof(w);
            if (obj->x > envdx)
              envdx = obj->x;
                break;
               case 'y':
            obj->y = atof(w);
            if (obj->y > envdy)
              envdy = obj->y;
                break;
               case 'X':
            obj->X = atof(w);
            if (obj->X > envdx)
              envdx = obj->X;
                break;
               case 'Y':
            obj->Y = atof(w);
            if (obj->Y > envdy)
              envdy = obj->Y;
                break;
               case 'h':
            obj->h = atof(w);
                break;
               case 'r':
            obj->r = atof(w);
                break;
               case 'c':
            if (color >= 0 && color < 3)
              obj->c[color] = atof(w);
            color++;
               case 'f':
               load_image(obj,w);
                break;
           default:
            sprintf(message,"unknown env. parameter %c%s in file evorobot.env",c,w);
                    display_warning(message);
            break;
             }
           }
        }
       else
            {
         sprintf(st,"unknown object: %s", word);
         display_warning(st);
        }
       n++;
       }
      }
        fclose(fp);
        sprintf(message,"loaded %d objects from file: evorobot.env", n);
        display_stat_message(message);
        }
       else
       {
         sprintf(message,"Error: the file evorobot.env could not be opended");
     display_error(message);
       }
     // calculate world size (if not specified manually)
     if (envdx == 0 || envdy == 0)
       {
        for(i=0,obj = *envobjs; i < envnobjs[SWALL]; i++, obj++)
          {
        if (obj->x > envdx) envdx = obj->x;
        if (obj->y > envdy) envdy = obj->y;
        if (obj->X > envdx) envdx = obj->X;
        if (obj->Y > envdy) envdy = obj->Y;
          } 

      } 
     if (envnobjs[IMAGE] > 0)
      {
        obj = *(envobjs + IMAGE);
        if (obj->iw > envdx) envdx = obj->iw;
        if (obj->ih > envdy) envdy = obj->ih;
      } 

}


/*
 * save the environment description into a file evorobot.env
 */
void 
save_world(char *filename)

{


     FILE   *fp;
     int    i;
     int    ii;
     struct envobject **cenvobjs;
     struct envobject *obj;

     if ((fp=fopen(filename, "w")) != NULL)
      {
       cenvobjs = envobjs;
       for (i=0; i < NOBJS; i++)       
       {
        for (ii=0, obj = *(cenvobjs + i); ii < envnobjs[i]; ii++, obj++)
        {
          if (obj->type == SWALL)
       fprintf(fp, "swall x%.1f y%1.f X%.1f Y%.1f h%.1f c%.1f c%.1f c%.1f\n", obj->x, obj->y, obj->X, obj->Y, obj->h, obj->c[0], obj->c[1], obj->c[2]); 
          if (obj->type == SROUND)
       fprintf(fp, "sround x%.1f y%1.f r%.1f h%.1f c%.1f c%.1f c%.1f\n", obj->x, obj->y, obj->r, obj->h, obj->c[0], obj->c[1], obj->c[2]); 
          if (obj->type == ROUND)
       fprintf(fp, "round x%.1f y%1.f r%.1f h%.1f c%.1f c%.1f c%.1f\n", obj->x, obj->y, obj->r, obj->h, obj->c[0], obj->c[1], obj->c[2]);
          if (obj->type == GROUND)
       fprintf(fp, "ground t%d x%.1f y%1.f r%.1f c%.1f\n", obj->subtype, obj->x, obj->y, obj->r, obj->c[0]);
          if (obj->type == LIGHT)
       fprintf(fp, "light x%.1f y%1.f r%.1f h%.1f\n", obj->x, obj->y, obj->r, obj->h);
          if (obj->type == ROBOT)
       fprintf(fp, "robot c%.1f c%.1f c%.1f\n", obj->c[0], obj->c[1], obj->c[2]);
          //if (obj->type == WALL)
       //fprintf(fp, "wall x%.1f y%1.f X%.1f Y%.1f h%.1f c%.1f c%.1f c%.1f\n", obj->x, obj->y, obj->X, obj->Y, obj->h, obj->c[0], obj->c[1], obj->c[2]); 
          //if (obj->type == CYLINDER)
       //fprintf(fp, "cylinder x%.1f y%1.f r%.1f h%.1f c%.1f c%.1f c%.1f\n", obj->x, obj->y, obj->r, obj->h, obj->c[0], obj->c[1], obj->c[2]); 
        }
       }
       display_stat_message("saved current environment in file evorobot.env");
       fclose(fp);
      }
     else
      {
       display_warning("unable to open file evorobot.env");
      }

}

