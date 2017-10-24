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
 * parameters.c
/*
 */

#include "defs.h"
#include "public.h"

struct Parameter {
    char   *name;                 // the name of the parameter
    char   *folder;               // the folder of the parameter
    int     type;		  // the type, 0=int 
    int     n1;                   // the minimum value (for int only)
    int     n2;		          // the maximum value (for int only)
    int    *vpointer;             // the pointer to the parameter
    struct Parameter *next;       // the next parameter of the same folder
    struct Parameter *nextfolder; // the first parameter of the next folder
    char   *description;          // parameter description
};

struct Parameter *parlist = NULL;  // parameters list


#include <iostream>

int prova()
{
	void *ptr;
	int *iptr;
	int i, j;

	i = 10;
	j = 0;
	ptr = &i;

	iptr = (int *) ptr;

	j = *iptr;

	std::cout << "j = " << j << std::endl;

	return 0;
}



/*
 * This function create a new parameter
 * if the parameter have been already created,
 *  it modifies the value and eventually the limits
 * type, 0=int, 1=float, 2=char, 3=int-M, 4=float-M, 5=char-M
 */
void create_parameter(char *name, char *folder, void *vpointer, int type, int n1, int n2, char *description)

{
    struct Parameter *cfolder;
    struct Parameter *cpar;
    struct Parameter *lastfolder;
    struct Parameter *lastpar;
    int folderfound;
    int parfound;
    int i;
    char emessage[64];

    cfolder = parlist;
    lastfolder = cfolder; 
    folderfound = 0;
    while(cfolder != NULL && folderfound == 0)
     {
       if (strcmp(cfolder->folder, folder) == 0)
        {
         folderfound = 1;
        }
       else
        {
         lastfolder = cfolder;
         cfolder = cfolder->nextfolder;
        }
     }

     parfound = 0;
     if (folderfound == 1)
        {
	 cpar = cfolder;
         while(cpar != NULL && parfound == 0)
          {
            if (strcmp(cpar->name, name) == 0)
             {
              parfound = 1;
             }
	    else
             {
	      lastpar = cpar;
              cpar = cpar->next;
             }
          }
        }

     if (parfound == 0)
       {
        cpar = (struct Parameter  *) malloc (sizeof (struct Parameter));
        cpar->name   = (char *) malloc((unsigned)(strlen(name) + 1)); // +1 for trailing NULL
        cpar->folder = (char *) malloc((unsigned)(strlen(folder) + 1));
        cpar->description = (char *) malloc((unsigned)(strlen(description) + 1));
       }

     strcpy(cpar->name, name);
     strcpy(cpar->folder, folder);
     strcpy(cpar->description, description);
     cpar->type = type;
     cpar->n1 = n1;
     cpar->n2 = n2;
     cpar->next = NULL;
     cpar->nextfolder = NULL;
     if (type == 0)
	cpar->vpointer = (int *) vpointer;


     if (parlist == NULL)      // the first parameter and folder has been created
          parlist = cpar;
       else
         if (folderfound == 0) // an additional folder created
           lastfolder->nextfolder = cpar;
	  else
	    if (parfound == 0)
	      lastpar->next = cpar; // an additional param. created
}

/*
 * This function modify the value of a previously defined parameter
 */
void set_parameter(char *name, char *folder, int value)

{
    struct Parameter *cfolder;
    struct Parameter *cpar;
    int folderfound;
    int parfound;
    int i;
    char emessage[64];

    cfolder = parlist;
    folderfound = 0;
    while(cfolder != NULL && folderfound == 0)
     {
       if (strcmp(cfolder->folder, folder) == 0)
         folderfound = 1;
       else
         cfolder = cfolder->nextfolder;
     }

     if (folderfound == 0)
	{
	 sprintf(emessage, "set parameter: parameter %s->%s not found", folder, name);
	 display_warning(emessage);
	 return;
        }

     parfound = 0;
     if (folderfound == 1)
        {
	 cpar = cfolder;
         while(cpar != NULL && parfound == 0)
          {
            if (strcmp(cpar->name, name) == 0)
              parfound = 1;
	    else
              cpar = cpar->next;
          }
        }

     if (folderfound == 0)
	{
	 sprintf(emessage, "set parameter: parameter %s->%s not found", folder, name);
	 display_warning(emessage);
	 return;
        }

    if (value < cpar->n1 || value > cpar->n2)
	{
	 sprintf(emessage, "parameter should be within the [%d-%d] range", cpar->n1, cpar->n2);
	 display_warning(emessage);
	 return;
        }

    *cpar->vpointer = value;
}

/*
 * search and return the variable with a given name
 */
struct Parameter *lookup_var(char *s)
{
    struct Parameter *sp;

    for (sp = parlist; sp != (struct Parameter  *) NULL; sp = sp->next) {
	if (strcmp(sp->name, s) == 0)
	    return sp;
    }
    return NULL;		/* 0 ==> not found */
}


/*
 * parse evolution parameters
 * all parameters have 1 arguments
 * 0 means off >0 means on
 * we check whether the parameter exist and the number of arguments
 */
int
parse_evolution_parameter(char *st)

{

        struct  iparameters *pipar;
	int     narguments;
	int     nc;
	int     done;
	char    word[128];
	char    message[128];
	char    wordb[128];
	char    stb[128];

	pipar = ipar;
	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');
	while(*st == ' ')
           st++;

	if (strcmp(word, "npopulations") == 0)
	{
	   npopulations = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nteam") == 0)
	{
	   nteam = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "homogeneous") == 0)
	{
	   homogeneous = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "ngenerations") == 0)
	{
	   generations = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "startgeneration") == 0)
	{
	   startgeneration = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nreplications") == 0)
	{
	   nreplications = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nreproducing") == 0)
	{
	   bestfathers = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "offspring") == 0)
	{
	   nkids = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "mutation_rate") == 0)
	{
	   percmut = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "elitism") == 0)
	{
	   elitism = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "seed") == 0)
	{
	   seed = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "pop_seed") == 0)
	{
	   pop_seed = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "savenbest") == 0)
	{
	   savebest = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "saveifit") == 0)
	{
	   saveifit = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "additional_ind") == 0)
	{
	   additional_ind = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "learningc") == 0)
	{
	   learningc = atoi(st);
	   done = 1;
	}

	sprintf(wordb,"%s", word);
	sprintf(stb,"%s", st);
	narguments = 0;
        while (getword(wordb,stb,' ') > 0 && narguments < 2)
	   {
	     narguments++;
	   }

	if (done == 0)
	{
	  sprintf(message, "parameter evolution.%s unknown", word);
	  display_error(message);
	}
	if (done == 1 && narguments != 1)
	{
	  done = 0;
	  sprintf(message, "wrong number of arguments for parameter evolution %s ", word);
	  display_error(message);
	}

	return(done);

}
int parse_robot_connection(char *st)
{
	int noRobot, noComPort;
	sscanf(st,"%d %d\r\n",&noRobot, &noComPort);
	noRobot=noRobot;
	noComPort=noComPort;
	if (real==1 && noRobot<8 && noRobot >=0)
	{
	connectRobot(&robots[noRobot],noComPort);
	}else
	{
	display_warning("Connection failed!");
	}
	return 1;

}


/*
 * display individual parameters
 */
void
display_evolution_parameters()

{

	char    message[256];

	helptn = 1;

	display_message("evolution");

	sprintf(message,"npopulations %d / number of evolving populations", npopulations);
	display_message(message);

	if (nteam > 0 || verbose > 0)
	{
	sprintf(message,"nteam %d / number of individuals forming a team", nteam);
	display_message(message);
	sprintf(message,"homogeneous %d / [0/1] whether the team is genetically homogeneous", homogeneous);
	display_message(message);
	}

	sprintf(message,"ngenerations %d / number of generations", generations);
	display_message(message);

	sprintf(message,"learningc %d / number of learning cycles", learningc);
	display_message(message);

	if (startgeneration > 0 || verbose > 0)
	{
	sprintf(message,"startgeneration %d / number of the initial generation", startgeneration);
	display_message(message);
	}

	if (nreplications > 1 || verbose > 0)
	{
	sprintf(message,"nreplications %d / number of replication of the experiment", nreplications);
	display_message(message);
	}

	sprintf(message,"nreproducing %d / number of individuals allowed to reproduce", bestfathers);
	display_message(message);

	sprintf(message,"offspring %d / number of offspring for reproducing individuals", nkids);
	display_message(message);

	sprintf(message,"mutation_rate %d / percentage of bits that replaced with new random value", percmut);
	display_message(message);

	if (elitism > 0 || verbose > 0)
	{
	sprintf(message,"elitism  %d / [1/2] 1=elitism 2=elitism and preservation of previous fitness data", elitism);
	display_message(message);
	}

	sprintf(message,"seed %d / seed of the random number generators", seed);
	display_message(message);

	sprintf(message,"pop_seed  %d / whether seed is re-initialize for population individuals", pop_seed);
	display_message(message);

	sprintf(message,"savenbest %d / number of best individual saved for each generation", savebest);
	display_message(message);

	sprintf(message,"saveifit %d / save individual fitness", savebest);
	display_message(message);

	sprintf(message,"additional_ind %d / number of additional individuals to be allocated", additional_ind);
	display_message(message);

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}

}

/*
 * parse individual parameters
 * all parameters have 1 arguments
 * 0 means off >0 means on
 * we check whether the parameter exist and the number of arguments
 */
int
parse_ind_parameter(char *st)

{

        struct  iparameters *pipar;
	int     narguments;
	int     nc;
	int     done;
	char    word[128];
	char    message[128];
	char    wordb[128];
	char    stb[128];

	pipar = ipar;
	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');
	while(*st == ' ')
           st++;

	if (strcmp(word, "nhiddens") == 0)
	{
	   pipar->nhiddens = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "wrange") == 0)
	{
	   wrange = (float) atof(st);
	   done = 1;
	}
	if (strcmp(word, "rec_hiddens") == 0)
	{
	   pipar->dynrechid = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "rec_outputs") == 0)
	{
	   pipar->dynrecout = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "input_output_c") == 0)
	{
	   pipar->dynioconn = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "delta_inputs") == 0)
	{
	   pipar->dyndeltai = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "delta_hiddens") == 0)
	{
	   pipar->dyndeltah = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "delta_outputs") == 0)
	{
	   pipar->dyndeltao = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nifsensors") == 0)
	{
	   pipar->nifsensors = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nlightsensors") == 0)
	{
	   pipar->lightsensors = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "camerasensors") == 0)
	{
	   pipar->cameraunits = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "camerarange") == 0)
	{
	   pipar->camerarange = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "motsensors") == 0)
	{
	   pipar->motmemory = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "groundsensor") == 0)
	{
	   pipar->groundsensor = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "a_groundsensor") == 0)
	{
	   pipar->a_groundsensor = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "a_groundsensor2") == 0)
	{
	   pipar->a_groundsensor2 = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "elmansensors") == 0)
	{
	   pipar->elman_net = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "input_outputs") == 0)
	{
	   pipar->input_outputs = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signal_sensors") == 0)
	{
	   ninput_other = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signalss") == 0)
	{
	   pipar->signalss = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signalso") == 0)
	{
	   pipar->signalso = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signalssf") == 0)
	{
	   pipar->signalssf = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signalssb") == 0)
	{
	   pipar->signalssb = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "signaltype") == 0)
	{
	   pipar->signaltype = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "motors") == 0)
	{
	   pipar->wheelmotors = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "arbitration") == 0)
	{
	   pipar->arbitration = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "simplevision") == 0)
	{
	   pipar->simplevision = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nutrients") == 0)
	{
	   pipar->nutrients = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "compass") == 0)
	{
	   pipar->compass = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "energy") == 0)
	{
	   pipar->energy = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "ffitness") == 0)
	{
	   pipar->ffitness = atoi(st);
	   sprintf(ffitness_descp,"fitness n. %d", ipar->ffitness);
	   display_fit_parameters(ipar->ffitness);
	   done = 1;
	}

	sprintf(wordb,"%s", word);
	sprintf(stb,"%s", st);
	narguments = 0;
        while (getword(wordb,stb,' ') > 0 && narguments < 2)
	   {
	     narguments++;
	   }

	if (done == 0)
	{
	  sprintf(message, "parameter individual.%s unknown", word);
	  display_error(message);
	}
	if (done == 1 && narguments != 1)
	{
	  done = 0;
	  sprintf(message, "wrong number of arguments for parameter individual.%s ", word);
	  display_error(message);
	}


	return(done);

}


/*
 * display individual parameters (of individual ni)
 */
void
display_ind_parameters(int ni)

{

        struct  iparameters *pipar;
	char    message[256];

	pipar = ipar;

	helptn = 1;

	display_message("individual");

	sprintf(message,"ffitness %d / number of the fitness function", pipar->ffitness);
	display_message(message);

	sprintf(message,"motors %d / [2-3] two motor neurons controlling the two wheels", pipar->wheelmotors);
	display_message(message);

	if (pipar->nifsensors > 0 || verbose > 0)
	{
	sprintf(message,"nifsensors %d / [8,6,4] number of infrared sensors", pipar->nifsensors);
	display_message(message);
	}

	if (pipar->lightsensors > 0 || verbose > 0)
	{
	sprintf(message,"nlightsensors %d  / [8,3,2] number of light sensors", pipar->lightsensors);
	display_message(message);
	}

	if (pipar->cameraunits > 0 || verbose > 0)
	{
	sprintf(message,"camerasensors %d / number of photoreceptor of the linear camera", pipar->cameraunits);
	display_message(message);
	sprintf(message,"camerarange %d / angular range of the linear camera", pipar->camerarange);
	display_message(message);
	}
	if (pipar->motmemory > 0 || verbose > 0)
	{
	sprintf(message,"motsensors %d / [0-1] sensors of the state of the two motors at time t-1", pipar->motmemory);
	display_message(message);
	}
	if (pipar->groundsensor > 0 || verbose > 0)
	{
	sprintf(message,"groundsensor %d / n. sensors that binarily detect whether the robot is on a target area", pipar->groundsensor);
	display_message(message);
	}
	if (pipar->a_groundsensor > 0 || verbose > 0)
	{
	sprintf(message,"a_groundsensor %d / additional ground sensors which encode the previous state of ground sensors", pipar->a_groundsensor);
	display_message(message);
	}
	if (pipar->a_groundsensor2 > 0 || verbose > 0)
	{
	sprintf(message,"a_groundsensor2 %d / additional ground sensors which encode the previous state of ground sensors", pipar->a_groundsensor2);
	display_message(message);
	}
	if (pipar->elman_net > 0 || verbose > 0)
	{
	sprintf(message,"elmansensors %d / [0-1] sensors of the state of internal neurons at time t-1", pipar->elman_net);
	display_message(message);
	}

	if (pipar->input_outputs > 0 || verbose > 0)
	{
	sprintf(message,"input_outputs %d / additional inputs and outputs, inputs encode the state of the outputs at time t-1", pipar->input_outputs);
	display_message(message);
	}
	if (ninput_other > 0 || verbose > 0)
	{
	sprintf(message,"signal_sensors %d / [0-1] 4 sensors that detect the signal produced by other nearby robots", ninput_other);
	display_message(message);
	}
	if (pipar->signalss > 0 || verbose > 0)
	{
	sprintf(message,"signalss %d / [1/4] n. of sensors detecting signals", pipar->signalss);
	display_message(message);
	}
	if (pipar->signalso > 0 || verbose > 0)
	{
	sprintf(message,"signalso %d / [1/4] n. of output producing signals", pipar->signalso);
	display_message(message);
	}
	if (pipar->signalssf > 0 || verbose > 0)
	{
	sprintf(message,"signalssf %d / n. of phonemes in which a signal is segmented", pipar->signalssf);
	display_message(message);
	}
	if (pipar->signalssb > 0 || verbose > 0)
	{
	sprintf(message,"signalssb %d / [0/1] binary signals", pipar->signalssb);
	display_message(message);
	}
	if (pipar->signaltype > 0 || verbose > 0)
	{
	sprintf(message,"signaltype %d / [0/1] type of signal 0=sound 1=light", pipar->signaltype);
	display_message(message);
	}
        sprintf(message,"wrange %.2f / range of the weights and biases", wrange);
	display_message(message);

        sprintf(message,"nhiddens %d / number of internal neurons", pipar->nhiddens);
	display_message(message);

	if (pipar->dynrechid > 0 || verbose > 0)
	{
        sprintf(message,"rec_hiddens %d / [0-1] internal neurons have recurrent connections", pipar->dynrechid);
	display_message(message);
	}
	if (pipar->dynrecout > 0 || verbose > 0)
	{
        sprintf(message,"rec_outputs %d / [0-1] motor neurons have recurrent connections", pipar->dynrecout);
	display_message(message);
	}
	if (pipar->dynioconn > 0 || verbose > 0)
	{
        sprintf(message,"input_output_c %d / [0-1] we have direct sensory-motor connections", pipar->dynioconn);
	display_message(message);
	}
	if (pipar->dyndeltai > 0 || verbose > 0)
	{
        sprintf(message,"delta_inputs %d / [0-1] we have delta on sensory neurons", pipar->dyndeltai);
	display_message(message);
	}
	if (pipar->dyndeltah > 0 || verbose > 0)
	{
        sprintf(message,"delta_hiddens %d / [0-1] we have delta on internal neurons", pipar->dyndeltah);
	display_message(message);
	}
	if (pipar->dyndeltao > 0 || verbose > 0)
	{
	sprintf(message,"delta_outputs %d / [0-1] we have delta on motor neurons", pipar->dyndeltao);
	display_message(message);
	}
	if (pipar->arbitration > 0 || verbose > 0)
	{
	sprintf(message,"arbitration %d / number of arbitration units", pipar->arbitration);
	display_message(message);
	}
	if (pipar->simplevision > 0 || verbose > 0)
	{
	sprintf(message,"simplevision %d / [0-1-2] angle of visually detected objects", pipar->simplevision);
	display_message(message);
	}
	if (pipar->nutrients > 0 || verbose > 0)
	{
	sprintf(message,"nutrients %d / [0-1] proprioceptors of ingested nutrients", pipar->nutrients);
	display_message(message);
	}
	if (pipar->compass > 0 || verbose > 0)
	{
	sprintf(message,"compass %d / [0-1] compass", pipar->compass);
	display_message(message);
	}
	if (pipar->energy > 0 || verbose > 0)
	{
	sprintf(message,"energy %d / [0-1] the time passed since the last visited area", pipar->energy);
	display_message(message);
	}

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}


}




/*
 * parse lifetime parameters
 * all parameters have 1 arguments
 * 0 means off >0 means on
 * we check whether the parameter exist and the number of arguments
 */
int
parse_lifetime_parameter(char *st)

{

	int     narguments;
	int     nc;
	int     done;
	char    word[128];
	char    message[128];
	char    wordb[128];
	char    stb[128];

	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');
	while(*st == ' ')
           st++;

	if (strcmp(word, "ntrials") == 0)
	{
	   amoebaepochs = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "ncycles") == 0)
	{
	   sweeps = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "nworlds") == 0)
	{
	   nworlds = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "timelaps") == 0)
	{
	   timelaps= atoi(st);
	   done = 1;
	}
	if (strcmp(word, "sensornoise") == 0)
	{
	   sensornoise = (float) atof(st);
	   done = 1;
	}
	if (strcmp(word, "signal_mdist") == 0)
	{
	   max_dist_attivazione = (float) atof(st);
	   done = 1;
	}
	if (strcmp(word, "stop_on_crash") == 0)
	{
	   stop_when_crash = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "random_tareas") == 0)
	{
	   random_foodzones = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "startfarfrom") == 0)
	{
	   startfarfrom = atoi(st);
	   done = 1;
	}
	sprintf(wordb,"%s", word);
	sprintf(stb,"%s", st);
	narguments = 0;
        while (getword(wordb,stb,' ') > 0 && narguments < 2)
	   {
	     narguments++;
	   }

	if (done == 0)
	{
	  sprintf(message, "parameter lifetime.%s unknown", word);
	  display_error(message);
	}
	if (done == 1 && narguments != 1)
	{
	  done = 0;
	  sprintf(message, "wrong number of arguments for parameter lifetime.%s ", word);
	  display_error(message);
	}


	return(done);

}


/*
 * display individual parameters
 */
void
display_lifetime_parameters()

{

	char    message[256];


	helptn = 1;

	display_message("lifetime");

	sprintf(message,"ntrials %d / number of trials", amoebaepochs);
	display_message(message);

	sprintf(message,"ncycles %d / number of cycles for each trial", sweeps);
	display_message(message);

	sprintf(message,"nworlds %d / number of worlds", nworlds);
	display_message(message);

	sprintf(message,"timelaps %d / duration of a cycle in hundreds of milliseconds", timelaps);
	display_message(message);

	sprintf(message,"sensornoise %.3f / percentage of noise on infrared and light sensors", sensornoise);
	display_message(message);

	sprintf(message,"signal_mdist %.1f / range of communication sensors", max_dist_attivazione);
	display_message(message);

	sprintf(message,"stop_on_crash %d / [0-1] trials end when one robot collide", stop_when_crash);
	display_message(message);

	sprintf(message,"random_tareas %d / [0-1] randomize foodzone positions", random_foodzones);
	display_message(message);

	sprintf(message,"startfarfrom %d / [0-1] start far from targets and obstacles", startfarfrom);
	display_message(message);

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}

}



/*
 * parse display parameters
 * all parameters have 1 arguments
 * 0 means off >0 means on
 * we check whether the parameter exist and the number of arguments
 */
int
parse_display_parameter(char *st)

{

	int     narguments;
	int     nc;
	int     done;
	char    word[128];
	char    message[128];
	char    wordb[128];
	char    stb[128];

	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');
	while(*st == ' ')
           st++;

	if (strcmp(word, "drawtrace") == 0)
	{
	   drawtrace = atoi(st);
	   if (drawtrace > 1000)
	     drawtrace = 1000;
	   done = 1;
	}

	if (strcmp(word, "pause") == 0)
	{
	   timeadjust = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "drawneurons") == 0)
	{
	   drawneurons = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "drawnstyle") == 0)
	{
	   drawnstyle = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "drawn_allt") == 0)
	{
	   drawn_alltogether = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "dindividual") == 0)
	{
	   dindividual = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "cindividual") == 0)
	{
	   cindividual = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "drawneuronsdx") == 0)
	{
	   drawneuronsdx = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "grid") == 0)
	{
	   grid = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "play_sound") == 0)
	{
	   play_sound = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "verbose") == 0)
	{
	   verbose = atoi(st);
	   done = 1;
	}
	sprintf(wordb,"%s", word);
	sprintf(stb,"%s", st);
	narguments = 0;
        while (getword(wordb,stb,' ') > 0 && narguments < 2)
	   {
	     narguments++;
	   }

	if (done == 0)
	{
	  sprintf(message, "parameter display.%s unknown", word);
	  display_error(message);
	}
	if (done == 1 && narguments != 1)
	{
	  done = 0;
	  sprintf(message, "wrong number of arguments for parameter display.%s ", word);
	  display_error(message);
	}


	return(done);

}


/*
 * display individual parameters
 */
void
display_display_parameters()

{

	char    message[256];

	helptn = 1;

	display_message("display");

	if (drawtrace > 0 || verbose > 0)
	{
	sprintf(message,"drawtrace %d / draw robots' trace for n. cycles (max 1000)", drawtrace);
	display_message(message);
	}

	if (drawneurons > 0 || verbose > 0)
	{
	sprintf(message,"drawneurons %d / [0-1]draw the state of the neurons", drawneurons);
	display_message(message);
	}

	if (drawnstyle > 0 || verbose > 0)
	{
	sprintf(message,"drawnstyle %d / [0-1]draw neurons state by using bars or curves", drawnstyle);
	display_message(message);
	}

	sprintf(message,"dindividual %d / current individual of the population", dindividual);
	display_message(message);

	sprintf(message,"cindividual %d / current individual of the team", cindividual);
	display_message(message);

	sprintf(message,"drawneuronsdx %d / [1-1000]width of the area for displaying neurons state", drawneuronsdx);
	display_message(message);

	sprintf(message,"drawn_allt %d / [0-1]draw the state of the neurons of different individuals on the same graph", drawn_alltogether);
	display_message(message);

	if (timeadjust > 0 || verbose > 0)
	{
	sprintf(message,"pause %d / delay in ms for each cycle", timeadjust);
	display_message(message);
	}

	if (grid > 0 || verbose > 0)
	{
	sprintf(message,"grid %d / grid step affecting world and network editing", grid);
	display_message(message);
	}

	if (play_sound > 0 || verbose > 0)
	{
	sprintf(message,"play_sound %d / [0-1] play signals with sounds", play_sound);
	display_message(message);
	}

	if (verbose > 0 || verbose > 0)
	{
	sprintf(message,"verbose %d / [0-1-2] level of info and/or help", verbose);
	display_message(message);
	}

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}


}


/*
 * parse environmental parameters
 */
int
parse_env_parameter(char *st)

{

	int     narguments;
	int     nc;
	int     i;
	int     o;
	int     v;
	int     done;
	char    word[128];
	char    message[128];
	char    arg[10][128];
        struct  individual *pind;
	int     team;



	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');
	while(*st == ' ')
           st++;

	if (strcmp(word, "cworld") == 0)
	{
	   if (atoi(st) < nworlds && atoi(st) >= 0)
	     cworld = atoi(st);
	   done = 1;
	}

	if (strcmp(word, "dx") == 0)
	{
	   env[cworld].dx = atoi(st);
	   #ifdef EVOGRAPHICS
           set_scale();
	   #endif
	   done = 1;
	}

	if (strcmp(word, "dy") == 0)
	{
	   env[cworld].dy = atoi(st);
	   #ifdef EVOGRAPHICS
           set_scale();
	   #endif
	   done = 1;
	}

        // wall
	if (strcmp(word, "wall") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 7)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 5 || narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (narguments == 5)
	     {
	       if (i >= env[cworld].nobstacles)
	       {
                i = env[cworld].nobstacles;
	        env[cworld].nobstacles++;
	       }
               env[cworld].obstacles[i][0] = (float) atoi(arg[1]);
               env[cworld].obstacles[i][1] = (float) atoi(arg[2]);
               env[cworld].obstacles[i][2] = (float) atoi(arg[3]);
               env[cworld].obstacles[i][3] = (float) atoi(arg[4]);
	     }
	     if (narguments == 1 && i < env[cworld].nobstacles && i >= 0)
	       {
                for(o=i;o < (env[cworld].nobstacles - 1); o++)
		 for(v=0; v < 4; v++)
                  env[cworld].obstacles[o][v] = env[cworld].obstacles[o+1][v];
                env[cworld].nobstacles--;
	       }
	   }
	   else
	   {
	     sprintf(message, "wall objects require 5 arguments", word);
	     display_error(message);
	   }

	}


        // round
	if (strcmp(word, "round") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 5)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 3 || narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (narguments == 3)
	     {
	       if (i >= env[cworld].nroundobst)
	       {
                i = env[cworld].nroundobst;
	        env[cworld].nroundobst++;
	       }
               env[cworld].roundobst[i][0] = (float) atoi(arg[1]);
               env[cworld].roundobst[i][1] = (float) atoi(arg[2]);
	     }
	     if (narguments == 1 && i < env[cworld].nroundobst && i >= 0)
	       {
                for(o=i;o < (env[cworld].nroundobst - 1); o++)
		 for(v=0; v < 2; v++)
                  env[cworld].roundobst[o][v] = env[cworld].roundobst[o+1][v];
                env[cworld].nroundobst--;
	       }
	   }
	   else
	   {
	     sprintf(message, "round objects require 3 arguments", word);
	     display_error(message);
	   }

	}


        // sround
	if (strcmp(word, "sround") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 5)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 3 || narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (narguments == 3)
	     {
	       if (i >= env[cworld].nsroundobst)
	       {
                i = env[cworld].nsroundobst;
	        env[cworld].nsroundobst++;
	       }
               env[cworld].sroundobst[i][0] = (float) atoi(arg[1]);
               env[cworld].sroundobst[i][1] = (float) atoi(arg[2]);
	     }
	     if (narguments == 1 && i < env[cworld].nsroundobst && i >= 0)
	       {
                for(o=i;o < (env[cworld].nsroundobst - 1); o++)
		 for(v=0; v < 2; v++)
                  env[cworld].sroundobst[o][v] = env[cworld].sroundobst[o+1][v];
                env[cworld].nsroundobst--;
	       }
	   }
	   else
	   {
	     sprintf(message, "sround objects require 3 arguments", word);
	     display_error(message);
	   }

	}


        // light
	if (strcmp(word, "light") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 5)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 3 || narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (narguments == 3)
	     {
	       if (i >= env[cworld].nbulblights)
	       {
                i = env[cworld].nbulblights;
	        env[cworld].nbulblights++;
	       }
               env[cworld].bulblights[i][0] = (float) atoi(arg[1]);
               env[cworld].bulblights[i][1] = (float) atoi(arg[2]);
	     }
	     if (narguments == 1 && i < env[cworld].nbulblights && i >= 0)
	       {
                for(o=i;o < (env[cworld].nbulblights - 1); o++)
		 for(v=0; v < 2; v++)
                  env[cworld].bulblights[o][v] = env[cworld].bulblights[o+1][v];
                env[cworld].nbulblights--;
	       }
	   }
	   else
	   {
	     sprintf(message, "light objects require 3 arguments", word);
	     display_error(message);
	   }

	}


        // t_area
	if (strcmp(word, "t_area") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 6)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 4 || narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (narguments == 4)
	     {
	       if (i >= env[cworld].nfoodzones)
	       {
                i = env[cworld].nfoodzones;
	        env[cworld].nfoodzones++;
	       }
               env[cworld].foodzones[i][0] = (float) atoi(arg[1]);
               env[cworld].foodzones[i][1] = (float) atoi(arg[2]);
               env[cworld].foodzones[i][2] = (float) atoi(arg[3]);

	     }
	     if (narguments == 1 && i < env[cworld].nfoodzones && i >= 0)
	       {
                for(o=i;o < (env[cworld].nfoodzones - 1); o++)
		 for(v=0; v < 3; v++)
                  env[cworld].foodzones[o][v] = env[cworld].foodzones[o+1][v];
                env[cworld].nfoodzones--;
	       }
	   }
	   else
	   {
	     sprintf(message, "target_a objects require 4 arguments", word);
	     display_error(message);
	   }

	}

        // start
	// if robots have been moved we create or update a start position
	//  otherwise we remove a start position
	if (strcmp(word, "start") == 0)
	{
	   narguments = 0;
	   done = 1;
	   i = 0;
           while (getword(word,st,' ') > 0 && narguments < 2)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }
           if (narguments == 1)
	   {
	     i = atoi(arg[0]);
	     if (manual_start == 1)
	     {
	       if (i >= env[cworld].nstarts)
	       {
                i = env[cworld].nstarts;
	        env[cworld].nstarts++;
	       }
	       for (team=0,pind=ind; team < nteam; team++,pind++)
	       {
                env[cworld].starts[i][team][0] = pind->pos[0];
                env[cworld].starts[i][team][1] = pind->pos[1];
                env[cworld].starts[i][team][2] = (float) pind->direction;
	       }
	       manual_start = 0;
	     }
	     else
	     {
                for(o=i;o < (env[cworld].nstarts - 1); o++)
		 for(team=0; team < nteam; team++)
		  for(v=0; v < 3; v++)
                   env[cworld].starts[o][team][v] = env[cworld].starts[o+1][team][v];
                env[cworld].nstarts--;
	     }
	   }
	   else
	   {
	     sprintf(message, "start require 1 arguments");
	     display_error(message);
	   }

	}


	if (done == 0)
	{
	  sprintf(message, "unknown environmental parameter");
	  display_error(message);
	}

	return(done);

}



/*
 * display individual parameters
 */
void
display_env_parameters()

{

	char    message[256];
	int i;

	helptn = 1;

	display_message("environment:");

	sprintf(message,"cworld %d", cworld);
	display_message(message);

	sprintf(message,"dx %d dy %d",env[cworld].dx, env[cworld].dy);
	display_message(message);

        for (i=0; i < env[cworld].nobstacles; i++)
         {
          sprintf(message,"wall [%d] %.1f %.1f %.1f %.1f", i, env[cworld].obstacles[i][0],env[cworld].obstacles[i][1],env[cworld].obstacles[i][2],env[cworld].obstacles[i][3]);
	  display_message(message);
         }
        for (i=0; i < env[cworld].nroundobst; i++)
         {
          sprintf(message,"round [%d] %.1f %.1f", i, env[cworld].roundobst[i][0],env[cworld].roundobst[i][1]);
	  display_message(message);
         }
        for (i=0; i < env[cworld].nsroundobst; i++)
          {
           sprintf(message,"sround [%d] %.1f %.1f", i, env[cworld].sroundobst[i][0],env[cworld].sroundobst[i][1]);
	   display_message(message);
          }
	for (i=0; i < env[cworld].nfoodzones; i++)
          {
           sprintf(message,"t_area [%d] %.1f %.1f %.1f", i, env[cworld].foodzones[i][0],env[cworld].foodzones[i][1],env[cworld].foodzones[i][2]);
	   display_message(message);
          }
        for (i=0; i < env[cworld].nbulblights; i++)
          {
           sprintf(message,"light [%d] %.1f %.1f", i, env[cworld].bulblights[i][0],env[cworld].bulblights[i][1]);
	   display_message(message);
          }
        for (i=0; i < env[cworld].nstarts; i++)
         {
           sprintf(message,"start [%d] %.1f %.1f %.1f", i, env[cworld].starts[i][0][0],env[cworld].starts[i][0][1],env[cworld].starts[i][0][2]);
	   display_message(message);
         }
        for (i=0; i < env[cworld].nlandmarks; i++)
         {
	   sprintf(message,"landmark [%d] %.1f %.1f %.1f %.1f %.1f", i, env[cworld].landmark[i][0],env[cworld].landmark[i][1],env[cworld].landmark[i][2],env[cworld].landmark[i][3],env[cworld].landmark[i][4]);
	   display_message(message);
         }

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}


}



/*
 * parse network blocks parameters
 * the command to be issues is "set net [N]"
 * if a block of four neurons have been previously selected
 *  it replaces the block N with a block of connections (or create a new block)
 * if a block of two neurons have been previously selected
 *  it replaces the block N with a block of neurons to be updated (or create a new block)
 * if no neurons have been selected
 *  it delete block n
 * the command set net erase remove all connections, update blocks, biases, and deltas
 */
int
parse_net_parameter(char *st)

{

/*
     int     narguments;
     int     i;
     int     ii;
     int     done;
     char    word[128];
     char    message[128];
     char    arg[10][128];

     done = 0;
     while(*st == ' ')
               st++;

     if (strcmp(st, "erase") == 0)
     {
       net_nblocks = 0;
       for(i=0;i < ind->nneurons; i++)
	   {
             neuronbias[i] = 0;
             neurondelta[i] = 0;
	     neurondisplay[i] = 1;
	   }
     }
     else
     {
        narguments = 0;
        i = 0;
        while (getword(word,st,' ') > 0 && narguments < 2)
	   {
             sprintf(arg[i],"%s",word);
	     i++;
	     narguments++;
	   }


	// creates or modifies a block
        if (narguments == 1 && (cneuronn == 0 || cneuronn == 2 || cneuronn == 4))
	   {
	     i = atoi(arg[0]);
	     if (i >= net_nblocks)
		{
		  i = net_nblocks;
		  net_nblocks++;
		}
	     if (cneuronn == 0 && net_nblocks > 0)
		{
		  for(ii=i;ii < (net_nblocks - 1); ii++)
		  {
		   net_block[ii][0] = net_block[ii+1][0];
 		   net_block[ii][1] = net_block[ii+1][1];
		   net_block[ii][2] = net_block[ii+1][2];
		   net_block[ii][3] = net_block[ii+1][3];
		   net_block[ii][4] = net_block[ii+1][4];
		  }
                   net_nblocks--;
		}

	     if (cneuronn == 2)
		{
		  net_block[i][0] = 1;
		  net_block[i][1] = cneuron[0];
		  net_block[i][2] = cneuron[1] - cneuron[0] + 1;
		  net_block[i][3] = 0;
		  net_block[i][4] = 0;
		  cneuronn = 0;
		}
	     if (cneuronn == 4)
		{
		  net_block[i][0] = 0;
		  net_block[i][1] = cneuron[0];
		  net_block[i][2] = cneuron[1] - cneuron[0] + 1;
		  net_block[i][3] = cneuron[2];
		  net_block[i][4] = cneuron[3] - cneuron[2] + 1;
		  cneuronn = 0;
		}
	   }
	   else
	   {
	     if (narguments > 1)
	       sprintf(message, "this command require one parameter only");
              else
	       sprintf(message, "this command require to select 0, 2 or 4 neurons");
	     display_error(message);
	   }
	 }
*/
	return(1);

}


/*
 * display individual parameters
 */
void
display_net_parameters()

{
/*
	char    message[256];
	int b;

	helptn = 1;

	display_message("network architecture:");
	sprintf(message,"n. genes: %d", ind->numchars - 20); // we allocated 20 additional parameters
	display_message(message);
	sprintf(message,"total neurons: %d  sensors: %d  internal: %d  motor: %d",
	   ind->nneurons, ind->ninputs, ind->nneurons - (ind->ninputs + ind->noutputs), ind->noutputs);
	display_message(message);
	if (net_nblocks > 0)
	 for (b=0; b < net_nblocks; b++)
	 {
	   if (net_block[b][0] == 0)
             sprintf(message,"[%d]  %d %d %d %d / connections block",
	       b, net_block[b][1], net_block[b][2],net_block[b][3],net_block[b][4]);
           else
             sprintf(message,"[%d]  %d %d       / update block",
	       b, net_block[b][1], net_block[b][2]);
	   display_message(message);
	 }

	if (helptn > 0 && nodisplaydata == 0)
	{
	  //update_rendparameters();
	  helptn = 0;
	}

*/
}


/*
 * if f = 0 display fitness functions
 * if f > 0 update the ffitness_descp string
 */
void
display_fit_parameters(int f)

{

	char    message[256];

	helptn = 1;

	display_message("fitness functions:");

	sprintf(message,"Fitness 1: Ostacle Avoidance (move fast, straitght, and keep far from obstacles)");
	display_message(message);
	if (f == 1)
	  strcpy(ffitness_descp, message);

	sprintf(message,"Fitness 2: Discrim (1.0 when the robot is inside a target area)");
	display_message(message);
	if (f == 2)
	  strcpy(ffitness_descp, message);

	sprintf(message,"Fitness 3: Collective Navigation (1.0 for each robot inside targets, -1.0 for each extra robot)");
	display_message(message);
	if (f == 3)
	  strcpy(ffitness_descp, message);

	sprintf(message,"Fitness 4: Collective Navigation (1.0 for each robot inside targets, -2.0 for each extra robot)");
	display_message(message);
	if (f == 4)
	  strcpy(ffitness_descp, message);

	sprintf(message,"Fitness 16: Different Areas (1.0 if two robots are in two different areas, -1 if they are in the same)");
	display_message(message);
	if (f == 16)
	  strcpy(ffitness_descp, message);

	sprintf(message,"Fitness 18: Target Switch (1.0 every time the two robots are in two different area for the first time or after a switch)");
	display_message(message);
	if (f == 18)
	  strcpy(ffitness_descp, message);

	//if (helptn > 0)
	  //update_rendparameters();
	if (nodisplaydata == 0)
	 helptn = 0;

}

/*
 * load a parameter file
 * "evolution", "individual", "lifetime", "display" act as parameter folders
 */
void
load_new_parameters(char *filename)

{


     FILE	*fp;
     char       st[128];
     char       word[128];
     char       message[128];
     int n;
     int nc;
     int folder;
     int folder_command;

     folder = 0;
     if ((fp=fopen(filename, "r")) != NULL)
	{
	  n = 0;
	  while (fgets(st,128,fp) != NULL && n < 1000)
	  {
            folder_command = 0;
            nc = copyword(word,st,' ');
	    if (strcmp(word, "evolution") == 0)
	    {
	      folder = 0;
	      folder_command = 1;
	    }
	    if (strcmp(word, "individual") == 0)
	    {
	      folder = 1;
	      folder_command = 1;
	    }
	    if (strcmp(word, "lifetime") == 0)
	    {
	      folder = 2;
	      folder_command = 1;
	    }
	    if (strcmp(word, "display") == 0)
	    {
	      folder = 3;
	      folder_command = 1;
	    }
	    if (folder_command == 0 && nc > 0)
	     {
	      if (folder == 0)
		parse_evolution_parameter(st);
	      if (folder == 1)
		parse_ind_parameter(st);
	      if (folder == 2)
		parse_lifetime_parameter(st);
	      if (folder == 3)
		parse_display_parameter(st);
	     }
	    n++;
	  }
        fclose(fp);
        sprintf(message,"loaded parameter file: %s",filename);
        display_stat_message(message);
        }
       else
       {
         sprintf(message,"Error: the file %s could not be opended",filename);
	 display_error(message);
       }

}


/*
 * execute a command line parameters (i.e. parameters with the following sintax
 * set evolution ngenerations 500
 */
void
exec_commandline(char *command)

{


     char       word[128];
     int folder;
     int folder_command;

     folder = -1;
     folder_command = 0;
     getword(word,command,' ');
     if ((strcmp(word, "set") == 0) || (strcmp(word, "SET") == 0))
     {
       getword(word,command,' ');
            if ((strcmp(word, "evolution") == 0) || (strcmp(word, "evo") == 0))
	    {
	      folder = 0;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "individual") == 0) || (strcmp(word, "ind") == 0))
	    {
	      folder = 1;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "lifetime") == 0) || (strcmp(word, "lif") == 0))
	    {
	      folder = 2;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "display") == 0) || (strcmp(word, "dis") == 0))
	    {
	      folder = 3;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "environment") == 0) || (strcmp(word, "env") == 0))
	    {
	      folder = 4;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "network") == 0) || (strcmp(word, "net") == 0))
	    {
	      folder = 5;
	      folder_command = 1;
	    }
	    if ((strcmp(word, "fitness") == 0) || (strcmp(word, "fit") == 0))
	    {
	      folder = 6;
	      folder_command = 1;
	    }
		   if ((strcmp(word, "robot_connection") == 0) || (strcmp(word, "rcon") == 0))
	    {
	      folder = 7;
	      folder_command = 1;
	    }
	    if (folder >= 0)
	    {
	       switch(folder)
		  {
		   case 0:
	            if (strlen(command) > 0)
		      parse_evolution_parameter(command);
		    display_evolution_parameters();
		    break;
		   case 1:
	            if (strlen(command) > 0)
		      parse_ind_parameter(command);
		    display_ind_parameters(0);
		    break;
		   case 2:
	            if (strlen(command) > 0)
		      parse_lifetime_parameter(command);
		    display_lifetime_parameters();
		    break;
		   case 3:
	            if (strlen(command) > 0)
		      parse_display_parameter(command);
		    display_display_parameters();
		    break;
		   case 4:
	            if (strlen(command) > 0)
		      parse_env_parameter(command);
		    display_env_parameters();
		    break;
		   case 5:
	            if (strlen(command) > 0)
		      parse_net_parameter(command);
		    break;
		   case 6:
		    display_fit_parameters(0);
		    break;
		   case 7:
			parse_robot_connection(command);
			break;
		}
	    }
            else
	    {
              display_error("unknown folder name");
	    }
     }
     else
     {
       display_error("command syntax should be 'set <folder> <param> <value>'");
     }

}


/*
 * save parameters on a file by using the display functions
 */
void
save_parameters(char *filename)
{

    FILE *fp;
    int  h;

    nodisplaydata = 1;

    if ((fp=fopen(filename, "w!")) != NULL)
	{

	  display_evolution_parameters();
          for(h=0;h < (helptn - 1); h++)
	  {
            cat_comment_from_st(helpt[h]);
            fprintf(fp,"%s\n", helpt[h]);
	  }
          fprintf(fp,"\n");
	  helptn = 0;

	  display_ind_parameters(0);
          for(h=0;h < (helptn - 1); h++)
	  {
            cat_comment_from_st(helpt[h]);
            fprintf(fp,"%s\n", helpt[h]);
	  }
          fprintf(fp,"\n");
	  helptn = 0;

	  display_lifetime_parameters();
          for(h=0;h < (helptn - 1); h++)
	  {
            cat_comment_from_st(helpt[h]);
            fprintf(fp,"%s\n", helpt[h]);
	  }
          fprintf(fp,"\n");
	  helptn = 0;

	  display_display_parameters();
          for(h=0;h < (helptn - 1); h++)
	  {
            cat_comment_from_st(helpt[h]);
            fprintf(fp,"%s\n", helpt[h]);
	  }
          fprintf(fp,"\n");
	  helptn = 0;

	  sprintf(drawdata_st,"parameters data saved on file %s file",filename);
	  display_stat_message(drawdata_st);
	  fclose(fp);
	}
        else
	{
	  sprintf(drawdata_st,"unable to save %s file",filename);
	  display_warning(drawdata_st);
	}

    nodisplaydata = 0;

}

/*
 * parse a string by setting the corresponding parameter
 * es. "ngenerations 100" "wall 0 12 15 18"
 */
int
parse_parameter(char *st)

{

	int     narguments;
	int     nc;
	int     done;
	char    word[128];
	int     i;

	int     ngenerations=0;
	int     wall[4];
	char    buffer[128];
	char    arg[10][128];


	done = 0;
	while(*st == ' ')
               st++;

        nc = getword(word,st,' ');

	if (strcmp(word, "ngenerations") == 0)
	{
	   ngenerations = atoi(st);
	   done = 1;
	}
	if (strcmp(word, "wall") == 0)
	{
	   //st2 = st;
	   narguments = 0;
           //while (getword(word,st2,' ') > 0)
	   //  narguments++;

	   i = 0;
           while (getword(word,st,' ') > 0)
	   {
             sprintf(arg[i],"%s",word);
	     wall[i] = atoi(word);
	     i++;
	     narguments++;
	   }

	}

	sprintf(buffer, "narguments %d, %s %s  - %d %d %d %d", narguments, arg[0], arg[1], wall[0], wall[1], wall[2], wall[3]);
	display_stat_message(buffer);

	return(1);

}


/*
 * extract a word from a string (the original string is shortened)
 */
int
getword(char *word, char *line, char stop)
{
    int x;
    int y;
    int l;

    x=0;
    for(x=0,l=0;((line[x]) && (line[x] != stop && line[x] != '\n'));x++,l++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
    y=0;

    while(line[y++] = line[x++]);

    return(l);
}

/*
 * extract a word from a string (the original string is left intact)
 */
int
copyword(char *word, char *line, char stop)
{
    int x;
    int l;

    x=0;
    for(x=0,l=0;((line[x]) && (line[x] != stop && line[x] != '\n'));x++,l++)
        word[x] = line[x];

    word[x] = '\0';

    return(l);
}

/*
 * remove the first space characters from a string
 */
void
clear_from_space(char *stringa)
{

    while(*stringa == ' ')
       stringa++;

}

/*
 * cut a comment ("../...") from a st
 */
void
cat_comment_from_st(char *st)
{

    while(*st != '/' && *st != '\0')
       st++;
    *st = '\0';

}



/*
 * Initialize population parameters
 */
void
init_phenotype()
{
    struct  individual *pind;
    int     team;

    for (team=0,pind=ind; team < nteam; team++, pind++)
	 {

		pind->population = team;
		pind->n_team = team;
		pind->dead = 0;
	}

}


/*
 * Initialize population parameters (we always initialize 2 populations)
 */
void
init_parameters()
{
    struct  iparameters *pipar;

    pipar = ipar;

	pipar->ffitness       = 0;
	pipar->wheelmotors    = 0;
	pipar->nifsensors     = 0;
	pipar->lightsensors   = 0;
	pipar->cameraunits    = 0;
	pipar->camerarange    = 0;
	pipar->motmemory      = 0;
	pipar->oppsensor      = 0;
	pipar->groundsensor   = 0;
	pipar->a_groundsensor = 0;
	pipar->a_groundsensor2= 0;
	pipar->elman_net      = 0;
	pipar->input_outputs  = 0;
	pipar->signal_sensors = 0;
	pipar->signalss       = 0;
	pipar->signalso       = 0;
	pipar->signalssf      = 0;
        pipar->signalssb      = 0;
	pipar->signaltype     = 0;
        ninput_other          = 0;
        pipar->nhiddens       = 0;
        pipar->dynrechid      = 0;
        pipar->dynrecout      = 0;
        pipar->dynioconn      = 0;
        pipar->dyndeltai      = 0;
        pipar->dyndeltah      = 0;
	pipar->dyndeltao      = 0;
	pipar->arbitration    = 0;
	pipar->simplevision   = 0;
	pipar->nutrients      = 0;
	pipar->compass        = 0;
	pipar->energy         = 0;


}


void
compute_parameters()

{

   struct  iparameters *pipar;
   struct  individual *pind;
   int     team;

   pipar = ipar;
   nindividuals = nkids * bestfathers;
   for (team=0,pind=ind; team<nteam; team++, pind++)
   {
		   /* ninputs */
		   pind->ninputs  = 0;
		   pind->noutputs = 0;
		   if (pipar->groundsensor   > 0)
			   pind->ninputs += pipar->groundsensor;
		   if (pipar->a_groundsensor   > 0)
			   pind->ninputs += pipar->a_groundsensor;
		   if (pipar->a_groundsensor2   > 0)
			   pind->ninputs += pipar->a_groundsensor2;
		   if (pipar->elman_net     == 1)
			   pind->ninputs += pipar->nhiddens;
		   if (pipar->lightsensors  > 0)
			   pind->ninputs += pipar->lightsensors;
		   if (pipar->motmemory     == 1)
			   pind->ninputs += 2;
		   if (ninput_other > 0)
			   pind->ninputs += (4 * ninput_other);
		   if (pipar->signalss > 0)
			   pind->ninputs += pipar->signalss * pipar->signalso;
		   if (pipar->signalso > 0)
			   pind->ninputs += pipar->signalso;
		   if (pipar->signalssf > 0)
			   pind->ninputs += pipar->signalso * pipar->signalssf;
		   if (pipar->nifsensors    > 0)
			   pind->ninputs += pipar->nifsensors;
		   if (pipar->cameraunits    > 0)
			   pind->ninputs += pipar->cameraunits;
		   if (pipar->simplevision   == 1)
			   pind->ninputs += 5;
		   if (pipar->simplevision   == 2)
			   pind->ninputs += 3;
		   if (pipar->nutrients > 0)
			   pind->ninputs += 2;
		   if (pipar->compass    > 0)
			   pind->ninputs += 2;
		   if (pipar->energy == 1)
			   pind->ninputs += 1;
		   if (pipar->arbitration    > 0)
			   pind->ninputs += pipar->arbitration;
		   /* noutputs */
		   if (pipar->wheelmotors > 0)
			   pind->noutputs += pipar->wheelmotors;
		   if (pipar->signalso > 0)
			   pind->noutputs += pipar->signalso;
		   if (pipar->arbitration > 0)
			   pind->noutputs += pipar->arbitration;
		   /* special cases */
		   if (pipar->input_outputs > 0)
		   {
			   pind->noutputs += pipar->input_outputs;
			   pind->ninputs  += pipar->input_outputs;
		   }

		   pind->nhiddens = pipar->nhiddens;
		   pind->nneurons = pind->ninputs + pind->noutputs + pipar->nhiddens;


  }
}



