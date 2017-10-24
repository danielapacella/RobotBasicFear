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

extern struct iparameters *ipar; // individual parameters 
extern int real;           // whether we use real or simulated robots
extern long int run;       // current step of the trial
extern int manual_start;   // whether robots' position has been manually set by the user
extern float robotradius;  // the radius of the robot's body (KHEPERA RADIUS: 27.5   EPUCK RADIUS: 37.5)
extern float sensornoise;  // noise on sensors
extern float max_dist_attivazione;  //distanza massima percezione segnali
extern int  individual_crash;   // a crash occurred between two individuals
extern char ffitness_descp[255];// description of the current fitness function
extern int  robottype;     // robot type 0=khepera, 1=e-puck, 2=lego 

void set_scale();
void remove_object(char *nobject, int idobject);
extern void connect_disconnect_robots(); 
extern void initialize_robot_position();
extern void setEyeTarget(int t);//if t<0 there is no target t >=0 the tth sround is selected as target
extern void setEye2Target(int t1, int t2); 
extern void initialize_world();
extern void update_motors(struct individual *pind, struct  iparameters *pipar, float *act);

// simulator.cpp
extern double RADtoDGR(double r);
extern double DGRtoRAD(double d);
extern float  SetX(float x,float d,double angle);
extern float  SetY(float y,float d,double angle);
extern double mang(int x, int y, int x1, int y1, double dist);
extern double mangrel(double absolute,double kepera);
extern double mangrel2(double absolute, double kepera);
extern int    read_ground(struct individual *pind);
extern float  read_ground_color(struct individual *pind);
extern int    check_crash(float p1,float p2);
extern void   reset_pos_from_crash(struct individual *pind);
extern void   update_infrared_s(struct individual *pind);
extern void   update_ginfrared_s(struct individual *pind, struct  iparameters *pipar);
extern void   update_lights_s(struct individual *pind);
extern void   update_glights_s(struct individual *pind, struct  iparameters *pipar);
extern void   update_signal_s(struct individual *pind, struct  iparameters *pipar);
extern void   update_camera(struct individual *pind, struct  iparameters *pipar);
extern void   update_simplecamera(struct individual *pind, struct  iparameters *pipar);
extern void   update_legocam(struct individual *pind, struct  iparameters *pipar);
extern void   update_groundsensors(struct individual *pind, struct  iparameters *pipar);
extern void   update_deltaground(struct individual *pind, struct  iparameters *pipar);
extern void   update_lastground(struct individual *pind, struct  iparameters *pipar);
extern void   move_robot(struct individual *pind);
extern int    move_gripper(struct individual *pind);
extern void   update_retina(struct individual *pind, struct  iparameters *pipar);
extern void   update_laserScan(struct individual *pind, struct  iparameters *pipar);
extern float  gaussianActivation(float val);
extern float gaussianActivation(float val, float var);

// sample files and functions
extern void   load_motor();
extern void   create_world_space();
extern void   load_obstacle(char *filename, int **object, int *objectcf);
extern float **motor;              // motor sampling, 12 rows each with 12*2(ang and dist) columnsi
extern int   **wall;               // infrared wall sampling, (180 degrees and 38 distances)
extern int   **obst;               // infrared round obstacle sampling, (180 d. and 38 distances
extern int   **sobst;              // infrared small round obstacle sampling (180 d. and 20 distances
extern int   **light;              // light sampling (180 degrees and 27 distances)
extern int   *wallcf;              // wall sampling configuration                          
extern int   *obstcf;              // obst sampling configuration                          
extern int   *sobstcf;             // sobst sampling configuration                          
extern int   *lightcf;             // light sampling configuration    

// epuck_io.cpp
void set_input_real(struct individual *pind, struct  iparameters *pipar);
void setpos_real(struct individual *pind,float o1, float o2);
void update_realsensors();
                    

struct individual

{

  int   ninputs;                                /* n. input units                           */
  int   nhiddens;                               /* n. hidden units                          */
  int   noutputs;                               /* n. output units                          */
  float input[80];                              /* the sensors                              */

  float   pos[2];				/* kepera position (center of the body)     */
  double  direction;				/* kepera direction                         */
  float   oldpos[2];
  double  oldirection;
  float   speed[2];				/* actual speed                             */
  int     crashed;                              // last step in which the robot crashed (-1 = no crash)
  int     jcrashed;                             // whether the robot crashed in the last time step
  float   cifsensors[8];                        /*  infrared sensors                        */
  int     cifang[8];                            //  orientation of each infrared
  float   clightsensors[8];                     /*  light sensors                           */
  float   csignals[4];                          // signal sensors
  float   scamera[5];                           // simple camera
  float   camera[3][360];                       // camera up to 360 photoreceptors * 3 colors
  float   ground[10];                           /* ground sensors state                     */
  float   lastground[10];                       // last visited ground
  float   detected_signal[4][2];                //  signals detected from nearby robots and corresponding distance                           
  float   energylevel;                          //  the current energy level                          
  float   proteinlevel;                         //  the current protein level                          
  float   waterlevel;                           //  the current water level                          
  float   wheel_motor[30];                       //  desired speed of wheel motors                           
  float   gripper_motor[2];                     //  the motors controlling the 2 DOF gripper
  float   signal_motor[4];                      //  the signals produced                           
  float   pseudo_motor[10];                     //  pseudo motor used in conjunction with efferent copies
  float   zoom_motor[4];							//  when using retina it activates zoom and up to three additional output
  float   decision_units[4];					//decision units useful when a decision is needed
  int     camera_nr;                            //  camera number of photoreceptors
  int     camera_c;                             //  camera number of photoreceptors of different colors
  int     camera_ang;                           //  camera intitial angle
  int     camera_dang;                          //  camera view angle of each photoreceptors
  int     grip_obtaken;                         //  1 when the robot is holding an object
  int     grip_obreleased;                      //  +=1 every time an object is released outside the arena
  float   retinaMatrix[10][10];						// up to 100 neurons
  float   retinaColorReceptor;					//to be used with retinaFovea option
  float   laserSensor;
  float   laserPos; //[0-1]
  float   emotionUnits[10];						//max 10 units
  float sensationUnits[10]; //(Daniela) max 10 units
  float clock[2]; //(Daniela) max 2 units


  double  fitness;				/*  current fitness                         */
  double  oldfitness;				/*  fitness up to the last epoch            */
  double  oldcfitness;				/*  fitness up to the last cycle            */
  int     population;                           /*  current population                      */
  int     n_team;
  int     lifecycle;				/*  lifecycle (within the current epoch)    */
  int     totcycles;                            /*  total lifecycles                        */
  int     number;
  float   eatenfood[2];                         // type of food eaten
  int     visited[1];                           // indicate whether it previously visited area1 or 2

  int     dead;                                 // whether an individual is temporary dead
  int     controlmode;                          // 0=neural_controller 1=behavior_based_controller
  int     gridF[10][10];						//useful to divide the retina world in cells for computing explorations

};


extern struct individual *ind;    // pointer to individuals phenotype
extern int  drawtrace;      // draw robots' trajectory trace for n. cycles  



struct iparameters

{
  // network structure
  int   nhiddens;                               /* number of hidden units                   */
  float wrange;                                 /* weights range                            */
  int   dynrechid;                              /* recurrent connections on the hiddens     */
  int   dynrecout;                              /* recurrent connections on the outputs     */
  int   dynioconn;                              /* input output connections                 */
  int   dyndeltai;                              /* delta on the input units                 */
  int   dyndeltah;                              /* delta on the hidden units                */
  int   dyndeltao;                              /* delta on the output units                */

  // sensors & motors
  int     nifsensors;				/* number of infrared sensors               */
  int     ngifsensors;                          /* number of genometrical infrared sensors  */
  int     ifang;                                /* infrared relative angle of the first     */
  int     ifdang;                               /* infrared, delta angle between sensors    */
  int     ifrange;                              /* infrared range in millimeters            */
  int     motmemory;				/* copy motor activation on sensors         */
  int     groundsensor;				/* ground sensors                           */
  int     a_groundsensor;			/* additional ground sensors (delta)        */
  int     a_groundsensor2;			/* additional ground sensors (prev-state)   */
  int     lightsensors;                         /* light sensors of the opponent's light    */
  int     oppsensor;                            /* sesor of the opponent dist               */
  int     linearcamera;                         // parametric linear camera 
  int     camerareceptors;                      // camera n. of photoreceptors
  int     cameraviewangle;                      // camera angleof view (+- n. degrees)
  int     cameradistance;                       // max distance at which colour are perceived
  int     cameracolors;                         // n. of colors discriminated by camera photoreceptors
  int     legocam;                              // lego camera, detect the size and the left-right position of red blobs
  int     input_outputs;                        /* additional output copied back into inputs*/
  int     signal_sensors;                       /* signal sensors                           */
  int     signalss;                             /* signals detected by nearby robots        */
  int     signalso;                             /* signals produce by robots                */
  int     signalssf;                            /* signal segmented phonemes                */
  int     signalssb;                            /* binary signals                           */
  int     signaltype;                           /* signal type 0=sound 1=led                */
  int     compass;                              /* compass sensor                           */
  int     simplevision;                         /* return the angle of visually detected objects */
  int     nutrients;                            /* the eaten nutrients                      */
  int     energy;                               /* propriosensor encoding the current level of energy       */
  int     protein;                              /* propriosensor encoding the current level of energy       */
  int     water;                                /* propriosensor encoding the current level of energy       */
  int     wheelmotors;                          /* presence of wheel motors                 */
  int     gripper;                              // 2DOF gripper
  char    ffitness[64];				/* fitness function name                        */
  float  retinaX;					//retina center x
  float  retinaY;					//retina center y
  float  retinaSize;				//size of the retina
  float cRetinaX, cRetinaY, cRetinaXold, cRetinaYold;			//current values c stand for current
  float cRetinaZoom; //current retinazoom to be used with muscular control
  float  retinaXrange, retinaYrange;		//defines the extent of horizontal and vertical movements of the retina
  float cRetinaSize, cRetinaSizeold;				//it depends on zoom if activated
  int    retina;				//number of neurons per side, so the tol neuron allocated will be retinaNeurons^2
  int	 eyeTargetRepeat;				//0; random n>0 the target wil be setted trial%eyeTargetRepeat
  float  retinaReceptiveField;		//represent the c constant of e gaussian
  float  cRetinaReceptiveField;		//this allows to use an output to select the automatically the current receptive field of neurons
  int    retinaFovea;				//enable color sensor positioned on the center of the retina
  int    motorEfference;			//motor efference
  int    retinaMotorControlType;	//0: the retina is controlled in position; 1: the retina is controlled in velocity
  int    retinaZoomMotor;
  int	 rMotor1GaussProprio;		//proprioception of the moptr #1 activate through a lattice of n neurons in a gaussian fashion____/^\____
  int	 rMotor2GaussProprio;
  int    decisionUnits;
  int     nEmotionUnits;						// no. of emotion units
  int	nSensationUnits; //(Daniela) number of sensation input units
  int nClock; //(Daniela)number of unit which counts the past steps
  int    overThreshold; //activation of the actuator on the empty spaces
  int    nTargetSetting;
  int    laserScan;					//a sensors that works like a laser beam to detect distances of nearby obstacles
  int    laserScanRange;			//range in degree +-laserRange from the head of the robot
  int	 laserScanProprioception;   //activate the prorpiocetion indicates the angle of the laserScan
  float  laserSpeed;				//angualr velocity
  float	 laserLength;				//legth of the beam
  float  trialScore;				//for testing
  int    firstChoice;				//for testing simmetry in decision: two target are simettrical placed around the center of the agent
									//default -1; 0 left 1 right

};


