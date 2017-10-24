/*
 * Evorobot* - robot-env.cpp
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
 * robot-env.cpp
 */
#include <algorithm>
#include "public.h"
#include "epuck.h"
#include "environment.h"
#include "ncontroller.h"

#ifdef EVOWINDOWS
#include "time.h"
clock_t sTime;
clock_t eTime;
#endif

extern MusclePair* xMuscle;
extern MusclePair* yMuscle;
extern MusclePair* zMuscle;

// type of robot
//float robotradius=27.5;        //the radius of the robot's body (KHEPERA RADIUS: 27.5   EPUCK RADIUS: 37.5)
float robotradius=37.5;        //the radius of the robot's body (KHEPERA RADIUS: 27.5   EPUCK RADIUS: 37.5)

// general
int     robottype=0;        // robot type 0=khepera, 1=e-puck, 2=lego 
int     real=0;             // whether we use real or simulated robots
int     drawtrace=0;        // draw robots' trajectory trace for n. cycles  
float   max_dist_attivazione = 0.0f;  //range di attivazione del sensore other
long int run = 0;           // current cycle
int     runcontrol = 0;    // (Daniela) variable which changes if the robot selects the wrong stimulus       
int     wrongcanc = 0;       // (Daniela) counts how many times the robot selects the wrong stimulus
float colorinput = 0.0; //(Daniela) stores the perceived target color and gives as input to em(!)
int goodfood =0; //(Daniela)counter of the good food eaten (input for cancellation task sensation)
int badfood =0; //(Daniela)counter of the bad food correctly thrown (input for cancellation task sensation)
int wrongfood = 0; //(Daniela)counter of the errors in eating or throwing the food (fitness cancellation task sensation)
int correctfood = 0; //(Daniela)counter of the correct eaten or thrown food (fitness cancellation task sensation)
int percentagegood =0; //(Daniela) percentage of good food in the specific trial (cancellation task sensation)
int totalcanc = 0; //(Daniela) total of cancelled targets in the trial (cancellation task sensation)
int *targets = NULL; //(Daniela) list of good and bad targets (cancellation task sensation)
int trialscount = 0; //count of the trials (Daniela)

bool even = false; //if the current gen is even or odd (Daniela)
int     cepoch=0;           // current trial
int     fitmode=0;          // 1=fitness/lifecycles 2=fitness/epochs
float   background_color=0.0;  // background color for the camera
char    ffitness_descp[255];// description of the current fitness function
int     individual_crash;   // a crash occurred between two individuals
int     manual_start=0;     // whether robots' position has been manually set by the user
// lifetime
int   testsamep=0;          // test, start from the same position       
int   nobreak=0;            // no artificial break on epochs          
int   stop_when_crash=1;    // we stop lifetime if a crash occurred  
int   startfarfrom=0;       // the robot start far from target areas and round obstacles
int	  centerFixed=0;		// if 1 the robot is placed at the center of the arena, 2 random change of direction 3, randome change of direction and position
int	  displaceObjects=0;		// randomize target position for the EYBOT
int   stepTarget=400;
//sensors & actuators
int fearTimer=0; //it activates the fear unit when > 0
struct iparameters *ipar=NULL; // individual parameters 
float  sensornoise=0.0;     // noise on sensors
//logstuff
float sens0 = 0;
float sens1 = 0;
float clock0 = 0;
FILE *evencor; //(Daniela) to print inside the fitness function
FILE *evenincor; //(Daniela) to print inside the fitness function
FILE *oddcor; //(Daniela) to print inside the fitness function
FILE *oddincor; //(Daniela) to print inside the fitness function
FILE *hyperact; //(Daniela) to print inside the fitness function
FILE *hiddenact; //(Daniela) to print inside the fitness function

struct  individual *ind=NULL;  // individual phenotype                

double maxfitness=1.0;      // max fitness value used to normalize performance within [0.0, 1.0] when possible

float ffitness_obstacle_avoidance(struct individual *pind);

float ffitness_patchFinder(struct individual *pind);
void initialize_world_patchFinder();

float ffitness_discrim(struct  individual  *pind);
void initialize_world_discrim();

float ffitness_stay2_on_areas(struct  individual  *pind);
void initialize_robot_position_stay2();

float ffitness_stay_on_different_areas(struct  individual  *pind);

float ffitness_tswitch(struct individual *pind);
void initialize_world_tswitch();

float ffitness_arbitrate(struct individual *pind);
void initialize_robot_position_arbitrate();
void initialize_world_arbitrate();

float ffitness_explore(struct individual *pind);
void initialize_world_explore();
void initialize_robot_position_explore();

float ffitness_garbage(struct  individual  *pind);

void  initialize_world_garbage();

float ffitness_neglect(struct  individual  *pind);
void initialize_world_neglect();

float ffitness_retinaSearch(struct  individual  *pind);
void initialize_world_retinaSearch();
void initialize_step_retinaSearch(int step, int epoch);

float ffitness_cancellationTask(struct  individual  *pind);
void initialize_world_cancellationTask();
void initialize_step_cancellationTask(int step, int epoch);

float ffitness_cancellationTask2(struct  individual  *pind); //a variant of canellation task, it still use init wolr and step of cancellation task
float ffitness_cancellationTask3(struct  individual  *pind); //a variant of cancellation task, the bootstrap fitness is based on grid exploration 
void  initialize_robot_cancellationTask3(); 

//impostazioni cancellation task Daniela
float ffitness_cancellationTask3Emotion(struct  individual  *pind); //a variant of cancellationTask3 plus emotion

float ffitness_cancellationTaskSensation(struct  individual  *pind); //(Daniela) cancellation task with sensation 


float ffitness_obstacle_avoidance2(struct  individual  *pind);

extern int dtime;              //time to accomplish sense-act cycle in real robot

float (*pffitness)(struct  individual  *pind) = NULL;  // pointer to fitness function()
void  (*pinit_robot_pos)() = NULL;                     // pointer to initialize_robot_position()
void  (*pinit_world)() = NULL;                         // pointer to initialize_world()
void  (*pinit_step)(int step, int epoch) = NULL;	  //  pointer to a function initialize_step();


/*
 * create robot and environmental parameters
 */
void
create_epuck_par()

{

   int    i,ii,iii;
   int    w;

   // allocate space for only one individual's parameter
   ipar = (struct iparameters *) malloc(1 * sizeof(struct iparameters));
   sprintf(ipar->ffitness,"NULL");
   ipar->wheelmotors     = 0;
   ipar->gripper         = 0;
   ipar->nifsensors      = 0;
   ipar->ngifsensors     = 0;
   ipar->ifang           = -45;
   ipar->ifdang          = 45;
   ipar->ifrange         = 400;
   ipar->lightsensors    = 0;
   ipar->linearcamera    = 0;
   ipar->camerareceptors = 0;
   ipar->cameraviewangle = 18;
   ipar->cameradistance  = 1000;
   ipar->cameracolors    = 1;
   ipar->motmemory       = 0;
   ipar->oppsensor       = 0;
   ipar->groundsensor    = 0;
   ipar->a_groundsensor  = 0;
   ipar->a_groundsensor2 = 0;
   ipar->input_outputs   = 0;
   ipar->signal_sensors  = 0;
   ipar->signalss        = 0;
   ipar->signalso        = 0;
   ipar->signalssf       = 0;
   ipar->signalssb       = 0;
   ipar->signaltype      = 0;
   ipar->nhiddens        = 0;
   ipar->dynrechid       = 0;
   ipar->dynrecout       = 0;
   ipar->dynioconn       = 0;
   ipar->dyndeltai       = 0;
   ipar->dyndeltah       = 0;
   ipar->dyndeltao       = 0;
   ipar->simplevision    = 0;
   ipar->legocam         = 0;
   ipar->nutrients       = 0;
   ipar->compass         = 0;
   ipar->energy          = 0;
   ipar->protein         = 0;
   ipar->water           = 0;
   ipar->retina			 = 0;
   ipar->retinaSize		 = 0;
   ipar->retinaX		 = 0;
   ipar->retinaY		 = 0;
   ipar->eyeTargetRepeat = 3;
   ipar->retinaReceptiveField =25.0;
   ipar->cRetinaReceptiveField=25.0;
   ipar->motorEfference = 0;
   ipar->retinaMotorControlType = 0;
   ipar->retinaZoomMotor= 0;
   ipar->decisionUnits=0;
   ipar->retinaXrange=90.0;
   ipar->retinaYrange=90.0;
   ipar->rMotor1GaussProprio=0;
   ipar->rMotor2GaussProprio=0;
   ipar->nTargetSetting=0;
   ipar->laserLength=100;
   ipar->laserScan=0;
   ipar->laserScanProprioception=0;
   ipar->laserScanRange=90;
   ipar->laserSpeed=5;
   


   create_cparameter("ffitness","individual",&ipar->ffitness, 0, 0, "fitness function");
   create_iparameter("robottype","individual",&robottype, 0, 5, "robot 0=khepera, 1=epuck, 2=lego, 3 Lego2, 4 Eye, 5 Retina");
   create_iparameter("nhiddens","individual",&ipar->nhiddens, 0, 200, "number of internal neurons");
   create_iparameter("motors","individual",&ipar->wheelmotors, 0, 30, "motor neurons controlling the two wheels");
   create_iparameter("gripper","individual",&ipar->gripper, 0, 1, "gripper with 2 DOF");
   create_iparameter("motmemory","individual",&ipar->motmemory, 0, 1, "efferent copy of motor neurons");
   create_iparameter("nifsensors","individual",&ipar->nifsensors, 0, 8, "n. of infrared sensors");
   create_iparameter("ngifsensors","individual",&ipar->ngifsensors, 0, 24, "n. of geometrical infrared sensors");
   create_iparameter("ifang","individual",&ipar->ifang, 0, 0, "relative angle of the first infrared sensor");
   create_iparameter("ifdang","individual",&ipar->ifdang, 0, 0, "delta angle between sensors");
   create_iparameter("ifrange","individual",&ipar->ifrange, 0, 0, "infrared sensors (geometrical) range (mm)");
   create_iparameter("nlightsensors","individual",&ipar->lightsensors, 0, 8, "n. of light sensors");
   create_iparameter("linearcamera","individual",&ipar->linearcamera, 0, 1, "parametric linear camera");
   create_iparameter("camerareceptors","individual",&ipar->camerareceptors, 0, 360, "camera: n. of photoreceptors");
   create_iparameter("cameraviewangle","individual",&ipar->cameraviewangle, 0, 180, "camera: view angle (+/- n. degrees)");
   create_iparameter("cameradistance","individual",&ipar->cameradistance, 0, 2000, "distance at wich object can be perceived");
   create_iparameter("cameracolors","individual",&ipar->cameracolors, 1, 3, "camera: n. colors discriminated by the camera");
   create_iparameter("simplevision","individual",&ipar->simplevision, 0, 2, "angle of visually detected objects");
   create_iparameter("legocam","individual",&ipar->legocam, 0, 1, "lego camera, size and left-right orientation of red blobs");
   create_iparameter("motsensors","individual",&ipar->motmemory, 0, 1, "efferent copies of motor states");
   create_iparameter("motorEfference","individual",&ipar->motorEfference, 0, 1, "efferent copies of motor states");
   create_iparameter("input_outputs","individual",&ipar->input_outputs, 0, 10, "additional outputs with efferent copies");
   create_iparameter("groundsensor","individual",&ipar->groundsensor, 0, 0, "n. of ground sensors");
   create_iparameter("a_groundsensor","individual",&ipar->a_groundsensor, 0, 0, "additional ground sensors which encode the previous state of ground sensors");
   create_iparameter("a_groundsensor2","individual",&ipar->a_groundsensor2, 0, 0, "additional ground sensors which encode the previous state of ground sensors");
   create_iparameter("signalss","individual",&ipar->signalss, 0, 4, "n. of sensors detecting signals");
   create_iparameter("signalso","individual",&ipar->signalso, 0, 4, "n. of outputs producing signals");
   create_iparameter("signalssb","individual",&ipar->signalssb, 0, 1, "binary signals");
   create_iparameter("signaltype","individual",&ipar->signaltype, 0, 1, "type of signal 0=sound 1=light");
   create_iparameter("compass","individual",&ipar->compass, 0, 1, "compass sensor");
   create_iparameter("energysensor","individual",&ipar->energy, 0, 1, "energy level propriosensor");
   create_iparameter("proteinsensor","individual",&ipar->protein, 0, 1, "protein level propriosensor");
   create_iparameter("watersensor","individual",&ipar->water, 0, 1, "water level propriosensor");
   create_fparameter("retinaX","individual",&ipar->retinaX, 0, 800, "Retina center Xcoordinate");
   create_fparameter("retinaY","individual",&ipar->retinaY, 0, 800, "Retina center Ycoordinate");
   create_fparameter("retinaSize","individual",&ipar->retinaSize, 0, 800, "Retina size");
   create_iparameter("retina","individual",&ipar->retina, 0, 10, "neurons per side of the retina, the total number will be retina^2");
   create_iparameter("eyeTargetRepeat","individual",&ipar->eyeTargetRepeat, 0, 30, "repeating target module");
   create_fparameter("retinaReceptiveField","individual",&ipar->retinaReceptiveField, 0, 2, "Retina size");
   create_iparameter("retinaFovea","individual",&ipar->retinaFovea, 0, 1, "colr sensor positioned on the center of the fovea");
   create_iparameter("retinaMotorControlType","individual",&ipar->retinaMotorControlType, 0, 40, "0: position; 1: velocity");
   create_iparameter("retinaZoomMotor","individual",&ipar->retinaZoomMotor, 0, 3, "1 activates the motor, 2 and more: add outputs");
   create_iparameter("decisionUnits","individual",&ipar->decisionUnits, 0, 4, "acitvates up to 4 decisional outputs");
   create_iparameter("emotionUnits","individual",&ipar->nEmotionUnits, 0, 10, "sets emotional input");
   create_iparameter("sensationUnits","individual",&ipar->nSensationUnits, 0, 10, "sets sensation input"); //(Daniela)
   create_iparameter("clock","individual",&ipar->nClock, 0, 2, "sets step count input"); //(Daniela)
   create_iparameter("rMotor1GaussProprio","individual",&ipar->rMotor1GaussProprio, 0, 20, "population representation of proprioception");
   create_iparameter("rMotor2GaussProprio","individual",&ipar->rMotor2GaussProprio, 0, 20, "population representation of proprioception");
   create_iparameter("nTargetSetting", "lifetime",&ipar->nTargetSetting, 0, 10, "different kiendo target setting");
   create_iparameter("laserScan","individual",&ipar->laserScan, 0, 1, "activates a laser sensor to detect distances from nearby obstacles");
   create_iparameter("laserScanRange","individual",&ipar->laserScanRange, 0, 180, "range in degrees of the laser scan");
   create_iparameter("laserScanProprioception","individual",&ipar->laserScanProprioception, 0, 20, "number of neurons to encode the laser prorpioception _/^\_");
   create_fparameter("laserSpeed","individual",&ipar->laserSpeed, 0, 180, "angular spedd of the laser beam");
   create_fparameter("laserLength","individual",&ipar->laserLength, 0, 180, "angular spedd of the laser beam");

   create_fparameter("sensornoise","lifetime",&sensornoise, 0, 0, "% of noise on infrared and light sensors");
   create_fparameter("signal_mdist","lifetime",&max_dist_attivazione, 0, 0, "range of communication sensors");
   create_iparameter("stop_on_crash","lifetime",&stop_when_crash, 0, 1, "trials end when one robot collide");
   create_iparameter("random_tareas","lifetime",&random_groundareas, 0, 1, "randomize foodzone positions");
   create_iparameter("random_round","lifetime",&random_round, 0, 1, "randomize small round");
   create_iparameter("startfarfrom","lifetime",&startfarfrom, 0, 1, "start far from targets and obstacles");
   create_iparameter("centerFixed","lifetime",&centerFixed, 0, 10, "1 start always at the center; 2random around the center");
   create_iparameter("displaceObjects","lifetime",&displaceObjects, 0, 10, "1 displace round objects, 2 right left to the retina");
   create_iparameter("stepTarget","lifetime",&stepTarget, 0, 2000, "the step at which the target appears");
   create_iparameter("drawtrace","lifetime",&drawtrace, 0, 0, "draw robot trace for n. cycles");

}


/*
 * Initializes the robots and the environment
 */
void
init_epuck()

{
    struct  individual  *pind;
    struct  iparameters *pipar;
    struct  envobject **eobj;

    int     team;
    int     cneuron;
    int     i,ii;
    int     a;

    switch(robottype)
     {
       case 0: // khepera
        robotradius=27.5;
    break;
       case 1: // e-puck
        robotradius=37.5;
    break;
       case 2: // lego
        robotradius=60;
    break;
	  case 3: // lego
        robotradius=60;
    break;
	  case 4: // the eye
        robotradius=37.5; //actually an epuck used as an eye
    break;
     }

    ind = (struct individual *) malloc((nteam) * sizeof(struct individual));
    if (ind == NULL)
    {
       display_error("individual malloc error");
    }

    for (team=0, pind=ind, pipar = ipar; team < nteam; team++, pind++)
     {
        pind->population = team;
        pind->n_team = team;
        pind->controlmode = 0;
        pind->dead = 0;
        pind->ninputs  = 0;
        pind->noutputs = 0;
        cneuron = 0;
        // sensors
        if (pipar->motmemory == 1)       
          for (i=0; i < pipar->wheelmotors; i++, cneuron++, pind->ninputs++)
             sprintf(neuronl[cneuron],"M%d",i);
        for (i=0; i < pipar->nifsensors; i++, cneuron++, pind->ninputs++)
             sprintf(neuronl[cneuron],"I%d",i);
        for (i=0; i < pipar->ngifsensors; i++, cneuron++, pind->ninputs++)
             sprintf(neuronl[cneuron],"I%d",i);
        for (i=0,a = pipar->ifang; i < pipar->nifsensors; i++, a+= pipar->ifdang)
         {
          if (a > 360) a -= 360;
          if (a < 0) a += 360;
          pind->cifang[i] = a;
          if (robottype == KHEPERA && i == 6)
            pind->cifang[i] = 144;
          if (robottype == KHEPERA && i == 7)
            pind->cifang[i] = 216;
          pind->cifsensors[i] = 0.0;
         }
        for (i=0,a = pipar->ifang; i < pipar->ngifsensors; i++, a+= pipar->ifdang)
         {
          if (a > 360) a -= 360;
          if (a < 0) a += 360;
          pind->cifang[i] = a;
          if (robottype == KHEPERA && i == 6)
            pind->cifang[i] = 144;
          if (robottype == KHEPERA && i == 7)
            pind->cifang[i] = 216;
          pind->cifsensors[i] = 0.0;
         }
        for (i=0; i < pipar->lightsensors; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"L%d",i);           
        for (i=0; i < (pipar->signalss * pipar->signalso); i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"s%d",i);
        for (i=0; i < pipar->signalso; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"S%d",i);    
        if (pipar->linearcamera == 1)
          for (i=0; i < pipar->camerareceptors * pipar->cameracolors; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"V%d",i); 
        if (pipar->simplevision == 1)  
          for (i=0; i < 5; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"V%d",i);
        if (pipar->simplevision == 2)  
          for (i=0; i < 3; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"V%d",i);
        if (pipar->legocam == 1) 
          for (i=0; i < 3; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"V%d",i);
        for (i=0; i < pipar->a_groundsensor2; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"g%d",i);
        for (i=0; i < pipar->groundsensor; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"G%d",i);
        for (i=0; i < pipar->a_groundsensor; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"g%d",i);
        for (i=0; i < pipar->input_outputs; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"O%d",i);
        if (pipar->nutrients > 0)  
          for (i=0; i < 2; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"N%d",i);
        if (pipar->compass == 1)  
          for (i=0; i < 2; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"C%d",i);
        if (pipar->energy == 1)  
          for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"E");
        if (pipar->protein == 1)  
          for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"P");
        if (pipar->water == 1)  
          for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"W");
        if (pipar->gripper == 1)  
          for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"B");
		if(pipar->retina>0)
			for(i=0;i<(pipar->retina * pipar->retina); i++,cneuron++,pind->ninputs++)
				sprintf(neuronl[cneuron],"R%d",i);
		if(pipar->retinaFovea==1)
		for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"F");

		if(pipar->motorEfference==1)
			for (i=0; i < pipar->wheelmotors; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"Me%d",i);

		//retina gaussian motor proprioception
		if(pipar->rMotor1GaussProprio>0)
			for (i=0; i < pipar->rMotor1GaussProprio; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"M1p%d",i);

		if(pipar->rMotor2GaussProprio>0)
			for (i=0; i < pipar->rMotor1GaussProprio; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"M2p%d",i);

		if(pipar->laserScan==1)
		for (i=0; i < 1; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"Lz");
		if(pipar->laserScanProprioception>0)
		for (i=0; i < pipar->laserScanProprioception; i++, cneuron++, pind->ninputs++)
            sprintf(neuronl[cneuron],"Lp%d",i);
		if (pipar->nEmotionUnits >0)
         for (i=0; i <pipar->nEmotionUnits; i++, cneuron++,pind->ninputs++)
           sprintf(neuronl[cneuron],"Em%d",i);
		if (pipar->nSensationUnits >0) //(Daniela)
         for (i=0; i <pipar->nSensationUnits; i++, cneuron++,pind->ninputs++)
           sprintf(neuronl[cneuron],"Se%d",i);
		if (pipar->nClock >0) //(Daniela)
         for (i=0; i <pipar->nClock; i++, cneuron++,pind->ninputs++)
           sprintf(neuronl[cneuron],"Cl%d",i);


        // hidden        
        for (i=0; i < pipar->nhiddens; i++, cneuron++)
            sprintf(neuronl[cneuron],"H%d",i);
        // motor neurons
        if (pipar->wheelmotors > 0)
         for (i=0; i < pipar->wheelmotors; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"M%d",i);
        if (pipar->gripper == 1)
         for (i=0; i < 2; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"G%d",i);
        for (i=0; i < pipar->signalso; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"S%d",i);
        for (i=0; i < pipar->input_outputs; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"O%d",i);
		if (pipar->retinaZoomMotor >0)
         for (i=0; i < pipar->retinaZoomMotor; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"Z%d",i);
		if (pipar->decisionUnits >0)
         for (i=0; i < pipar->decisionUnits; i++, cneuron++, pind->noutputs++)
           sprintf(neuronl[cneuron],"D%d",i);		
        // fitness 
        sprintf(neuronl[cneuron],"F");
        cneuron++;
        // camera
        if (pipar->legocam == 1)
         {
          pipar->cameracolors = 1;
          pipar->cameraviewangle = 42;
          pipar->camerareceptors = 2;
          pind->camera_nr = pipar->camerareceptors;
          pind->camera_c = 1;
          pind->camera_ang = (pipar->cameraviewangle / 2);
          pind->camera_dang = pipar->cameraviewangle / pipar->camerareceptors;
          for(i=0; i < pipar->cameracolors; i++)
           for(ii=0; ii < pipar->cameraviewangle; ii++)
              pind->camera[i][ii] = 0.0;
          }
        if (pipar->linearcamera == 1)
         {
          pind->camera_nr = pipar->camerareceptors;
          pind->camera_c = pipar->cameracolors;
          pind->camera_ang = (pipar->cameraviewangle / 2);
          pind->camera_dang = pipar->cameraviewangle / pipar->camerareceptors;
          for(i=0; i < pipar->cameracolors; i++)
           for(ii=0; ii < pipar->cameraviewangle; ii++)
                     pind->camera[i][ii] = 0.0;
          }
		if(pipar->retina>0)
		{
			//resetting camera
			pind->retinaColorReceptor=0;
			for(i=0;i<pipar->retina;i++)
				for(ii=0;ii<pipar->retina;ii++)
					pind->retinaMatrix[i][ii]=0;
		}

        pind->nhiddens = pipar->nhiddens;

        for(i=0;i < (pind->ninputs + pind->noutputs + pipar->nhiddens);i++)
                  strcpy(neuroncl[i],neuronl[i]); 

    }

    create_world_space();
    load_motor();

#ifdef EVOWINDOWS
    load_obstacle("../bin/sample_files/wall.sam",   wall,     wallcf);
    load_obstacle("../bin/sample_files/round.sam",  obst,     obstcf);
    load_obstacle("../bin/sample_files/small.sam",  sobst,    sobstcf);
    load_obstacle("../bin/sample_files/light.sam",  light,    lightcf);
#else    
        load_obstacle("../bin/sample_files/wall.sam",   wall,     wallcf);
    load_obstacle("../bin/sample_files/round.sam",  obst,     obstcf);
    load_obstacle("../bin/sample_files/small.sam",  sobst,    sobstcf);
    load_obstacle("../bin/sample_files/light.sam",  light,    lightcf);
#endif

    // pointer to the default function that initialize robot positions
    pinit_robot_pos = initialize_robot_position;
    // pointer to the default initialize world position
    pinit_world = initialize_world;

    sprintf(ffitness_descp,"Fitness: undefined");

    if (strcmp(ipar->ffitness,"obstacle-avoidance") == 0)
     {
      sprintf(ffitness_descp,"Fitness: ostacle-avoidance (move fast, straitght, and keep far from obstacles)");
      pffitness = ffitness_obstacle_avoidance;
      fitmode = 1;
     }
    if (strcmp(ipar->ffitness,"obstacle-avoidance2") == 0)
     {
      sprintf(ffitness_descp,"Fitness: ostacle-avoidance2 (move fast, straitght, and keep far from dark ground areas)");
      pffitness = ffitness_obstacle_avoidance2;
      fitmode = 1;
     }
    if (strcmp(ipar->ffitness,"discrim") == 0)
     {
      sprintf(ffitness_descp,"Fitness: discrim (1.0 when the robot is inside a target area)");
      pffitness = ffitness_discrim;
      pinit_world = initialize_world_discrim;
      fitmode = 1;
     }
    if (strcmp(ipar->ffitness,"collective-navigation") == 0)
     {
      sprintf(ffitness_descp,"Fitness: collective-navigation (0.25 for each robot inside targets, -1.0 for each extra robot)");
      pffitness = ffitness_stay2_on_areas;
      pinit_robot_pos = initialize_robot_position_stay2; // dedicated function for initializing the robot position
      fitmode = 2;
     }
    if (strcmp(ipar->ffitness,"garbage") == 0)
     {
      sprintf(ffitness_descp,"Fitness: garbage-collection (1 for holding an object, 10000 for releasing outside, -1 for crashes");
      pffitness = ffitness_garbage;
      pinit_world = initialize_world_garbage;
      fitmode = 1;
     }
    if (strcmp(ipar->ffitness,"different-areas") == 0)
     {
      sprintf(ffitness_descp,"Fitness: different-areas (1.0 if two robots are in two different areas, -1 if they are in the same)");
      pffitness = ffitness_stay_on_different_areas;
      fitmode = 2;
     }

    if (strcmp(ipar->ffitness,"tswitch") == 0)
     {
      sprintf(ffitness_descp,"Fitness: tswitch (1.0 every time the two robots are in two different area for the first time or after a switch)");
      pffitness = ffitness_tswitch;
      pinit_world = initialize_world_tswitch;
      fitmode = 2;
     }

    if (strcmp(ipar->ffitness,"explore") == 0)
     {
      sprintf(ffitness_descp,"Fitness: explore unvisited areas of the environment");
      pffitness = ffitness_explore;
      pinit_world = initialize_world_explore;
      pinit_robot_pos = initialize_robot_position_explore;
      maxfitness = 138.0;
      fitmode = 2;
     }

    if (strcmp(ipar->ffitness,"arbitrate") == 0)
     {
      sprintf(ffitness_descp,"Fitness: recharge, forage, and avoid predator");
      pffitness = ffitness_arbitrate;
      pinit_world = initialize_world_arbitrate;
      pinit_robot_pos = initialize_robot_position_arbitrate;
      ind->controlmode = 0;
      if (nteam > 1)
        {
         pind = (ind + 1);
         pind->controlmode = 1;
        }
      fitmode = 1;
     }
	  if (strcmp(ipar->ffitness,"neglect") == 0)
     {
      sprintf(ffitness_descp,"Fitness: neglegct fitness");
      pffitness = ffitness_neglect;
      pinit_world = initialize_world_neglect;
      fitmode = 1;
     }
	  	 
	 if (strcmp(ipar->ffitness,"retinaSearch") == 0)
     {
      sprintf(ffitness_descp,"Fitness: retinaSearch fitness");
      pffitness = ffitness_retinaSearch;
      pinit_world = initialize_world_retinaSearch;
	  pinit_step = initialize_step_retinaSearch;
      fitmode = 1;
     }
	  if (strcmp(ipar->ffitness,"cancellationTask") == 0)
     {
		
		 pffitness = ffitness_cancellationTask;
         pinit_world = initialize_world_cancellationTask;
		 pinit_step = initialize_step_cancellationTask;
		 fitmode=1;
	  }
	  	  if (strcmp(ipar->ffitness,"cancellationTask2") == 0)
     {
		
		 pffitness = ffitness_cancellationTask2;
         pinit_world = initialize_world_cancellationTask;
		 pinit_step = initialize_step_cancellationTask;
		 fitmode=1;
	  }
		 if (strcmp(ipar->ffitness,"cancellationTask3") == 0)
     {
		
		 pffitness = ffitness_cancellationTask3;
         pinit_world = initialize_world_cancellationTask;
		 pinit_step = initialize_step_cancellationTask;
		pinit_robot_pos = initialize_robot_cancellationTask3;
		 fitmode=1;
	  }

		  if (strcmp(ipar->ffitness,"cancellationTask3Emotion") == 0)
     {
		
		 pffitness = ffitness_cancellationTask3Emotion;
         pinit_world = initialize_world_cancellationTask;
		 pinit_step = initialize_step_cancellationTask;
		pinit_robot_pos = initialize_robot_cancellationTask3;
		 fitmode=1;
	  }
		  if (strcmp(ipar->ffitness,"cancellationTaskSensation") == 0)
     {
		
		 pffitness = ffitness_cancellationTaskSensation;
         pinit_world = initialize_world_cancellationTask;
		 pinit_step = initialize_step_cancellationTask;
		pinit_robot_pos = initialize_robot_cancellationTask3;
		 fitmode=1;
	  }

		  	 if (strcmp(ipar->ffitness,"patchFinder") == 0)
     {
      
		 sprintf(ffitness_descp,"Lookiing for foodpatch");
      pffitness = ffitness_patchFinder;
      pinit_world = initialize_world_patchFinder;
	  //pinit_step = initialize_step_retinaSearch;
      fitmode = 1;
     }

    // we now call the function that initialize the robot position
    // WE SHOULD DISCOMMENT THIS AND REMOVE THE CALL FROM MAINWINDOW.CPP
    //(*pinit_robot_pos)();

}

/*
 * read and return the number of sensors, internal and motors
 */
void
read_robot_units(int i, int *nsensors, int *ninternals, int *nmotors)

{
  struct  individual  *pind;

  pind = (ind + i);

  *nsensors = pind->ninputs;
  *ninternals = pind->nhiddens;
  *nmotors = pind->noutputs;

}

/*
 * set sensor activations (i.e. it update pind->input[])
 */
void
update_sensors(struct individual *pind, struct  iparameters *pipar)

{

    float  p1;
    float  p2;
    double direction;
    int    i,ii;
    int    c;
    int    s;
    int    nsensor;
    float  arb_max;
    int    arb_ind;

    p1        = pind->pos[0];
    p2        = pind->pos[1];
    direction = pind->direction;

    nsensor = 0;

    // efferent copies of motor neurons
    if (pipar->motmemory > 0)
    {
      for (i=0; i < ipar->wheelmotors; i++, nsensor++)
        pind->input[nsensor] = pind->wheel_motor[i]; 
    }
    // infrared sensors
    if (pipar->nifsensors > 0)
     {
          update_infrared_s(pind);
      switch(pipar->nifsensors)
       {
        case 3:
         for(ii=0;ii<3;ii++,nsensor++)
          pind->input[nsensor] = pind->cifsensors[ii];
         break;
        case 4:
         pind->input[nsensor]   = (pind->cifsensors[0] + pind->cifsensors[1]) / 2.0f;
         pind->input[nsensor+1] = (pind->cifsensors[2] + pind->cifsensors[3]) / 2.0f;
         pind->input[nsensor+2] = (pind->cifsensors[4] + pind->cifsensors[5]) / 2.0f;
         pind->input[nsensor+3] = (pind->cifsensors[6] + pind->cifsensors[7]) / 2.0f;
         nsensor += 4;
         break;
        case 6:
         for(ii=0;ii<6;ii++,nsensor++)
          pind->input[nsensor] = pind->cifsensors[ii];
         break;
        case 8:
         for(ii=0;ii<8;ii++,nsensor++)
          pind->input[nsensor] = pind->cifsensors[ii];
         break;
           }
         }
    // geometric infrared sensors
    if (pipar->ngifsensors > 0)
     {
          update_ginfrared_s(pind, pipar);
      for(ii=0;ii<pipar->ngifsensors;ii++,nsensor++)
          pind->input[nsensor] = pind->cifsensors[ii];

         }
    // light sensors
    if (pipar->lightsensors > 0)
    {
          if (robottype == 2)
       {
            update_glights_s(pind,pipar); // lego
        pind->input[nsensor] = pind->clightsensors[0];
        nsensor++;
        pind->input[nsensor] = pind->clightsensors[1];
        nsensor++;
           }
      else
       {
            update_lights_s(pind); // khepera
        switch(pipar->lightsensors)
        {
         case 2:
         pind->input[nsensor] = pind->clightsensors[2];
         nsensor++;
         pind->input[nsensor] = pind->clightsensors[6];
         nsensor++;
         break;
         case 3:
         pind->input[nsensor] = (pind->clightsensors[0] + pind->clightsensors[1]) / 2.0f;
         nsensor++;
         pind->input[nsensor] = (pind->clightsensors[2] + pind->clightsensors[3]) / 2.0f;
         nsensor++;
         pind->input[nsensor] = (pind->clightsensors[4] + pind->clightsensors[5]) / 2.0f;
         nsensor++;
         break;
         case 8:
         for(ii=0;ii<8;ii++,nsensor++)
        pind->input[nsensor] = pind->clightsensors[ii];
         break;
           }
          }
         }

        // signals from nearby robots
    if (pipar->signalss > 0)
    {
       update_signal_s(pind, pipar);
       for(i=0;i < pipar->signalss*pipar->signalso;i++,nsensor++)
         pind->input[nsensor] = pind->csignals[i];
    }

        // sensors encoding previously produced signals
    if (pipar->signalso > 0)
    {
         for(s=0;s < pipar->signalso; s++,nsensor++)
               pind->input[nsensor] = pind->signal_motor[s];
    }

        // linear camera
        if (pipar->linearcamera == 1)
        {
          update_camera(pind, pipar);
      for(c=0; c < pipar->cameracolors; c++)
       for(i=0; i < pipar->camerareceptors; i++, nsensor++)
         pind->input[nsensor] = pind->camera[c][i];
    }

    // simple vision
    if (pipar->simplevision > 0)
    {
          update_simplecamera(pind, pipar);
      if (pipar->simplevision == 1)
       for(i=0; i < 5; i++, nsensor++)
         pind->input[nsensor] = pind->scamera[i];
      if (pipar->simplevision == 2)
       for(i=0; i < 3; i++, nsensor++)
         pind->input[nsensor] = pind->scamera[i];
    }
    // lego camera sensors
    // return the linear distance between the centre of the blog a -30 e +30 degree, and the relative size of the blob with respect to the view field
    if(pipar->legocam == 1)
       {
         update_legocam(pind, pipar);
         for(i=0; i < 3; i++, nsensor++)
           pind->input[nsensor] = pind->camera[0][i];    
       }
    // sensors of last visited ground area
    if (pipar->a_groundsensor2 > 0)
    {
      for (i=0;i < pipar->a_groundsensor2; i++, nsensor++)
            pind->input[nsensor] = pind->lastground[i];
          if (real==0)
        update_lastground(pind, pipar);
    }

    // ground sensors
    if (pipar->groundsensor > 0)
    {
          if (real==0)
            update_groundsensors(pind, pipar);
      for (i=0;i < pipar->groundsensor; i++, nsensor++)
            pind->input[nsensor] = pind->ground[i];

    }

    // additional ground sensors (delta)
    if (pipar->a_groundsensor > 0)
    {
         if (real==0)
           update_deltaground(pind, pipar);
     for (i=0;i < pipar->a_groundsensor; i++, nsensor++)
           pind->input[nsensor] = pind->ground[i];
    }

    // the state of additional output units at time t-1
    if (pipar->input_outputs > 0)
    {
      for (i=0;i < pipar->input_outputs; i++,nsensor++)
        pind->input[nsensor] = pind->pseudo_motor[i];
    }

    // proprioceptors of eaten nutrients
    if (pipar->nutrients > 0)
    {
         pind->input[nsensor] = pind->eatenfood[0] / 100.0f;
         pind->input[nsensor+1] = pind->eatenfood[1] / 100.0f;
         nsensor += 2;
    }

    // a compass sensor
    if (pipar->compass > 0)
    {
          pind->input[nsensor] = (float) (sin(DGRtoRAD(pind->direction)) + 1.0) / 2.0;
      nsensor++;
          pind->input[nsensor] = (float) (cos(DGRtoRAD(pind->direction)) + 1.0) / 2.0;
      nsensor++;
    }

    // energy sensor
    if (pipar->energy > 0)
    {
      pind->input[nsensor] = pind->energylevel;
      nsensor++;
    }

    // protein sensor
    if (pipar->protein > 0)
    {
      pind->input[nsensor] = pind->proteinlevel;
      nsensor++;
    }

    // water sensor
    if (pipar->water > 0)
    {
      pind->input[nsensor] = pind->waterlevel;
      nsensor++;
    }

    // gripper light-barrier sensor
    if (pipar->gripper > 0)
    {
      if (pind->grip_obtaken == 0)
        pind->input[nsensor] = 0.0;
      else
        pind->input[nsensor] = 1.0;
      nsensor++;
    }
	//retina sensor
	if(pipar->retina>0)
	{
		//pipar->cRetinaX=pipar->retinaX;
		//pipar->cRetinaY=pipar->retinaY;
		update_retina(pind,pipar);
		for(int r=0;r<pipar->retina;r++)
			for(int c=0;c<pipar->retina;c++)
			{
				pind->input[nsensor]=pind->retinaMatrix[r][c];
				nsensor++;
			}
	}
	//fovea sensor
	if(pipar->retinaFovea==1)
	{
		pind->input[nsensor]=pind->retinaColorReceptor; //to do
		nsensor++;
	}

	if(pipar->motorEfference==1)
	{
		for(int i=0;i<pipar->wheelmotors;i++)
		{
		pind->input[nsensor]=pind->wheel_motor[i];
		nsensor++;
		}
		
	}

	//motor1 proprioception
	if(pipar->rMotor1GaussProprio>0)
	{
		float sigma=0.04;
		float df=1.0/((float)pipar->rMotor1GaussProprio-1);
		float xd=(ipar->cRetinaX-ipar->retinaX+ipar->retinaXrange)/(2*ipar->retinaXrange);
		float sn;
		for(int i=0;i<ipar->rMotor1GaussProprio;i++, nsensor++)
		{
			sn=i*df;
			pind->input[nsensor]=gaussian((xd-sn),sigma);
		}
	}
		//motor2 proprioception
	if(pipar->rMotor2GaussProprio>0)
	{
		float sigma=0.04;
		float df=1.0/((float)pipar->rMotor2GaussProprio-1);
		float yd=(ipar->cRetinaY-ipar->retinaY+ipar->retinaYrange)/(2*ipar->retinaYrange);
		float sn;
		for(int i=0;i<ipar->rMotor2GaussProprio;i++, nsensor++)
		{
			sn=i*df;
			pind->input[nsensor]=gaussian((yd-sn),sigma);
		}
	}

		//fovea sensor
	if(pipar->laserScan==1)
	{
		update_laserScan(pind,pipar);
		pind->input[nsensor]=pind->laserSensor; //to do
		nsensor++;
	}
		
	if(pipar->laserScanProprioception>0)
	{
		if(pipar->laserScanProprioception==1)
		{
		pind->input[nsensor]=pind->laserPos; //to do
		nsensor++;

		}
		else
		{
					float sigma=0.04;
					float df=1.0/((float)pipar->laserScanProprioception-1);
					float yd=pind->laserPos;
					float sn;
					for(int i=0;i<ipar->laserScanProprioception;i++, nsensor++)
						{
							sn=i*df;
							pind->input[nsensor]=gaussian((yd-sn),sigma)*0.3;
						}
		}
	}

	if(pipar->nEmotionUnits>0)
	{
		for(int i=0;i<ipar->nEmotionUnits;i++, nsensor++)
		{
			/*
			if(cepoch%8<4)
			pind->input[nsensor]=0.0; //trial pari spenta					
			else
			pind->input[nsensor]=1.0; //trial dispari accesa
			*/
			if(i==0)
			{
			if(fearTimer>0) pind->input[nsensor]=1.0;
			else
				pind->input[nsensor]=0.0;

			fearTimer--;
			if(fearTimer<0) fearTimer=0;
			}
			//edited Daniela
			if(i==1)
				//pind->input[nsensor]=pind->decision_units[1];
				pind->input[nsensor]=colorinput; //Daniela, puts in input the current target perceived
			/*if (i ==1)
			{
				if (fearTimer >0) 
				{
					pind->input[nsensor]=1.0;
				}
				else
				{
					pind->input[nsensor]=pind->decision_units[1];
				}
			}*/
			//end edited

		}

	}
	if(pipar->nSensationUnits>0)//(Daniela)
		{
			for(int i=0;i<ipar->nSensationUnits;i++, nsensor++)
			{
				if (i == 0)
				{
					//calculate the total number of good items on board given the good percentage and the total number of items (envnobjs)
					//float numberofgood = (((float)envnobjs[SROUND] * (float)percentagegood) /100.0); 
					
					//we use the total number of items on the board instead

					//scaling the percentage of good food eaten to the range [min 1 - max 10] for the sake of the saturation of the logistic function
					if (goodfood != 0)
					{
						pind->input[nsensor] = foodlogisticpos(((float)goodfood*10)/(float)envnobjs[SROUND]);
					}
					else
					{
						pind->input[nsensor] = 0.0; //which corrisponds to a 0 passed to the sigmoid
					}
				}
				if (i == 1)
				{
					//same as above but of course we use the inverse percentage (% of bad food)
					//float numberofbad = (((float)envnobjs[SROUND]* (100.0-(float)percentagegood))/100) ;
					if (badfood != 0)
					{
						pind->input[nsensor] = foodlogisticpos(((float)badfood*10)/(float)envnobjs[SROUND]);
					}
					else
					{
						pind->input[nsensor] = 0.0; //which corrisponds to a 0 passed to the sigmoid
					}
				}
			}
		}
		if(pipar->nClock>0)//(Daniela)
		{
			for(int i=0;i<ipar->nClock;i++, nsensor++)
			{
				//scaling the number of past steps to the range [min 1 - max 10]
				pind->input[nsensor] = clocklogisticpos(((float)run*10.0)/(float)sweeps);
			}
		}


}


/*
 * update motor states 
 */
void
update_motors(struct individual *pind, struct  iparameters *pipar, float *act)

{

      int nmotor; 
      int i;
      float *a;

      if (act != NULL && pipar->wheelmotors > 0)
       {
    nmotor = pind->ninputs + pind->nhiddens;
    a = (act + nmotor);

    pind->wheel_motor[2] = 1.0;
    for (i=0; i < ipar->wheelmotors; i++, nmotor++, a++)
          pind->wheel_motor[i] = *a;
        if (ipar->gripper == 1)
     for (i=0; i < 2; i++, nmotor++, a++)
          pind->gripper_motor[i] = *a;
    for (i=0; i < ipar->signalso; i++, nmotor++, a++)
          pind->signal_motor[i] = *a;
    for (i=0; i < ipar->input_outputs; i++, nmotor++, a++)
          pind->pseudo_motor[i] = *a;
	for (i=0; i < ipar->retinaZoomMotor; i++, nmotor++, a++)
          pind->zoom_motor[i] = *a;
	for (i=0; i < ipar->decisionUnits; i++, nmotor++, a++)
          pind->decision_units[i] = *a;
       }
       else
       {
    pind->wheel_motor[2] = 1.0;
    for (i=0; i < ipar->wheelmotors; i++, nmotor++, a++)
          pind->wheel_motor[i] = 0.0;
    for (i=0; i < ipar->signalso; i++, nmotor++, a++)
          pind->signal_motor[i] = 0.0;
    for (i=0; i < ipar->input_outputs; i++, nmotor++, a++)
          pind->pseudo_motor[i] = 0.0;
       }


}


/*
 * set the initial position of the robots
 * default function (may be overwritten by experiment specific functions)
 */
void
initialize_robot_position()
{

    struct individual *pind;
    int    volte;
    int    startset;
    int    team;

	
    for(team=0,pind=ind; team < nteam; team++, pind++)
     if (pind->dead == 0)
     {
    startset = 0;

    /*
     * start out of foodzones
     */
     if (startfarfrom == 1 && startset == 0)
        {
          pind->pos[0]    = 50.0 + ( double ) mrand(envdx - 100);
          pind->pos[1]    = 50.0 + ( double ) mrand(envdy - 100);
          pind->direction = (double) mrand(360);
          volte = 0;
          while (volte < 1000 && ((check_crash(pind->pos[0],pind->pos[1]) == 1) || (read_ground(pind) > 0)))
          {
            volte++;
            pind->pos[0]    = 50.0 + ( double ) mrand(envdx - 100);
            pind->pos[1]    = 50.0 + ( double ) mrand(envdy - 100);
            pind->direction = (double) mrand(360);
          }
        }

    // random positions without collisions (default case)
    if (startset == 0)
    {
       
		volte=0;
       do
       {
         volte++;
         pind->pos[0] = (float) fabs(rans((float) (envdx - 80))) + 40;
         pind->pos[1] = (float) fabs(rans((float) (envdy - 80))) + 40;
             pind->direction = fabs(rans(360.0));
       }
       while (volte == 0 || (volte < 1000 && (check_crash(pind->pos[0],pind->pos[1]) == 1) ));
    }
		 if (centerFixed == 1)
	 {
		 pind->pos[0] = (float)envdx/2.0;
         pind->pos[1] = (float)envdy/2.0;
		 pind->direction = -90.0;
	 
	 }
		 if (centerFixed == 2)
	 {
		 pind->pos[0] = (float)envdx/2.0;
         pind->pos[1] = (float)envdy/2.0;
		 pind->direction = -135+rans(90);
	 
	 }
		  if (centerFixed == 3)
	 {
		 pind->pos[0] = (float)envdx/2.0;
         pind->pos[1] = (float)envdy/2.0+rans(30)-5;
		 pind->direction = -135+rans(90);
	 
	 }
		   if (centerFixed == 4)
	 {
		 pind->pos[0] = 55;
         pind->pos[1] = 300+rans(100);//200
		 pind->direction = 90+rans(45);//90
	 
	 }
     }



}

void
setEyeTarget(int t)
{
	struct envobject *obj;
	int    ta,tt;
	
	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
			obj->c[0]=1;
			
			obj->c[1]=0;
			if(ta==t)
			obj->c[1]=1;
			
			
			obj->c[2]=0;
	}

}
void setEye2Target(int t1, int t2)
{
		struct envobject *obj;
	int    ta,tt;
	
	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
			obj->c[0]=1;
			
			obj->c[1]=0;
			if(ta==t1 || ta==t2)
			obj->c[1]=1;
			
			
			obj->c[2]=0;
	}

}
/*
 * re-initialize the characteristics of the environment
 */
void
initialize_world()


{

    int    i;
    int    ta,tt;
    int    tw;
    double d,mind;

    double w1, w2;
    int    volte;
    double min_dist_ob;
    struct envobject *obj;
    struct envobject *objb;

    // we randomize sround positions
    if (random_round == 1)
    {
      for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
          mind = 0.0;
          volte = 0;
          min_dist_ob = 150;
          while (mind < min_dist_ob && volte < 1000)
          {
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
            obj->x = (float) (fabs(rans((float) envdx - (double) (120 * 2))) + 120);
            obj->y = (float) (fabs(rans((float) envdy - (double) (120 * 2))) + 120);
            for (tt=0, objb = *(envobjs + 1), mind = 9999.9; tt < ta; tt++, objb)
            {
             d = mdist(objb->x, objb->y, obj->x, obj->y);
             if (d < mind)
               mind = d;
            }

          }
        }
    }

    // we randomize round positions
    if (random_round == 1)
    {
      for (ta=0, obj = *(envobjs + ROUND); ta<envnobjs[ROUND]; ta++, obj++)
      {
        mind = 0.0;
        volte = 0;
        min_dist_ob = 150;
        while (mind < min_dist_ob && volte < 1000)
        {
          mind = 9999.9;
          volte++;
          if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
          obj->x = (float) (fabs(rans((float) envdx - 275)) + 132.0);
          obj->y = (float) (fabs(rans((float) envdy - 275)) + 132.0);
          for (tt=0, objb = *(envobjs + 2); tt < ta; tt++, objb++)
            {
              d = mdist(objb->x, objb->y, obj->x, obj->y);
              if (d < mind)
                mind = d;
            }
          for (tt=0, objb = *(envobjs + 1); tt < envnobjs[1]; tt++, objb++)
            {
              d = mdist(objb->x,objb->y,obj->x,obj->y);
              if (d < mind)
                mind = d;
            }
        }
      }
    }

}


/*
 * it let the robots interact with the environment for one trial
 */
void
trial(int epoch) 


{

    struct  individual *pind;
    struct  individual *find;
        struct  iparameters *pipar;
    int     i;
    int     team;
    int     oldzone, oldzoneb;
    int     prev_zone, prev_zoneb;
    int     count;
    extern  float sound;                                         
    double  totmot;
    int     nturn1, nturn2;
    float   *act;
    int     gcrash;
    char    sbuffer[256];
	FILE *logfile;
	char	logFileName[256];

    // INITIALIZATION OF THE TRIAL
    run = 0;
    stop = 0;
	//setEyeTarget(-1); //to put into init step of retina fitness function
	if(logData==1 && testindividual_is_running==1) 
	{
		//here we open for writing the trialFile
		sprintf(logFileName,"LogDataTrial%d.xls",epoch);
		logfile=fopen(logFileName,"w");
		//fclose(logfile);
	}
	if(logData == 4 && testindividual_is_running==1)
		{
			hiddenact = fopen("HIDDEN.xls", "a");
		}
	if(logData==2 && testindividual_is_running==1 ) 
	{
		//here we open for writing the trialFile
		sprintf(logFileName,"LogDataEpochl%d.xls",epoch);
		logfile=fopen(logFileName,"w");
		//fclose(logfile);
	}
	if(logData==3 && testindividual_is_running==1 && epoch==0) 
	{
		//here we open for writing the trialFile
		sprintf(logFileName,"LogDataEpochl%d.xls",epoch);
		logfile=fopen(logFileName,"w");
		//fclose(logfile);
	}
    individual_crash = 0;
    nturn1 = 0;
    nturn2 = 0;
    totmot = 0.0;

    oldzone = 0;
    oldzoneb = 0;
    prev_zone = 0;
    prev_zoneb = 0;
    count = 0;

        // we update the process events (i.e. the main window) every trial 
        update_events();

    if (manual_start == 0)
     {
           (*pinit_world)();     // initialize world
           (*pinit_robot_pos)(); // initialize robot position 
       //initialize_world();
       //initialize_robot_position();
     }
    else
     {
       manual_start = 0;
     }

    find=ind;
    for(team=0,pind=ind;team<nteam;team++,pind++)
    {
     reset_controller(team);    // we set neurons state to 0.0
     for (i=0;i < pind->ninputs; i++)
           pind->input[i] = 0.0;
         update_motors(pind, ipar, NULL);
     pind->lifecycle    = 0;
     pind->visited[0]   = 0;
     pind->eatenfood[0] = 0.0;
     pind->eatenfood[1] = 0.0;
     pind->energylevel = 1.0;
     pind->proteinlevel = 0.0;
     pind->waterlevel = 0.0;
     pind->crashed = -1;
     pind->jcrashed = 0;
     pind->grip_obtaken = 0;
         pind->grip_obreleased = 0;

     for (i=0;i < ipar->a_groundsensor; i++)
           pind->ground[i] = 0.0;
     for (i=0;i < ipar->a_groundsensor2; i++)
           pind->lastground[i] = 0.0;
     if (epoch == 0)
     {
       pind->oldcfitness = 0.0;
       pind->oldfitness = 0.0;
     }
         else
       pind->oldfitness = pind->fitness;
    }

        // BEGIN OF THE TRIAL (sweeps=numero di cicli)
    while ((run < sweeps) && stop == 0)
    {
#ifdef EVOWINDOWS
      if (real==1)
        sTime=clock();
#endif

     //if (run>stepTarget) setEyeTarget(epoch % ipar->eyeTargetRepeat); //setEye2Target(epoch%3,(epoch+2)%3);// to put in the other retina fitness function
	  
	  if(pinit_step!=NULL)
		  (*pinit_step)(run,epoch);

	 if (real==1)
            update_realsensors();

      for(team=0,pind=ind, pipar = ipar;team<nteam;team++, pind++)
      {
        pind->oldcfitness = pind->fitness;
        if (real==0)
        {
          update_sensors(pind, pipar);
        }
             else
        {
          set_input_real(pind,pipar);
        }
      }

      for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
       if (pind->dead == 0)
       {
            if (pind->controlmode == 0)
                {
         act = update_ncontroller(team, pind->input);
		 if (logData == 4)
			 {	for (i = 52; i < 72; i++)
				{fprintf(hiddenact, "%f,",act[i]); }
				fprintf(hiddenact, "\n");
			}
                 update_motors(pind, pipar, act);

         if (pipar->wheelmotors >= 2.0)
           move_robot(pind);
         gcrash = 0;
         if (pipar->gripper == 1)
           gcrash = move_gripper(pind);
        }
        else
                {
         update_bcontroller(pind, pipar);
        }
        // check for collision
        pind->jcrashed = 0;
        if (check_crash(pind->pos[0],pind->pos[1]) == 1 || gcrash == 1)
        {
          pind->crashed = run;
          pind->jcrashed = 1;

          if (stop_when_crash == 1)
            stop = 1;
           else
            reset_pos_from_crash(pind);
        }
        // compute fitness 1 (after each robot moved)
        if (fitmode == 1 && *pffitness != NULL)
          ind->fitness += (*pffitness)(pind);

		//here we log data
		if(logData==1 && testindividual_is_running==1)
		{
			//fprintf(logfile,"%f,%f,%f\n",pind->pos[0],pind->pos[1],pind->direction);
			//fprintf(logfile,"%d,%f\n",cepoch,pipar->trialScore);
			
		}
	


      } // end team-for
      // compute fitness after all individuals moved
      if (fitmode == 2 && *pffitness != NULL)
        ind->fitness += (*pffitness)(ind);
      // update the rendering of the robot/environment and network
          if (testindividual_is_running == 1)
        {
         update_rendrobot();
         update_rendcontrolstate(run);
#ifdef EVO3DGRAPHICS
         update_rend3drobot();
#endif
        }
      // update world and neurons graphics
          if (testindividual_is_running == 1)
      {
        switch(fitmode) 
          {
        case 1:
          if (cepoch == 0)
           sprintf(sbuffer,"trial %d step %d fitness %.3f ", cepoch,ind->lifecycle, (ind->fitness - ind->oldfitness) / maxfitness / run);
          else
           sprintf(sbuffer,"trial %d step %d fitness %.3f pfitness %.3f ", cepoch,ind->lifecycle, (ind->fitness - ind->oldfitness) / maxfitness / run, (ind->oldfitness / (float) cepoch / maxfitness / sweeps));
          break;
        case 2:
          if (cepoch == 0)
           sprintf(sbuffer,"trial %d step %d fitness %.3f ", cepoch,ind->lifecycle, (ind->fitness - ind->oldfitness) / maxfitness);
          else
           sprintf(sbuffer,"trial %d step %d fitness %.3f pfitness %.3f ", cepoch,ind->lifecycle, (ind->fitness - ind->oldfitness) / maxfitness, (ind->oldfitness / (float) cepoch / maxfitness));
          break;
        default:
           sprintf(sbuffer,"trial %d step %d fitness %.3f ", cepoch,ind->lifecycle, ind->fitness / maxfitness);
          break;
          }
         
sprintf(sbuffer,"trial %d step %d fitness %.3f ", cepoch, ind->lifecycle, ind->fitness);

         display_stat_message(sbuffer);
      }

      for(team=0,pind=ind; team < nteam; team++, pind++)
      {
       pind->lifecycle++;
       pind->totcycles++;
      }

      if (timeadjust > 0 && testindividual_is_running == 1)
      {
          pause(timeadjust);
      }

    run++;
	//runcontrol++; //added by Daniela


#ifdef EVOWINDOWS
    if (real==1)
        {
     //sprintf(dbgString,"Time: %d mms\r\n",(int)eTime-(int)sTime);
     //display_warning(dbgString);
     eTime=clock();
     dtime=(int)eTime-(int)sTime;
     while( ((int)clock()-(int)sTime)<100) {}; //waits until 100mms
        }
#endif

    if (userbreak == 1)
    {
      run = sweeps;
      //stopping real robots if connected
      if (real==1)
        {
             for(team=0,pind=ind;team<nteam;team++,pind++) 
                setpos_real(pind,0.5f, 0.5f);
        }
    }
   }   // end for-sweeps

  
   if(logData==2 && testindividual_is_running==1) 
		{
			//fprintf(logfile,"%d,%f,%d\n",epoch,pipar->trialScore,pipar->firstChoice);
			
			//modificato da Daniela
			fprintf(logfile,"%d,%f,%d,%ld,%d,%d\n",epoch,pipar->trialScore,pipar->firstChoice,run,wrongcanc,ipar->overThreshold);
		}
   if(logData==3 && testindividual_is_running==1) 
		{
			//fprintf(logfile,"%d,%f,%d\n",epoch,pipar->trialScore,pipar->firstChoice);
			
			//modificato da Daniela
			fprintf(logfile,"%d,%d,%d,%d\n",correctfood,wrongfood,goodfood,badfood);
		}
    //here we close loDataFile in trial mode
   if(logData==1 && testindividual_is_running==1)
		{
			fclose(logfile);
		}
       //here we close loDataFile in epoch mode
   if(logData==2 && testindividual_is_running==1 && epoch==amoebaepochs-1)
		{
			fclose(logfile);
   }
   if(logData==3 && testindividual_is_running==1 && epoch==amoebaepochs-1)
		{
			fclose(logfile);
   }
   if(logData==4 && testindividual_is_running==1 && epoch==amoebaepochs-1)
   {fclose(hiddenact);}
   	

   // compute fitness 3 (at the end of the trial)
  if (fitmode == 3 && *pffitness != NULL)
      ind->fitness += (*pffitness)(pind);

}


/*
 * we evaluate an individual or a team of individuals
 * by letting them leave for several trials while evaluating their performance
 * standard error (varfit) is not calculated
 */
double evaluate_epuck(double *varfit)

{
     struct  individual *pind;
     int i;
     double totfitness;


     for(i=0,pind=ind; i<nteam; i++, pind++)
	   {
		pind->fitness   = 0.0;
		pind->oldfitness= 0.0;
		pind->totcycles = 0;
		pind->number = i;
	   }   
     *varfit = 0.0;
     for (cepoch=0; cepoch<amoebaepochs;cepoch++)
	   {
	      
		   ipar->trialScore=0;
		   //init muscle
		   if (xMuscle==NULL) xMuscle= new MusclePair(&ipar->cRetinaX,290,510,3.0f,200.0,2.5f,3.7f);
		   if (yMuscle==NULL) yMuscle= new MusclePair(&ipar->cRetinaY,290,510,3.0f,200.0,2.5f,3.7f);
		   if (zMuscle==NULL) zMuscle= new MusclePair(&ipar->cRetinaSize,0,380,3.0f,200.0,2.5f,3.7f);
		   //
		   trial(cepoch); //int gen, int nind, int pop
	       if (userbreak == 1)
		   {
            cepoch = amoebaepochs;
		    testindividual_is_running = 0;
		    evolution_is_running = 0;
		   }
	   }
       	// we normalize fitness
        for (i=0, pind = ind; i < nteam; i++, pind++)
	   {
	    switch(fitmode) 
	      {
		case 1:
		  pind->fitness = pind->fitness / (double) (sweeps * amoebaepochs);
		  break;
		case 2:
		  pind->fitness = pind->fitness / amoebaepochs;
		  break;
	      }
        if (maxfitness > 0.0)
          pind->fitness = pind->fitness / maxfitness;
	    }

	// we assume a cooperative selection schema
    for(i=0,totfitness=0.0, pind = ind; i < nteam; i++, pind++)
	  totfitness += pind->fitness;
	
	return(totfitness);

}

/*
 * we evaluate an individual or a team of individuals
 * by letting them leave for several trials while evaluating their performance
 * the function also calculate the standard error (varfit)

double evaluate_epuck(double *varfit)

{
     struct  individual *pind;
     int i;
     double totfitness;
     double avefit;
     double trialfit[100];
     int    varep;


     for(i=0,pind=ind; i<nteam; i++, pind++)
       {
        pind->fitness   = 0.0;
        pind->oldfitness= 0.0;
        pind->totcycles = 0;
        pind->number = i;
       }   
     avefit = 0.0;
     for (cepoch=0; cepoch<amoebaepochs;cepoch++)
       {
           trial(cepoch); 
           switch(fitmode) 
            {
             case 1:
               trialfit[cepoch] = (ind->fitness - ind->oldfitness) / (double) sweeps;
               avefit += (ind->fitness - ind->oldfitness) / (double) sweeps;
               break;
             case 2:
               trialfit[cepoch] = ind->fitness - ind->oldfitness;
               avefit += ind->fitness - ind->oldfitness;
               break;
             default:
               trialfit[cepoch] = ind->fitness - ind->oldfitness;
               avefit += ind->fitness - ind->oldfitness;
               break;
            }
           if (userbreak == 1)
           {
            cepoch = amoebaepochs;
            testindividual_is_running = 0;
            evolution_is_running = 0;
           }
       }
     avefit = avefit / (double) amoebaepochs;
     *varfit = 0.0;
     varep = amoebaepochs;
     if (varep > 100)
       varep = 100;
     for (cepoch=0;cepoch<varep;cepoch++)
         *varfit += fabs(trialfit[cepoch] - avefit); 
     *varfit += pow((trialfit[cepoch] - avefit), 2); 
     *varfit = *varfit / (double) varep;

     // we normalize fitness
     for (i=0, pind = ind; i < nteam; i++, pind++)
       {
        switch(fitmode) 
          {
        case 1:
          pind->fitness = pind->fitness / (double) (sweeps * amoebaepochs);
          break;
        case 2:
          pind->fitness = pind->fitness / (double) amoebaepochs;
          break;
          }
        if (maxfitness > 0.0)
          pind->fitness = pind->fitness / (double) maxfitness;
        }

    // we assume a cooperative selection schema
    for(i=0,totfitness=0.0, pind = ind; i < nteam; i++, pind++)
      totfitness += pind->fitness;
    
    return(totfitness);

}
*/
/*
 * patchFinder fitness function
 * reward for moving foward far from obstacles 
 */
float
ffitness_patchFinder(struct  individual  *pind)
{
	    double distzone;
    int z;
    float p1;
    float p2;
        struct envobject *obj;
    int   pi1;
    int   pi2;
	int   nvisited=0;
	p1 = SetX(pind->pos[0], 37.0, pind->direction);
    p2 = SetY(pind->pos[1], 37.0, pind->direction);
	float rfit=0;
	
	if (envnobjs[GROUND] > 0)
    {
      for(z=0, obj = *(envobjs + GROUND); z < envnobjs[GROUND]; z++, obj++)
        {
          if (obj->subtype == 0)//oggetto tondo
           {
            distzone = ((p1 - obj->x) * (p1 - obj->x))+((p2 - obj->y) * (p2 - obj->y));
            distzone = sqrt(distzone);
            if (distzone < obj->r && obj->visited==0)
			{
				obj->visited=1;
            rfit+= (float)sweeps/(float)envnobjs[GROUND];//100.0;
			}
           }
          else
           {
            if ( (p1 >= obj->x) && (p1 < (obj->x + obj->X)) &&
                         (p2 >= obj->y) && (p2 < (obj->y + obj->Y)) && obj->visited==0 )
			{
				obj->visited=1;
           rfit+= (float)sweeps/(float)envnobjs[GROUND];//100.0; in questo modo una fitness di 1 indica che durante il lifetime ha compiuto un giro
			}
           }
        }

	  for(z=0, obj = *(envobjs + GROUND); z < envnobjs[GROUND]; z++, obj++)
        {
			nvisited+=obj->visited;
		}
	  if(nvisited==envnobjs[GROUND]) initialize_world_patchFinder();
    }
	return rfit;

}

void initialize_world_patchFinder()
{
	  int z;
    float p1;
    float p2;
        struct envobject *obj;
	for(z=0, obj = *(envobjs + GROUND); z < envnobjs[GROUND]; z++, obj++)
        {
			obj->visited=0;
	}

}

/*
 * obstacle-avoidance fitness function
 * reward for moving foward far from obstacles 
 */
float
ffitness_obstacle_avoidance(struct  individual  *pind)

{
   float vel1, vel2;
   float ave_mot;              
   float dif_m;                
   float maxs;  
   int   s;

   vel1 = pind->speed[0];
   vel2 = pind->speed[1];
   ave_mot = (float) ((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
   dif_m   = (float) sqrt(fabs( ((vel1 + (float) 10.0) / (float) 20.0) - ((vel2 + (float) 10.0) / (float) 20.0) ) );
   for(s=0, maxs = (float) 0.0; s < ipar->nifsensors; s++)
       if (pind->cifsensors[s] > maxs)
         maxs = pind->cifsensors[s];
   return(ave_mot * (1.0 - dif_m));// * (1.0 - maxs));

}


/*
 * obstacle-avoidance fitness function
 * reward for moving foward far from obstacles 
 */
float
ffitness_obstacle_avoidance2(struct  individual  *pind)

{
   float vel1, vel2;
   float ave_mot;              
   float dif_m;                

   vel1 = pind->speed[0];
   vel2 = pind->speed[1];
   ave_mot = (float) ((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
   dif_m   = (float) sqrt(fabs( ((vel1 + (float) 10.0) / (float) 20.0) - ((vel2 + (float) 10.0) / (float) 20.0) ) );
   return(ave_mot * (1.0 - dif_m) * (1.0 - pind->ground[0]));
}

/*
 * stay_on_area fitness function
 * reward for staying on a target area 
 */
float
ffitness_discrim(struct  individual  *pind)

{

  if (read_ground(pind) > 0)
      return(1.0);
    else
      return(0.0);
}


/*
 * re-initialize the characteristics of the environment
 */
void
initialize_world_discrim()


{

    int    i;
    int    ta,tt;
    int    tw;
    double d,mind;
    double w1, w2;
    int    volte;
    double min_dist_ob;
    struct envobject *obj;
    struct envobject *objb;
    struct envobject *objc;

    //randomize foodzone positions & corresponding small round objects
    if (random_groundareas == 1)
    {
      for (ta=0, obj = *(envobjs + 3); ta < envnobjs[3]; ta++, obj++)
      {
        mind = 0.0;
        volte = 0;
        min_dist_ob = obj->r * 2.0f;
        min_dist_ob += min_dist_ob * 0.1;
        while (mind < min_dist_ob && volte < 2000)
        {
            mind = 9999.9;
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
                min_dist_ob -= 1;
            obj->x = (float) fabs(rans((float) envdx - min_dist_ob)) + (min_dist_ob / 2);
            obj->y = (float) fabs(rans((float) envdy - min_dist_ob)) + (min_dist_ob / 2);
            if (envnobjs[1] > 0)
                 {
                           objc = *(envobjs + 1);
               objc->x = obj->x;
               objc->y = obj->y;
            }
            for (tt=0, objb = *(envobjs + 3); tt < ta; tt++, objb)
            {
                d = mdist(objb->x,objb->y,obj->x, obj->y);
                if (d < mind)
                    mind = d;
            }
        }
      }
    }


}



/*
 * stay2_on_areas fitness function
 * reward for staying on a target area 
 */
float
ffitness_stay2_on_areas(struct  individual  *pind)

{

  struct  individual  *cind;
  int nxarea1, nxarea2;
  int n;
  float score;

  nxarea1 = 0;
  nxarea2 = 0;
  score = 0.0;

  for(n=0, cind=pind; n < nteam; n++, cind++)
   {
     if (read_ground(cind) == 1)
      {
       nxarea1++;
       if (nxarea1 <= 2)
         score += 0.25;
        else
         score -= 1.0;
      }
     if (read_ground(cind) == 2)
      {
       nxarea2++;
       if (nxarea2 <= 2)
         score += 0.25;
        else
         score -= 1.0;
      }
    }
    return(score);
}

/*
 * set the initial position of the robots
 * robots start outside target area
 */
void
initialize_robot_position_stay2()
{

    struct individual *pind;
    int    volte;
    int    team;


    for(team=0,pind=ind; team < nteam; team++, pind++)
     if (pind->dead == 0)
     {
       volte=0;
       do
    {
      volte++;
      pind->pos[0] = (float) fabs(rans((float) (envdx - 80))) + 40;
      pind->pos[1] = (float) fabs(rans((float) (envdy - 80))) + 40;
          pind->direction = fabs(rans(360.0));
    }
       while (volte == 0 || (volte < 1000 && ((check_crash(pind->pos[0],pind->pos[1]) == 1) || (read_ground(pind) > 0)) ));
     }

}


/*
 * stay_on_different_areas fitness function
 * 1.0 or -1.0 for staying in different or equal areas
 */
float
ffitness_stay_on_different_areas(struct  individual  *pind)

{

  struct  individual *cind;

  if (nteam == 2)
    {
      cind = (pind + 1);
      if ((read_ground(pind) > 0) && (read_ground(cind) > 0))
        {
      if (((read_ground(pind) % 2) != (read_ground(cind) % 2)))
        return(1.0);
      else
        return(-1.0);
    }
    }
  return(0.0);
}

/*
 * tswitch variables
 */

float energycost = 0.0;

/*
 * tswitch fitness function
 * 1.0 every time the two individuals are in different areas for the first time or after a shift
 */
float
ffitness_tswitch(struct  individual  *pind)

{

  struct  individual  *pind2;
  float  cost;

  if (nteam == 2)
    {
    pind2 = (pind + 1);
    energycost += ((float) (abs(pind->speed[0]) + abs(pind->speed[1])) / 20.0) * 0.00125;
    energycost += ((float) (abs(pind2->speed[0]) + abs(pind2->speed[1])) / 20.0) * 0.00125;
    if ((read_ground(pind) > 0) && (read_ground(pind2) > 0) 
        && ((read_ground(pind) % 2) != (read_ground(pind2) % 2)) 
        && (pind->visited[0] == 0 || pind->visited[0] != read_ground(pind)))
      {
            pind->visited[0] = read_ground(pind);
            pind2->visited[0] = read_ground(pind2);
            cost = energycost;
            if (cost > 0.75)
              cost = 0.75;
            energycost = 0.0;
        return(1.0 - cost);
      }

     }

   return(0.0);

}

void initialize_world_neglect()
{
	
	//to do initializing the world
	struct envobject *obj;
	int    ta,tt;
	ipar->cRetinaX=ipar->retinaX;
	ipar->cRetinaY=ipar->retinaY;
	ipar->cRetinaSize=ipar->retinaSize;
	
	ipar->cRetinaYold=ipar->cRetinaY;
	ipar->cRetinaXold=ipar->cRetinaX;
	ipar->cRetinaSizeold=ipar->cRetinaSize;


	if(displaceObjects==1)
	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
			switch(ta)
			{
				case 0:
			obj->x=300+(int)rans(20);
			break;
			case 1:
			obj->x=250+(int)rans(30)-15;
			break;
			case 2:
			obj->x=350-(int)rans(20);
			break;
			}
	}
	
}

void initialize_world_retinaSearch()
{
	//nothing special
	//to do initializing the world
	struct envobject *obj;
	int    ta,tt;
	ipar->cRetinaX=ipar->retinaX;
	ipar->cRetinaY=ipar->retinaY;
	ipar->cRetinaSize=ipar->retinaSize;
	if(displaceObjects==2)
	{
		
		//we randomize round object position to the rigth and to the left
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			/*
			if(ta%2==0)
			{//random to the right
				obj->x=ipar->retinaX+mrand(90)+10;
				obj->y=ipar->retinaY+rans(90);
			}else
			{
				obj->x=ipar->retinaX-mrand(90)-10;
				obj->y=ipar->retinaY+rans(90);

				//random to the left
			}*/
			obj->x=ipar->retinaX+rans(ipar->retinaXrange);
			obj->y=ipar->retinaY+rans(ipar->retinaYrange);

		}
	}
}
/*
 * re-initialize the characteristics of the environment
 */
void
initialize_world_tswitch()


{

    int    ta,tt;
    int    tw;
    double d,mind;
    double w1, w2;
    int    volte;
    double min_dist_ob;
    struct envobject *obj;
    struct envobject *objb;
    struct envobject *objc;


    //randomize foodzone positions (joachim)
    if (random_groundareas == 1)
    {
      for (ta=0, obj = *(envobjs + 3); ta < envnobjs[3]; ta++, obj++)
      {
        mind = 0.0;
        volte = 0;
        min_dist_ob = obj->r*3.0f;
        while (mind < min_dist_ob && volte < 20000)
        {
            mind = 9999.9;
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
                min_dist_ob -= 1;
            obj->x = (float) (fabs(rans((float) envdx - (obj->r * 2.0f))) + obj->r);
            obj->y = (float) (fabs(rans((float) envdy - (obj->r * 2.0f))) + obj->r);
            //check for other foodzone
            for (tt=0, objb = *(envobjs + 3); tt < ta; tt++, objb++)
            {
                d = mdist(objb->x,objb->y,obj->x,obj->y);
                if (d < mind)
                    mind = d;
            }

        }
      }

     }


}


/*
 * global variables arbitrate
 * we assume that the environment contain a recharging area and a feading area.
 * The position of the food and of the predator is initialized in
 *  the feeding area
 * The position of the prey is initialized outside ground areas
 */
int     crashed_step;       // whether a robot crashed during the current step 
float   max_punishment;     // the max punishement which can be provided with one trial
int     rewardstart;        // fitness is scored as soon as the robot start to methabolize protein and water
float   foodareax,foodareay;// the barycentre of the area in which food grow
/*
 * randomize the position of the round objects
 * inside the feeding area (ground-2)
 * and initialize arbitrate variables
 */
void
initialize_world_arbitrate()


{

    int    i;
    int    ta,tt;
    int    tw;
    double d,mind;
    int    coin;

    double w1, w2;
    int    volte;
    double min_dist_ob;
    struct envobject *obj;
    struct envobject *objb;
    float feedx,feedy,feeddx,feeddy;

    crashed_step = 0;
    max_punishment = 0.0;
    rewardstart = 0;

    // feedarea
    if (envnobjs[GROUND] >= 2)
    {
      obj = *(envobjs + GROUND);
      obj++;
      feedx = obj->x;
      feedy = obj->y;
      feeddx = obj->X;
      feeddy = obj->Y;
        }
       else
    {
      feedx = 50;
      feedy = 50;
      feeddx = envdx - 50;
      feeddy = envdy - 50;
        }

    // we randomize sround positions
    if (random_round == 1)
    {
      coin = mrand(4);
      switch(coin)
       {
        case 0:
         foodareax = envdx/4 * 1;  
         foodareay = envdy/4 * 1;
         break;
        case 1:
         foodareax = envdx/4 * 1;  
         foodareay = envdy/4 * 3;
         break;
        case 2:
         foodareax = envdx/4 * 3;  
         foodareay = envdy/4 * 1;
         break;
        case 3:
         foodareax = envdx/4 * 3;  
         foodareay = envdy/4 * 3;
         break;
       }
      for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
          mind = 0.0;
          volte = 0;
          min_dist_ob = 50;
          while (volte < 1000 && mind < min_dist_ob)
          {
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
            obj->x = foodareax + (rans(100));
            obj->y = foodareay + (rans(100));
            for (tt=0, objb = *(envobjs + 1), mind = 9999.9; tt < ta; tt++, objb)
            {
             d = mdist(objb->x, objb->y, obj->x, obj->y);
             if (d < mind)
               mind = d;
            }

          }
        }
    }
    /* we randomize sround positions
    if (random_round == 1)
    {
      for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
          mind = 0.0;
          volte = 0;
          min_dist_ob = 150;
          while (volte < 1000 && mind < min_dist_ob)
          {
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
            obj->x = (float) (fabs(rans(feeddx)) + feedx);
            obj->y = (float) (fabs(rans(feeddy)) + feedy);
            for (tt=0, objb = *(envobjs + 1), mind = 9999.9; tt < ta; tt++, objb)
            {
             d = mdist(objb->x, objb->y, obj->x, obj->y);
             if (d < mind)
               mind = d;
            }

          }
        }
    }
    */

    // we randomize round positions
    if (random_round == 1)
    {
      for (ta=0, obj = *(envobjs + ROUND); ta<envnobjs[ROUND]; ta++, obj++)
      {
        mind = 0.0;
        volte = 0;
        min_dist_ob = 150;
        while (mind < min_dist_ob && volte < 1000)
        {
          mind = 9999.9;
          volte++;
          if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
          obj->x = (float) (fabs(rans(feeddx)) + feedx);
          obj->y = (float) (fabs(rans(feeddy)) + feedy);
          for (tt=0, objb = *(envobjs + 2); tt < ta; tt++, objb++)
            {
              d = mdist(objb->x, objb->y, obj->x, obj->y);
              if (d < mind)
                mind = d;
            }
          for (tt=0, objb = *(envobjs + 1); tt < envnobjs[1]; tt++, objb++)
            {
              d = mdist(objb->x,objb->y,obj->x,obj->y);
              if (d < mind)
                mind = d;
            }
        }
      }
    }

}

/*
 * set the initial position of the predator and prey robots
 * the first robot (prey) is placed outside target areas (groud color = < 0.25)
 * the second robot (predator) is placed in the feeding area (i.e. the second ground area)
 */
void
initialize_robot_position_arbitrate()
{

    struct individual *pind;
    int    volte;
    float feedx,feedy,feeddx,feeddy;
    struct envobject *obj;



    // feedarea
    if (envnobjs[GROUND] >= 2)
    {
      obj = *(envobjs + GROUND);
      obj++;
      feedx = obj->x;
      feedy = obj->y;
      feeddx = obj->X;
      feeddy = obj->Y;
        }
       else
    {
      feedx = 100;
      feedy = 100;
      feeddx = envdx - 100;
      feeddy = envdy - 100;
        }
    if (nteam > 0)
      {
        pind = ind;
        pind->pos[0] = 50.0 + (double) mrand(envdx - 100);
        pind->pos[1] = 50.0 + (double) mrand(envdy - 100);
    pind->direction = (double) mrand(360);
    volte = 0;
    while (volte < 1000 && (read_ground_color(pind) < 0.25 || (check_crash(pind->pos[0],pind->pos[1]) == 1) ) )
        {
          volte++;
          pind->pos[0]    = 50.0 + (double) mrand(envdx - 100);
          pind->pos[1]    = 50.0 + (double) mrand(envdy - 100);
          pind->direction = (double) mrand(360);
        }
        
      }

    if (nteam > 1)
      {
        pind = (ind + 1);
        pind->pos[0] = mrand((int) feeddx) + feedx;
        pind->pos[1] = mrand((int) feeddy) + feedy;
    pind->direction = (double) mrand(360);
    volte = 0;
    while (volte < 1000 && (read_ground_color(pind) < 0.25 || read_ground_color(pind) > 0.75 || (check_crash(pind->pos[0],pind->pos[1]) == 1) ) )
        {
          volte++;
              pind->pos[0] = mrand((int) feeddx) + feedx;
              pind->pos[1] = mrand((int) feeddy) + feedy;
          pind->direction = (double) mrand(360);
        } 
      }
    


}


/*
 * drink and forage
 * we reward the individual for an healthy diet (i.e. protein and water within [0.5, 1.5]
 * and for keeping its energy level above 0.0.
 * energy is synthesized when protein and water are above 0.5
 */
float
ffitness_arbitrate(struct  individual  *pind)

{

  double dist;
  int t;
  float score;
  struct envobject *obj;


  score = 0.0;

  pind->waterlevel -= (1.0f / 800.0f); 
  pind->proteinlevel -= (1.0f / 800.0f);

  if (read_ground_color(pind) < 0.10)
      pind->waterlevel = 1.0;

  if (pind->waterlevel > 1.0)
    pind->waterlevel = 1.0;
  if (pind->waterlevel < 0.0)
    pind->waterlevel = 0.0;


  for (t=0, obj = *(envobjs + SROUND); t < envnobjs[SROUND]; t++, obj++)
     {
       dist = mdist((float)pind->pos[0],(float)pind->pos[1],(float) obj->x,(float) obj->y);
       if ((dist - 3) < (robotradius + obj->r))
        {
         pind->proteinlevel = 1.0;
         // better check the new object does not crash with other objects of with the predator 
         obj->x = -10000;
         obj->y = -10000;
        }
     }
  // food growth
  for (t=0, obj = *(envobjs + SROUND); t < envnobjs[SROUND]; t++, obj++)
     {

       if (obj->x < 0 && mrand(1000) == 0)
        {
         obj->x = foodareax + (rans(100));
         obj->y = foodareay + (rans(100));
         while (check_crash(pind->pos[0],pind->pos[1]) == 1)
              {
                obj->x = foodareax + (rans(100));
                obj->y = foodareay + (rans(100));
              }
        }
     }
    if (pind->proteinlevel < 0.0)
    pind->proteinlevel = 0.0;
 
  // originally the two components weights were 1.0 and 1.0
  pind->energylevel -= (0.5f / 1000.0f) + (1.5f / 1000.0f) *  ((float) (abs(pind->speed[0]) + abs(pind->speed[1])) / 20.0);

  if (pind->waterlevel >= 0.5 && pind->proteinlevel >= 0.5)
     {
      rewardstart = 1;
      pind->energylevel += (1.0f / 200.0f);
      // we do not reward anymore for food and water directly
      //score += 0.5;
     }

  if (pind->energylevel > 1.0)
    pind->energylevel = 1.0;
  if (pind->energylevel <= 0.0)
       pind->energylevel = 0.0;

  // reward for enery level was originally always 0.5 (for energy > 0)
  if (pind->energylevel > 0.0 && rewardstart == 1)
    score += pind->energylevel;

  //if (pind->jcrashed == 0)
  //  score += 0.1;

  if (rewardstart == 1 && pind->jcrashed == 0)
    return(score);
  else
    return(0.0);



}

float
ffitness_neglect(struct  individual  *pind)
{
	if(ipar->motmemory==1)
		return pind->input[16+2];
	else
	return pind->input[16];
}
float ffitness_retinaSearch(struct  individual  *pind)
{
	//to do
	//if(ipar->motmemory==1)
	//return pind->input[50+2]; //the fovea
	//else
	//	return pind->input[50];

	//return pind->retinaColorReceptor;
	//the fitness is the distance between retina and object only one!
	struct envobject *obj;
	obj = *(envobjs + SROUND);
	float dist=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
	return gaussianActivation(dist,30);
	
}
void initialize_step_retinaSearch(int step, int epoch)
{
	if(step==0) setEyeTarget(-1); 
	if (step>stepTarget) setEyeTarget(epoch % ipar->eyeTargetRepeat); //setEye2Target(epoch%3,(epoch+2)%3);// to put in the other retina fitness function
	

}

//cancellation task
float ffitness_cancellationTask(struct  individual  *pind)
{
	struct envobject *obj;
	int    ta,tt;
	float distTarget;
	if(pind->decision_units[0]>0.7) ipar->overThreshold++;
	float correctTarget=0;
	float wrongTarget=0;
	float totalCheckedTarget=0;
	float bootStrapDistance=-1;
	float rfit=0;
	

	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			
			
			if(obj->checked==0)
			{
				
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				if(distTarget<25) //it was 15
				{
					if(obj->visited==0)
					{
						
						//rfit+=10;
						if(obj->c[0]==1.0)
						rfit+=10;//+10*pind->decision_units[0];
						else
						rfit+=10;

						obj->visited=1;
						
					}

				
				//if (distTarget<15 && pind->zoom_motor[1]>0.7)
					//{
						if(pind->decision_units[0]>0.7)
						{
							obj->checked=1;
							if(obj->c[0]==1)
							{
							obj->x=4000;
							rfit+=100;
							}
							if(obj->c[0]==0.5) stop=1;
						}
						
				}
				
			}
	}
	
	//bootStrapDistance=-1;
	/*if(run==sweeps-1)
		{
			for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			if(obj->c[0]==1.0)totalCheckedTarget++;	
			if(obj->checked==1)
				{
					if(obj->c[0]==1)
					{
						correctTarget++;
						
					}
					else
					{
						wrongTarget++;
						//stop=1;
					}
						
					
				}
		}
			//if(totalCheckedTarget>0)
			//return (correctTarget/totalCheckedTarget)*1000-(wrongTarget/(float)envnobjs[SROUND])*1000;
			//else
				return 0.0;

	}*/
	//if(bootStrapDistance>-1)
	//return gaussianActivation(bootStrapDistance,25);
	//else
	return rfit;
}

//cancellation task
float ffitness_cancellationTask2(struct  individual  *pind)
{
	struct envobject *obj;
	int    ta,tt;
	float distTarget;
	if(pind->decision_units[0]>0.7) ipar->overThreshold++;
	float correctTarget=0;
	float wrongTarget=0;
	float totalCheckedTarget=0;
	float bootStrapDistance=-1;
	float rfit=0;
	

	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			
			
			if(obj->checked==0)
			{
				
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				if(distTarget<25) //it was 15
				{
					if(obj->visited==0)
					{
						
						//rfit+=10;
						if(obj->c[0]==1.0)
						rfit+=10;//+10*pind->decision_units[0];
						else
						rfit+=10;

						obj->visited=1;
						
					}

				
				//if (distTarget<15 && pind->zoom_motor[1]>0.7)
					//{
						if(pind->decision_units[0]>0.7)
						{
							obj->checked=1;
							if(obj->c[0]==1)
							{
							obj->x=4000;
							if(ipar->nTargetSetting==0)
							rfit+=1;
							if(ipar->nTargetSetting==1)
							{
								//if(cepoch%2==0)
								//rfit+=100*(float)envnobjs[SROUND]/2.0;
								//else
								 //rfit+=100;
								int t=cepoch%4;
								switch(t)
								{
								case 0:
									rfit+=1000;
									break;
								case 1:
									rfit+=500;
									break;
								case 2:
									rfit+=250;
									break;
								case 3:
									rfit+=125;
									break;
								}
							}
							}
							if(obj->c[0]==0.5) stop=1;
						}
						
				}
				
			}
	}
	
	
	return rfit;
}

float ffitness_cancellationTask3(struct  individual  *pind)
{
	struct envobject *obj;
	int    ta,tt;
	float distTarget;
	
	float correctTarget=0;
	float wrongTarget=0;
	float totalCheckedTarget=0;
	float bootStrapDistance=-1;
	float rfit=0;
	float gridx, gridy;
	int   igridx, igridy;

	gridx=(ipar->cRetinaX-(ipar->retinaX-ipar->retinaXrange))/(2*ipar->retinaXrange);
	gridy=(ipar->cRetinaY-(ipar->retinaY-ipar->retinaYrange))/(2*ipar->retinaYrange);
	igridx=(int)(gridx*10.0);
	igridy=(int)(gridy*10.0);
	if(pind->gridF[igridx][igridy]==0)
	{
		pind->gridF[igridx][igridy]=1;
		rfit+=10.0;
	}
	
	//here we compute the grid fitness

	for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			
			
			if(obj->checked==0)
			{
				
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				
				if(distTarget<25) //it was 15
				{
					
					if(obj->visited==0)
					{
						
						//rfit+=10;
						if(obj->c[0]==1.0)
						rfit+=0;//+10*pind->decision_units[0];
						else
						rfit+=0;

						obj->visited=1;
						
					}
					

				
				//if (distTarget<15 && pind->zoom_motor[1]>0.7)
					//{
						//if(4<1)
						if(pind->decision_units[0]>0.7)
						{
							obj->checked=1;
							if(obj->c[0]==1)
							{
								//for symmetry test
							if(ipar->trialScore==0)
								if(obj->x>400) ipar->firstChoice=1;
								else
								   ipar->firstChoice=0;
							//end symmetry test

							obj->x=4000;
							ipar->trialScore+=1;
							if(ipar->nTargetSetting==0)
							rfit+=1;
							if(ipar->nTargetSetting==1)
							{
								//if(cepoch%2==0)
								//rfit+=100*(float)envnobjs[SROUND]/2.0;
								//else
								 //rfit+=100;
								if (amoebaepochs==1500)exit(1);
								int t=cepoch%4;
								switch(t)
								{
								case 0:
									rfit+=(float)sweeps;
									
									//rfit+=1000;
									break;
								case 1:
									rfit+=(float)sweeps/2.0f;
									//rfit+=500;
									break;
								case 2:
									rfit+=(float)sweeps/4.0f;
									//rfit+=250;
									break;
								case 3:
									rfit+=(float)sweeps/8.0f;
									//rfit+=125;
									break;
								}
							}
							}
							if(obj->c[0]==0.5) stop=1;
						}
						
				}
				
			}
	}
	
	
	return rfit;
}

float ffitness_cancellationTask3Emotion(struct  individual  *pind)
{
	
	struct envobject *obj;
	int    ta,tt;
	float distTarget;
	
	if(pind->decision_units[0]>0.7) ipar->overThreshold++;
	float correctTarget=0;
	float wrongTarget=0;
	float totalCheckedTarget=0;
	float bootStrapDistance=-1;
	float rfit=0;
	float gridx, gridy;
	int   igridx, igridy;
	if(run==0) 
	{
		runcontrol=0;
		wrongcanc=0;
	}
	colorinput = 0.0;
	bool onTarget=false;

	gridx=(ipar->cRetinaX-(ipar->retinaX-ipar->retinaXrange))/(2*ipar->retinaXrange);
	gridy=(ipar->cRetinaY-(ipar->retinaY-ipar->retinaYrange))/(2*ipar->retinaYrange);
	igridx=(int)(gridx*10.0);
	igridy=(int)(gridy*10.0);
	if(pind->gridF[igridx][igridy]==0)
	{
		pind->gridF[igridx][igridy]=1;
		// modificato da Daniela 
		// rfit+=10.0;
		rfit+=0.0;
	}

	//here we compute the grid fitness
	//if(cepoch%8<2)
	if(cepoch%8<4)//spento cancella tutti
	{
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			if(obj->checked==0)
			{
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				if(distTarget<20) //it was 25
				{
					onTarget=true;
					//Daniela
					colorinput=obj->c[0];
					//end storing the color
					if(obj->visited==0)
					{
						obj->visited=1;
					}
					
					if(pind->decision_units[0]>0.7)
					{
						obj->checked=1;
						//if(obj->c[0]==1)
						//{
								//for symmetry test
							if(ipar->trialScore==0)
								if(obj->x>400) ipar->firstChoice=1;
								else
								   ipar->firstChoice=0;
							//end symmetry test

						obj->x=4000;
						ipar->trialScore+=1;
						if(ipar->nTargetSetting==0)
						{
							rfit+=1;
						}
						if(ipar->nTargetSetting==2)
						{
							rfit+=(float)sweeps/2.0; //perchè il massimo deve essere dato quando cancella tutti i 16 stimoli
						}
						if(ipar->nTargetSetting==1)
						{
							int t=cepoch%4;
							switch(t)
							{
								case 0:
									rfit+=(float)sweeps/2.0;
									//rfit+=1000;
									break;
								case 1:
									rfit+=(float)sweeps/4.0f;
									//rfit+=500;
									break;
								case 2:
									rfit+=(float)sweeps/8.0f;
									//rfit+=250;
									break;
								case 3:
									rfit+=(float)sweeps/16.0f;
									//rfit+=125;
									break;
							}
						}
							//}
							
							
					}
						
				}
				
			}
		}
	}
	if (cepoch%8>3)
		//if (cepoch%8>1) //dispari cancella solo i target
	{ 
		
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			if(obj->checked==0)
			{
				
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				
				if(distTarget<20) //it was 25
				{
					//Daniela
					colorinput=obj->c[0];
					//end storing the color
					if (wrongcanc>0 && obj->c[0]==0.5) fearTimer=20; //attiva il "fear" se lo stimolo dangerous e' percepito

					onTarget=true;
					if(obj->visited==0)
					{
						obj->visited=1;
					}
					if(pind->decision_units[0]>0.7)
					{
						obj->checked=1;

						obj->x=4000;
						if(obj->c[0]==1) //traget color=1
						{
							//for symmetry test
							if(ipar->trialScore==0)
								if(obj->x>400) ipar->firstChoice=1;
								else
								   ipar->firstChoice=0;
							//end symmetry test

						//	obj->x=4000;
							ipar->trialScore+=1;
							if(ipar->nTargetSetting==0)
							{
								rfit+=1;
							}
							if(ipar->nTargetSetting==2)
							{
								rfit+=(float)sweeps/2.0;	
							}
							if(ipar->nTargetSetting==1)
							{
								int t=cepoch%4;
								switch(t)
								{
								case 0:
									rfit+=(float)sweeps;
									
									//rfit+=1000;
									break;
								case 1:
									rfit+=(float)sweeps/2.0f;
									//rfit+=500;
									break;
								case 2:
									rfit+=(float)sweeps/4.0f;
									//rfit+=250;
									break;
								case 3:
									rfit+=(float)sweeps/8.0f;
									//rfit+=125;
									break;
								}
							}
						}
							
						//modificato da Daniela //distracter=0.5 

						if(obj->c[0]==0.5) 
						{
							//int fearrand = mrand(100);
							//fearTimer=fearrand + 20;
								
							//fearTimer=50; //currently used for evolution  
								
							//rfit-=2*(float)sweeps; 
								
							//runcontrol+=100; //currently used for evolution
							
							//stop = 1;
							wrongcanc++;
						    if (runcontrol > 1000) 
							{
								stop = 1;
							}
							if (wrongcanc > 2) 
							{
								//stop = 1; 
							}
						}
							 
							//if(obj->c[0]==0.5 & (cepoch%2==1)) stop=1;
					}
						
				}
				
			}

		}
	}
	
	if(pind->decision_units[0]>0.7 && !onTarget) //removes 250 life steps if the actuator is active on an empty area
	{ 
		int probstop = mrand(1000);
		if (probstop < 10)
		{
			//stop = 1;
		}
		//stop=1 ;
		//runcontrol+=250; //currently used for the evolution
	}
	/*if (runcontrol > 1000) 
	{
		stop = 1;
	}*/

	if(ipar->nTargetSetting==2 && (cepoch%8)>3)
	{
		if (run == (sweeps -1))
		{
			rfit += (8 - wrongcanc) * (float)sweeps/2.0;
		}
	}

	return rfit;
}

float ffitness_cancellationTaskSensation(struct  individual  *pind)
{
	
	struct envobject *obj;
	int    ta,tt;
	float distTarget;
	if(pind->decision_units[0]>0.7) ipar->overThreshold++;
	float correctTarget=0;
	float wrongTarget=0;
	float totalCheckedTarget=0;
	float bootStrapDistance=-1;
	float rfit=0;
	float gridx, gridy;
	int   igridx, igridy;
	if(run==0) 
	{
		runcontrol = 0;
		goodfood=0;
		badfood=0;
		correctfood=0;
		wrongfood=0;
		totalcanc = 0;
		targets = new int[envnobjs[SROUND]];
		if(logData ==3)
		{
			evencor = fopen("evencorrect.xls", "a");
			evenincor = fopen("evenincorrect.xls", "a");
			oddcor = fopen("oddcorrect.xls", "a");
			oddincor = fopen("oddincorrect.xls", "a");
			hyperact = fopen("bothunits.xls","a");
		}
		
	}
	colorinput = 0.0;
	bool onTarget=false;

	gridx=(ipar->cRetinaX-(ipar->retinaX-ipar->retinaXrange))/(2*ipar->retinaXrange);
	gridy=(ipar->cRetinaY-(ipar->retinaY-ipar->retinaYrange))/(2*ipar->retinaYrange);
	igridx=(int)(gridx*10.0);
	igridy=(int)(gridy*10.0);
	if(pind->gridF[igridx][igridy]==0)
	{
		pind->gridF[igridx][igridy]=1;
		rfit+=0.0;
	}
	int goodtargets = 0;
	 
	if (trialscount%30==0.0) //beginning of the generation
	{
		int divid = trialscount/30;
		if(divid%2 == 0)
		{
			even = true;
		}
		else 
		{
			even = false;
		}
	}
	
	if (run == 0)
		{
			//new condition

			//if (even) //even generation
			//{
			//	if (cepoch%3==0)//good food 
			//	{
			//		percentagegood = 50;
			//	}
			//	else //only bad food
			//	{	 
			//		percentagegood = 50;
			//	}
			//}
			//else
			//{
			//	if (cepoch%3==0)//good food 
			//	{
			//		percentagegood = 50;
			//	}
			//	else //only bad food
			//	{	 
			//		percentagegood = 50;
			//	}
			//}

			//old condition for test

			if (cepoch%2==0)//good food 
			{
				percentagegood = 75;
			}
			if (cepoch%2!=0) //only bad food
		
			{ 
				percentagegood = 25;
			}
			//END CONDITIONS
			if (percentagegood != 0)
			{
				goodtargets = ((envnobjs[SROUND] * percentagegood) /100);
			}
			else
			{ 
				goodtargets = 0;
			}
			for (int i = 0; i < envnobjs[SROUND]; i++)
			{
				if (i<goodtargets)
				{
					targets[i] = 1;
				}
				else
				{
					targets[i] = 0;
				}
			}
			std::random_shuffle(&targets[0],&targets[envnobjs[SROUND]]);
		}
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			if(obj->checked==0)
			{
				
				distTarget=mdist(obj->x,obj->y,ipar->cRetinaX,ipar->cRetinaY);
				//if(obj->c[1]==1) bootStrapDistance=distTarget;
				
				
				if(distTarget<20) //it was 25
				{
					onTarget=true;
					if(obj->visited==0)
					{
						obj->visited=1;
					}
							ipar->trialScore+=1;
							
							if(ipar->nTargetSetting==3)
							{
								if (pind->decision_units[0]>0.7 && pind->decision_units[1]<0.7) //good food unit activated
								{
									runcontrol += 1;
									
									if(targets[totalcanc] == 1) //it is good food
									{
										obj->checked=1; //the object gets cancelled and he gains fitness
										obj->x=4000;
										correctfood++;
										rfit+= (float)sweeps*(clocklogisticpos((run*10)/sweeps));
										//rfit+=(float)sweeps*(clocklogisticpos((correctfood*10)/envnobjs[SROUND]));
										totalcanc++;
										goodfood++;
										if(logData ==3)
										{
											if(cepoch%2 == 0) 
											{fprintf(evencor, "%d, 1, 0, %ld\n", trialscount,run);}
											else {fprintf(oddcor, "%d, 1, 0, %ld\n", trialscount,run);}
										}
									}
									else
									{
										obj->checked=1; //the object gets cancelled and he loses fitness
										obj->x=4000;
										wrongfood++;
										//rfit-= (float)sweeps*(clocklogisticpos((run*10)/sweeps));
										//rfit-=(float)sweeps*(logisticpos((wrongfood*10)/envnobjs[SROUND]));
										totalcanc++;
										//runcontrol+=(100*wrongfood);
										if(logData ==3)
										{
											if(cepoch%2 == 0) 
											{fprintf(evenincor, "%d, 0, 0, %ld\n", trialscount,run);}
											else {fprintf(oddincor, "%d, 0, 0, %ld\n", trialscount,run);}
										}
									}
								}
								else if (pind->decision_units[0]<0.7 && pind->decision_units[1]>0.7) //bad food unit activated
								{
									runcontrol += 1;

									if(targets[totalcanc] == 1) //it is good food
									{
										obj->checked=1; //the object gets cancelled and he loses fitness
										obj->x=4000;
										wrongfood++;
										//rfit-= (float)sweeps*(clocklogisticpos((run*10)/sweeps));
										//rfit-=(float)sweeps*(logisticpos((wrongfood*10)/envnobjs[SROUND]));
										totalcanc++;
										//runcontrol+=(100*wrongfood);
										if(logData ==3)
										{
											if(cepoch%2 == 0) 
											{fprintf(evenincor, "%d, 1, 1, %ld\n", trialscount,run);}
											else {fprintf(oddincor, "%d, 1, 1, %ld\n", trialscount,run);}
										}
									}
									else
									{
										obj->checked=1; //the object gets cancelled and he gains fitness
										obj->x=4000;
										correctfood++;
										rfit+= (float)sweeps*(clocklogisticpos((run*10)/sweeps));
										//rfit+=(float)sweeps*(clocklogisticpos((correctfood*10)/envnobjs[SROUND]));
										totalcanc++;
										badfood++;
										if(logData ==3)
										{
											if(cepoch%2 == 0) 
											{fprintf(evencor, "%d, 0, 1, %ld\n", trialscount,run);}
											else {fprintf(oddcor, "%d, 0, 1, %ld\n", trialscount,run);}
										}
									}

								}
								else if (pind->decision_units[0]>0.7 && pind->decision_units[1]>0.7) //both units activated
								{
									runcontrol +=1; //loses steps
									if(logData ==3) {fprintf(hyperact, "%d, %ld\n", trialscount,run);}
								}
								//else //the robot ignores the target - no unit activated
								//{
								//	runcontrol +=200;
								//	//rfit-=(float)sweeps; //loses steps and fitness
								//}
							}
					}
					
						
				}
				
			}

			
	if(!onTarget) //removes 100 life steps if the actuator is active on an empty area
	{ 
		if (pind->decision_units[0]>0.7)
		{
			runcontrol +=1;
		}
		if (pind->decision_units[1]>0.7)
		{
			runcontrol +=1;
		}
	}
	if ((run + runcontrol) > (sweeps-1))
	{
		stop = 1;
	}
	if (stop ==1 || run == (sweeps -1))
	{ 
		delete [] targets;
		if (rfit == 0.0)
		{
			rfit -= float(sweeps);
		}
		trialscount++;
		//fprintf(history, "\n");
		if(logData==3)
			{
				fclose(evencor);
				fclose(evenincor);
				fclose(oddcor);
				fclose(oddincor);
				fclose(hyperact);
		}
		
	}

	return rfit;
}

void initialize_robot_cancellationTask3()
{
	 struct individual *pind;
      int    team;
	  //initialize retina robot variable
	ipar->cRetinaX=ipar->retinaX;
	ipar->cRetinaY=ipar->retinaY;
	ipar->cRetinaSize=ipar->retinaSize;
	
	ipar->cRetinaYold=ipar->cRetinaY;
	ipar->cRetinaXold=ipar->cRetinaX;
	ipar->cRetinaSizeold=ipar->cRetinaSize;

	if(xMuscle!=NULL)
	{
	//xMuscle->resetMuscle();
	//yMuscle->resetMuscle();
	//zMuscle->resetMuscle();
	}
	  
	  
	  initialize_robot_position();

    for(team=0,pind=ind; team < nteam; team++, pind++)
	{
		for(int r=0;r<10;r++)
			for(int c=0;c<10;c++)
				pind->gridF[r][c]=0;
	}

}
void initialize_world_cancellationTask()
{
	//Initializing position of 8 objects
	struct envobject *obj, *obj1;
	int    ta,tt;
	int tab;
	float dxy;
	//init retina
	ipar->cRetinaX=ipar->retinaX;
	ipar->cRetinaY=ipar->retinaY;
	ipar->cRetinaSize=ipar->retinaSize;
	ipar->overThreshold=0;
	ipar->cRetinaReceptiveField=ipar->retinaReceptiveField;
	//end initialization retina parameters, to be put in a specific function!

	float pi=3.141592;
	int target1, target2;
	target1=mrand(4);
	target2=mrand(4);
	float setDist=0;
	float mindist=1000;
	bool dispOk=false;
	int targetsetting=0; //0 per le evoluzioni 2 per test simmetria
	//ipar->trialScore=0;
	//int nTargetSetting=0;
	ipar->firstChoice=-1;
	fearTimer=0;
	
	
		if(targetsetting==1)
		{
			int col;
			int row;
			dxy=sqrt((float)envnobjs[SROUND]);
			 col=(int)dxy;
			if((float)col==dxy) row=col;
			else
				row=col+1;
			float dx=ipar->retinaSize*0.7/(float)col;
			float dy=ipar->retinaSize*0.7/(float)row;
			
			for(int r=0;r<row;r++)
				for(int c=0;c<col;c++)
				{
					int index=row*r+c;
					if (index<envnobjs[SROUND])
					{
						obj=*(envobjs + SROUND)+index;
						obj->x=ipar->retinaX+dx*r-ipar->retinaSize*0.7/2.0+dx/2.0;
						obj->y=ipar->retinaY+dy*c-ipar->retinaSize*0.7/2.0+dy/2.0;
					}
				}
				
			
		
		}
	

		
		//we randomize round object position to the rigth and to the left
		if(targetsetting == 0)
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			
			dispOk=false;
			if(ta==0)
			{
			obj->x=rans(110)+ipar->retinaX;//cos((pi/4.0)*ta)*80+ipar->retinaX;
			obj->y=rans(110)+ipar->retinaY;//sin((pi/4.0)*ta)*80+ipar->retinaY;
			}
			else
			{
			
				while(!dispOk)
				{
				obj->x=rans(110)+ipar->retinaX;//cos((pi/4.0)*ta)*80+ipar->retinaX;
				obj->y=rans(110)+ipar->retinaY;//sin((pi/4.0)*ta)*80+ipar->retinaY;
				dispOk=true;
				for ( tab=0, obj1 = *(envobjs + SROUND); tab<ta; tab++, obj1++)
						{
							
							if(mdist(obj->x,obj->y,obj1->x,obj1->y)<40) dispOk=false; //it was 25 45
							

						}
				}
			}
			

				

		}
		if(targetsetting==2)
		{
			
			//it needs 400 trial (20x20 positions tested)
			float tcepoch=floor(cepoch/20.0);
			float d=(float)(cepoch%20)*90.0/20.0;//mrand(90);
			float dy =-100+(float)tcepoch*200.0/20.0; //rans(100);
			for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
			{
				if((ta%2)==0)
				obj->x=ipar->retinaX+d+10;
				else
				obj->x=ipar->retinaX-d-10;

				obj->y=ipar->retinaY+dy;
			}
		}


			//setting target iteme randomly
		for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
		{
			obj->checked=0;
			obj->visited=0;
			//if(ta==target1 || ta==target2)
			if(ipar->nTargetSetting==0)
				if(mrand(100)<40)
				{
					obj->c[0]=1.0;
					obj->c[1]=0.0;
				}
				else
				{
					obj->c[0]=0.5;
					obj->c[1]=0.0;
				}
			/*if(ipar->nTargetSetting==1)
			{
				if(ta%2==0)
				{
					obj->c[0]=1.0;
					obj->c[1]=0.0;
				}else
				{
					obj->c[0]=0.5;
					obj->c[1]=0.0;
				}
				if(cepoch%2==0 && ta>1)
					obj->x=4000;
			}*/
				if(ipar->nTargetSetting==1)
				{
					int t=cepoch%4;
					if(ta%2==0)
				{
					obj->c[0]=1.0;
					obj->c[1]=0.0;
				}else
				{
					obj->c[0]=0.5;
					obj->c[1]=0.0;
				}
					
					switch(t)
					{
					case 0:
						if(ta>1)obj->x=4000;
						break;
					case 1:
						if(ta>3)obj->x=4000;
						break;
					case 2:
						if(ta>7)obj->x=4000;
						break;
					case 3:
						//we take all target that it is suppose to be 16
						break;
					}
				}

				if(ipar->nTargetSetting==2)
				{
					
					if(ta%2==0)
				{
					obj->c[0]=1.0;
					obj->c[1]=0.0;
				}else
				{
					obj->c[0]=0.5;
					obj->c[1]=0.0;
				}
					
					
				}
				if(ipar->nTargetSetting==3) //(Daniela)
				{
					obj->c[0]=0.5;
					obj->c[1]=0.0;
				}



			
		}
	

}
void initialize_step_cancellationTask(int step, int epoch)
{
	

}
/*
 * garbage
 * 1 for holding an object, 10000 for releasing it outside, -1 for crashes
 */
float
ffitness_garbage(struct  individual  *pind)

{

  if (pind->jcrashed == 1)
    return(-1.0);

  if (pind->grip_obreleased > 0)
    {
     pind->grip_obreleased = 0;
     return(10000);
    }
  if (pind->grip_obtaken == 1)
    return(1.0);

  return(0.0);
}

/*
 * garbage
 * re-initialize the characteristics of the environment
 */
void
initialize_world_garbage()


{

    int    i;
    int    ta,tt;
    int    tw;
    double d,mind;

    double w1, w2;
    int    volte;
    double min_dist_ob;
    struct envobject *obj;
    struct envobject *objb;

    // we randomize sround positions
    if (random_round == 1)
    {
      for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
        {
          mind = 0.0;
          volte = 0;
          min_dist_ob = 150;
          while (mind < min_dist_ob && volte < 1000)
          {
            volte++;
            if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
            obj->x = (float) (fabs(rans((float) envdx - (double) (120 * 2))) + 120);
            obj->y = (float) (fabs(rans((float) envdy - (double) (120 * 2))) + 120);
            for (tt=0, objb = *(envobjs + SROUND), mind = 9999.9; tt < ta; tt++, objb++)
            {
             d = mdist(objb->x, objb->y, obj->x, obj->y);
             if (d < mind)
               mind = d;
            }

          }
        }
    }

    // we randomize round positions
    if (random_round == 1)
    {
      for (ta=0, obj = *(envobjs + ROUND); ta<envnobjs[ROUND]; ta++, obj++)
      {
        mind = 0.0;
        volte = 0;
        min_dist_ob = 150;
        while (mind < min_dist_ob && volte < 1000)
        {
          mind = 9999.9;
          volte++;
          if ((volte > 199) && (volte % 10) == 0)
              min_dist_ob -= 1;
          obj->x = (float) (fabs(rans((float) envdx - 275)) + 132.0);
          obj->y = (float) (fabs(rans((float) envdy - 275)) + 132.0);
          for (tt=0, objb = *(envobjs + 2); tt < ta; tt++, objb++)
            {
              d = mdist(objb->x, objb->y, obj->x, obj->y);
              if (d < mind)
                mind = d;
            }
          for (tt=0, objb = *(envobjs + 1); tt < envnobjs[1]; tt++, objb++)
            {
              d = mdist(objb->x,objb->y,obj->x,obj->y);
              if (d < mind)
                mind = d;
            }
        }
      }
    }

}

//*************************************************************
// explore
//************************************************************* 

int worldgrid[100][100]; // the environment is divided in 100x100mm virtual cells 
int cellsize=100;        // the size of the cells

/*
 * re-initialize the grid of visited cells
 */
void
initialize_world_explore()

{
    int    gx,gy;

    for(gx=0; gx < ((envdx / cellsize) + 1); gx++)
     for(gy=0; gy < ((envdy / cellsize) + 1); gy++)
       worldgrid[gx][gy] = 0;
       

}


/*
 * set the initial position of the robots
 * default function (may be overwritten by experiment specific functions)
 */
void
initialize_robot_position_explore()
{

    struct individual *pind;
    int    volte;
    int    startset;
    int    team;


    for(team=0,pind=ind; team < nteam; team++, pind++)
     if (pind->dead == 0)
     {
          pind->pos[0]    = 250 + mrand(envdx - 500);
          pind->pos[1]    = 250 + mrand(envdy - 500);
          pind->direction = (double) mrand(360);
          volte = 0;
          while (volte < 1000 && ((check_crash(pind->pos[0],pind->pos[1]) == 1)))
          {
            volte++;
            pind->pos[0]    = 250 + mrand(envdx - 500);
            pind->pos[1]    = 250 + mrand(envdy - 500);
            pind->direction = (double) mrand(360);
          }
     }


}
/*
 * the robot is rewarded for exploring the environment (i.e. visiting cells not previously explored)
 * when the enrgysensor is defined, the robot should also reacharge its battery levels
 * in gray areas to not run out of energy
 */
float ffitness_explore(struct  individual  *pind)
{

  float colorg;
  float fit;

  fit = 0.0;
  colorg = 1.0 - pind->ground[0];

  if (ipar->energy == 1)
   {
    // the robot consumes energy while it moves proportionally to the speed of its wheels
    pind->energylevel -= (1.0f / 1000.0f) * ((float) (abs(pind->speed[0]) + abs(pind->speed[1])) / 20.0);
    if (pind->energylevel <= 0)
      pind->energylevel = 0.0;

    // the robot recharge while it is in the recharging grey area 
    if (colorg > 0.25 && colorg < 0.75)
      pind->energylevel += 0.05;
    if (pind->energylevel > 1.0)
      pind->energylevel = 1.0;

    if (pind->energylevel <= 0) 
       stop = 1;
   } 
  // fitness increase of 1 point for each new cell visited (if ground is white)
 if (colorg > 0.25 && worldgrid[(int) (pind->pos[0]/cellsize)][(int) (pind->pos[1]/cellsize)] == 0)
    {
      worldgrid[(int)(pind->pos[0]/cellsize)][(int)(pind->pos[1]/cellsize)] = 1;
      return(1.0);
    }
    else
    {
      return(0.0);
    }

}