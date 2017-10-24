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

#include "defs.h"
#include "public.h"

#ifdef EVOREALWIN
#include "time.h"
clock_t sTime;
clock_t eTime;
#endif

char dbgString[30];

int prova1 = 1;
int prova2 = 2;
int prova3 = 3;
int prova4 = 4;
int buf1 = 11;
int buf2 = 12;
int buf3 = 13;
int buf4 = 14;


/*
 * try to load parameters from the configuration file (default robot.cf)
 * if the file does not exists the user should set the parameters by hand
 */
void
init_khepsim()
{

	int g;
	int gg;
	int p;
	int ng;
	int team;
	struct  individual *pind;

	// allocate space for only one individual's parameter
        ipar = (struct iparameters *) malloc(1 * sizeof(struct iparameters));

	init_parameters();
	load_new_parameters("evorobot.cf");

	// initialize world variables
	init_world();

	// create individuals
	create_poppointer();

	// initiliaze populations parameters
	init_phenotype();

	// also call the functions that allocate space
	compute_parameters();

        create_neurons_labels();
	if (load_net_blocks("evorobot.net") == 0)
           create_net_blocks(ind->ninputs, ind->noutputs, ind->nneurons,ipar->dyndeltai, ipar->dyndeltah, ipar->dyndeltao, ipar->dynioconn, ipar->dynrechid, ipar->dynrecout);
	// compute the number of free parameters
        ng = compute_nfreep(ind->nneurons);
        if (homogeneous == 0)
          ng = ng * nteam;
        ng += 20; //we add 20 additional to be sure

	for (p=0, pind=ind; p < nteam; p++, pind++)
	  pind->numchars = ng;

	create_genome();         // allocate space for the genome
        create_ind_phenotype();  // allocate space for the phenotype free parameters

create_parameter("prova1", "network",&prova1, 0, 0, 100, "bla bla");
create_parameter("prova2", "network",&prova2, 0, 0, 100, "bla bla bla");
create_parameter("prova3", "network",&prova3, 0, 0, 100, "");
create_parameter("prova4", "network",&prova4, 0, 0, 100, "");
create_parameter("buf1", "evolution",&buf1, 0, 0, 100, "");
create_parameter("buf2", "evolution",&buf2, 0, 0, 100, "");
create_parameter("bud3", "evolution",&buf3, 0, 0, 100, "");
create_parameter("buf4", "evolution",&buf4, 0, 0, 100, "");

set_parameter("prova3", "network",66);



	load_world();
	create_world_space();
	load_motor();

#ifdef EVOWINDOWS
	load_obstacle("../bin/sample_files/wall.sam",   wall,     wallcf);
	load_obstacle("../bin/sample_files/round.sam",  obst,     obstcf);
	load_obstacle("../bin/sample_files/small.sam",  sobst,    sobstcf);
	load_obstacle("../bin/sample_files/light.sam",  light,    lightcf);
#else	
        load_obstacle("..\bin\sample_files\wall.sam",   wall,     wallcf);
	load_obstacle("..\bin\sample_files\round.sam",  obst,     obstcf);
	load_obstacle("..\bin\sample_files\small.sam",  sobst,    sobstcf);
	load_obstacle("..\bin\sample_files\light.sam",  light,    lightcf);
#endif

	set_initial_position();

        for(team=0,pind=ind;team<nteam;team++, pind++)
	  getgenome(pind,0,team);

	sprintf(ffitness_descp,"fitness n. %d", ipar->ffitness);
	display_fit_parameters(ipar->ffitness);

	for (g=0; g < MAXGENERATIONS; g++)
	   for (gg=0; gg < 4; gg++)
	      astatfit[g][gg] = 0.0;

}

/*
 * allocate space for population pointers
 */
void
create_poppointer()
{

    ind = (struct individual *) malloc((nteam) * sizeof(struct individual));
    if (ind == NULL)
	{
		display_error("individual malloc error");
	}

}

/*
 * the regular loop
 */
void
loop(int gen,int pop, int nind,int epoch)


{

	struct  individual *pind;
	struct  individual *find;
        struct  iparameters *pipar;
	int     i;
	float   vel1,vel2;
	int     s;
	int     stop;
	int     crash_occurred;
	int     team;
	int     n_ind_on_food_1;
	int     n_ind_on_food_2;
	int     oldzone, oldzoneb;
	int     prev_zone, prev_zoneb;
	int     count;
	extern  float   sound;

	float   var_mot, ave_v1, ave_v2;
	double  ang, relang, dist;
	double  totmot;
	int     turn1, turn2, nturn1, nturn2;
	float   gpos[2], gposold[2];
	double  robots_dist;

	// INITIALIZATION OF THE TRIAL
	run = 0;
	stop = 0;

	individual_crash = 0;
	nturn1 = 0;
	nturn2 = 0;
	totmot = 0.0;

	oldzone = 0;
	oldzoneb = 0;
	prev_zone = 0;
	prev_zoneb = 0;
	count = 0;

	if (manual_start == 0)
	 {
	   set_world_parameters(gen,pop,nind,epoch);
	   set_initial_position();
	}
	else
	 {
	   manual_start = 0;
	}

	find=ind;
	for(team=0,pind=ind;team<nteam;team++,pind++)
	{
	 reset_net(pind->nneurons, pind->activation, pind->ninputs, pind->input);
	 pind->lifecycle     = 0;
	 pind->eatenfood[0] = 0.0;
	 pind->eatenfood[1] = 0.0;
	 for (i=0;i < ipar->a_groundsensor; i++)
           pind->ground[i] = 0.0;
	 for (i=0;i < ipar->a_groundsensor2; i++)
           pind->ground[i] = 0.0;
	 if (epoch == 0)
	 {
	   pind->oldcfitness = 0.0;
	   pind->oldfitness = 0.0;
	 }
         else
	   pind->oldfitness = pind->fitness;
	}

        // BEGIN OF THE TRIAL
	while ((run < sweeps) && stop == 0)
	{
#ifdef EVOREALWIN
	  sTime=clock();
#endif
	  n_ind_on_food_1 = 0;
	  n_ind_on_food_2 = 0;

	  if (real==1)
			{
            if (ipar->simplevision > 0)
            queryRobotsSensorsV(); //reads the sensors value of all robots
			else
			queryRobotsSensors(); 
			}

	  for(team=0,pind=ind, pipar = ipar;team<nteam;team++, pind++)
	  {
	    pind->oldcfitness = pind->fitness;
	    if (real==0)
		{
		  set_input(pind, pipar);
		}
	         else
		{
		  set_input_real(pind,pipar);
		}
	  }

	  for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
	   if (pind->dead == 0)
	   {

		activate_net(pind->freep, pind->nneurons, pind->activation, pind->ninputs, pind->input);

		// move the robot
		for(i=0; i < timelaps; i++)
		{
			if(real == 0)
			{
			    if (ipar->wheelmotors == 2)
				  setpos(pind,oselect(pind, 0),oselect(pind, 1),1.0);
				else
				  setpos(pind,oselect(pind, 0),oselect(pind, 1),oselect(pind, 2));

			}else
			{
				setpos_real(pind,oselect(pind, 0),oselect(pind, 1));

			}
		}


                // update the landmarks of the robots
                if (pipar->cameraunits > 0 && ipar->signaltype == 1 && ipar->signalso > 0)
		 {
	          int id;
                  if (env[cworld].nlandmarks < nteam)
                    env[cworld].nlandmarks = nteam;
		  id = (env[cworld].nlandmarks - nteam) + team;
	          env[cworld].landmark[id][0] = pind->pos[0];
	          env[cworld].landmark[id][1] = pind->pos[1];
	          env[cworld].landmark[id][2] = ROBOTRADIUS;
	          env[cworld].landmark[id][3] = 0.0;
	          env[cworld].landmark[id][4] = oselect(pind,ipar->wheelmotors);
		}

		// check for collision
		crash_occurred = 0;
		if (check_crash(pind->pos[0],pind->pos[1]))
		{
		  crash_occurred = 1;
		  if (stop_when_crash == 1)
		    stop = 1;
		   else
		    reset_pos_from_crash(pind);
		}
		// compute fitness 1 (after each lifecycle after each individual move)
		// fitness is update only when collision do not occur
		if (crash_occurred == 0)
		{

		  switch(pipar->ffitness)
		  {
		    // 1-collective obstacle avoidance
		    //   robots should move forward
		    case 1:
		      fitmode = 1;
		      vel1 = pind->speed[0];
		      vel2 = pind->speed[1];
		      //ave_mot = (float) fabs((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
		      ave_mot = (float) ((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
                      dif_m   = (float) sqrt(fabs( ((vel1 + (float) 10.0) / (float) 20.0) - ((vel2 + (float) 10.0) / (float) 20.0) ) );
		      for(s=0, maxs = (float) 0.0; s<8; s++)
                        if (pind->cifsensors[s] > maxs)
		          maxs = pind->cifsensors[s];
		      ind->fitness += ave_mot * (1.0 - dif_m) * (1.0 - maxs);
		      break;
		      // 2-stay on target
		      case 2:
			if (read_ground(pind) > 0)
			{
			   ind->fitness += 1.0f;
			}
			fitmode = 2;
			break;
		    // 3-food areas
		    case 3:
		      if (read_ground(pind) == 1)
			{
			   n_ind_on_food_1 += 1;
			   ind->fitness += 0.25f;
			   if (n_ind_on_food_1 > 2)
			     ind->fitness -= 1.0f;
			}
		      if (read_ground(pind) == 2)
			{
			   n_ind_on_food_2 += 1;
			   ind->fitness += 0.25f;
			   if (n_ind_on_food_2 > 2)
			     ind->fitness -= 1.0f;
			}
		      fitmode = 2;
		      break;
		      // 4-food areas, double punishment
		      case 4:
			if (read_ground(pind) == 1)
			{
			   n_ind_on_food_1 += 1;
			   ind->fitness += 0.25f;
			   if (n_ind_on_food_1 > 2)
			     ind->fitness -= 2.0f;
			}
			if (read_ground(pind) == 2)
			{
			   n_ind_on_food_2 += 1;
			   ind->fitness += 0.25f;
			   if (n_ind_on_food_2 > 2)
			     ind->fitness -= 2.0f;
			}
			fitmode = 1;
			break;
		      // 5 explore and categorize
		      // for the first 2 epochs the robots get a fraction of point for visiting each area up to 15 points
		      // for succeding epochs robots are rewarded for their categorization ability
		      case 5:
			if (epoch < 2)
			{
                         i = read_ground(pind);
			 if (i > 0 && env[cworld].foodzones[i-1][3] == 0.0)
			 {
			   oldzone++;
			   env[cworld].foodzones[i-1][3] = 1.0f;
			   ind->fitness += 0.5 / (float) env[cworld].nfoodzones;
			 }
			 if (oldzone == env[cworld].nfoodzones)
			 {
			   ind->fitness += 1.0;
			   oldzone = 0;
                           for (i=0; i < env[cworld].nfoodzones; i++)
                             env[cworld].foodzones[i][3] = 0.0f;
			 }
			 if (ind->fitness > 15.0)
			   ind->fitness = 15.0;
			}
			else
			{
			if (ind->fitness >= 15.0)
			 if ((epoch % 2) == 0)
			 {
                              ind->fitness += (1.0f - oselect(pind,2));
			 }
			  else
			 {
                              ind->fitness += oselect(pind,2);
			 }
			}
			fitmode = 3;
			break;
		  }
		}
	  } // end team-for
	  // compute fitness 2 (after each lifecycle after all individuals move)
	  // fitness is update only when collision do not occur
	  if (crash_occurred == 0)
		{
		  switch(pipar->ffitness)
		  {
		    // 11 - syncronized movements
		    case 11:
		      fitmode = 1;
		      ave_mot = 0.0;
		      var_mot = 0.0;
		      ave_v1 = 0.0;
		      ave_v2 = 0.0;
		      for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
		      {
		        vel1 = pind->speed[0];
		        vel2 = pind->speed[1];
                        ave_v1 += (vel1 / (float) 10.0);
                        ave_v2 += (vel2 / (float) 10.0);
		        ave_mot += (float) ((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
		      }
		      ave_mot = ave_mot / (float) nteam;
		      ave_v1 = ave_v1 / (float) nteam;
		      ave_v2 = ave_v2 / (float) nteam;
		      for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
		      {
		        vel1 = pind->speed[0];
		        vel2 = pind->speed[1];
		        var_mot += (float) fabs(ave_v1 - (vel1 / (float) 10.0));
		        var_mot += (float) fabs(ave_v2 - (vel2 / (float) 10.0));
		      }
		      if (var_mot < 3.0)
			dif_m = 3.0f - var_mot;
		       else
                        dif_m = 0.0f;
		      ind->fitness += ave_mot * dif_m;
		      break;
		    // 12 - turn-taking (team must be formed by 2 individuals)
		    //      fitness is maximized by straight motion and alternation of turns
		    //      distance between the two robots should be < 250
		    case 12:
		      fitmode = 3;
		      turn1  = 0;
		      turn2  = 0;
		      ang = 0.0;
		      if (nteam == 2)
		      {
			pind = (ind + 1);
		        dist = mdist(ind->pos[0],ind->pos[1], pind->pos[0], pind->pos[1]);
			if (dist < 250.0)
			{
		         ang = mang((int) ind->pos[0],(int) ind->pos[1], (int) pind->pos[0],(int) pind->pos[1], dist);
			 relang  = mangrel(ang,ind->direction);
			 if (relang > 90 && relang < 270)
			   turn1 = 1;
		         ang = mang((int) pind->pos[0],(int) pind->pos[1], (int) ind->pos[0],(int) ind->pos[1], dist);
			 relang  = mangrel(ang,pind->direction);
			 if (relang > 90 && relang < 270)
			  turn2 = 1;
			 if (turn1 == 1 && turn2 == 0)
			 {
			  nturn1++;
			 }
			 if (turn1 == 0 && turn2 == 1)
			 {
			  nturn2++;
			 }
                         totmot += ((ind->speed[0] / (float) 10.0) + (ind->speed[1] / (float) 10.0)) / (float) 2.0;
                         totmot += ((pind->speed[0] / (float) 10.0) + (pind->speed[1] / (float) 10.0)) / (float) 2.0;
			}
			if (run == (sweeps - 1) && nturn1 > 0 && nturn2 > 0)
			{

                          if (nturn2 > nturn1)
			    ind->fitness += ((float) nturn1 / (float) nturn2) * fabs(totmot);
			   else
			    ind->fitness += ((float) nturn2 / (float) nturn1) * fabs(totmot);
			}
		      }
		      break;
		    // 13 - explore by staying together
		    case 13:
		      fitmode = 3;
		      if ((run % 10) == 0)
		      {
                      pind = (ind + 1);
		      gpos[0] = (ind->pos[0] + pind->pos[0]) / 2.0f;
		      gpos[1] = (ind->pos[1] + pind->pos[1]) / 2.0f;
		      robots_dist = mdist((float)ind->pos[0],(float)ind->pos[1],(float) pind->pos[0],(float)pind->pos[1]);
		      if (run > 0 && robots_dist < 100.0)
		       ind->fitness += mdist(gpos[0],gpos[1],gposold[0],gposold[1]);
		      //if (run > 250 && robots_dist > 120.0)
		      // stop = 1;
		      gposold[0] = gpos[0];
		      gposold[1] = gpos[1];
		      }
                    break;
		    // 14-food areas, 0.1 for staying, 0.5 for staying together
		    case 14:
		      if (nteam == 2)
		       {
		        for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
			 if (read_ground(pind) > 0)
			   ind->fitness += 0.25f;
		        pind = (ind + 1);
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && (read_ground(ind) == read_ground(pind)))
                          ind->fitness += 0.5f;
		       }
		      fitmode = 2;
		      break;
		    // 15- fitness is gathered by combining two type of food
		    // even and odd area contains food A and B
		    // up to 100.0 units can be collected
		    // each time step 0.25 units are lost
		    // fitness is obtained by extracting energy from As and Bs
		    case 15:
		      //if (nteam == 2)
		      // {
		        for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
			 {
			  // ingest nutrients
			  if (read_ground(pind) > 0)
			   if ((read_ground(pind) % 2) == 0)
			   {
			    if (pind->eatenfood[0] < 100)
			       pind->eatenfood[0] += 1;
			   }
			   else
			   {
			    if (pind->eatenfood[1] < 100)
			       pind->eatenfood[1] += 1;
			   }
			  // gather fitness
			  if (pind->eatenfood[0] > 0 && pind->eatenfood[1] > 0)
			   if (pind->eatenfood[0] < pind->eatenfood[1])
			   {
			    pind->fitness += pind->eatenfood[0];
			    pind->eatenfood[1] -= pind->eatenfood[0];
			    pind->eatenfood[0] = 0.0;
			   }
			   else
			   {
			    pind->fitness += pind->eatenfood[1];
			    pind->eatenfood[0] -= pind->eatenfood[1];
			    pind->eatenfood[1] = 0.0;
			   }
			  // undigested nutrients degenerate
			  if (pind->eatenfood[0] > 0.25)
                            pind->eatenfood[0] -= 0.25;
			  if (pind->eatenfood[1] > 0.25)
                            pind->eatenfood[1] -= 0.25;
			}
		       //}
		      fitmode = 3;
		      break;
		    // 16-food areas, 1.0 for staying in different areas,
		    //                -1.0 for staying in the same area
		    case 16:
		      if (nteam == 2)
		       {
		        for(team=0,pind=ind, pipar=ipar;team<nteam;team++, pind++)
			 if (read_ground(pind) > 0)
			   ind->fitness += 0.00f;
		        pind = (ind + 1);
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && ((read_ground(ind) % 2) != (read_ground(pind) % 2)))
                          ind->fitness += 1.0f;
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && ((read_ground(ind) % 2) == (read_ground(pind) % 2)))
                          ind->fitness -= 1.0f;
		       }
		      fitmode = 1;
		      break;
		    // 17-shifting areas, 1.0 every time they are in different areas
		    //                    for the first time or after a shift
		    //    one target area is set in the upper-left corner
		    case 17:
		      if (nteam == 2)
		       {
		        pind = (ind + 1);
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && ((read_ground(ind) % 2) != (read_ground(pind) % 2))
			   && (oldzone == 0 || oldzone != read_ground(ind)))
				{
                                  ind->fitness += 1.0f;
				  oldzone = read_ground(ind);
				}
		       }
		      fitmode = 2;
		      break;
		    // 18-shifting areas, 1.0 every time they are in different areas
		    //                    for the first time or after a shift
		    case 18:
		      if (nteam == 2)
		       {
		        pind = (ind + 1);
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && ((read_ground(ind) % 2) != (read_ground(pind) % 2)) && (oldzone == 0 || oldzone != read_ground(ind)))
			 {
                          ind->fitness += 1.0f;
                          if (read_ground(ind) > 0)
                            oldzone = read_ground(ind);
                          if (read_ground(pind) > 0)
                            oldzoneb = read_ground(pind);
			 }
		       }
		      fitmode = 2;
		      break;
		    // 28-shifting areas, 1.0 every time a robot enter in a new area
		    //                    for the first time or after a shift
		    case 28:
		      if (nteam == 2)
		       {
		        pind = (ind + 1);
                        if (read_ground(ind) > 0 && read_ground(ind) != oldzone)
		           {
                            ind->fitness += 1.0f;
                            oldzone = read_ground(ind);
			   }
                        if (read_ground(pind) > 0 && read_ground(pind) != oldzoneb)
		           {
                            ind->fitness += 1.0f;
                            oldzoneb = read_ground(pind);
			   }
		       }
		      fitmode = 2;
		      break;
		    // 19 - aggregate (only two robots)
		    case 19:
		      fitmode = 3;
                      pind = (ind + 1);
		      robots_dist = mdist((float)ind->pos[0],(float)ind->pos[1],(float) pind->pos[0],(float)pind->pos[1]);
		      if (run > 0 && robots_dist < 1000.0)
		       ind->fitness += (1000.0 - robots_dist) / 1000.0;
		    // 20-shifting areas, 1.0 every time they are in different areas
		    //                    for the first time or after a shift
		    case 20:
		      pind = (ind + 1);
		      if ((read_ground(ind) > 0) && (read_ground(pind) > 0) &&
			 ((read_ground(ind) != oldzone && read_ground(pind) != oldzoneb) ))
			{
			  count++;
			  if ((read_ground(ind) % 2) != (read_ground(pind) % 2))
				 ind->fitness += 1.0f;
			      else
				 ind->fitness -= 1.0f;
			  if (count >= 100)
				{
				  oldzone = read_ground(ind);
				  oldzoneb = read_ground(pind);
				  count = 0;
				}
			}
			else
			count = 0;

			if (prev_zone > 0 && read_ground(ind) == 0)
				{
				oldzone = prev_zone;
				if (read_ground(pind) > 0)
				  oldzoneb = read_ground(pind);
				}
			if (prev_zoneb > 0 && read_ground(pind) == 0)
				{
				oldzone = prev_zone;
				if (read_ground(ind) > 0)
				  oldzone = read_ground(ind);
			}
			prev_zone = read_ground(ind);
			prev_zoneb = read_ground(pind);

			fitmode = 2;
			break;
		    // 21-shifting areas, 1.0 every time they are in different areas
		    //                    for the first time or after a shift
		    case 21:
		      pind = (ind + 1);
		      if ((read_ground(ind) > 0) && (read_ground(pind) > 0) &&
			 ((read_ground(ind) != oldzone && read_ground(pind) != oldzoneb) ))
			{
			  count++;
			  if ((read_ground(ind) % 2) != (read_ground(pind) % 2))
				{
				 if (count < 100)
				   ind->fitness += 1.0f;
				  else
				   ind->fitness -= 1.0f;
				}
			      else
				{
				 ind->fitness -= 1.0f;
				}
			  if (count >= 200)
				{
				  oldzone = read_ground(ind);
				  oldzoneb = read_ground(pind);
				  count = 0;
				}
			}
			else
			count = 0;

			if (prev_zone > 0 && read_ground(ind) == 0)
				{
				oldzone = prev_zone;
				if (read_ground(pind) > 0)
				  oldzoneb = read_ground(pind);
				}
			if (prev_zoneb > 0 && read_ground(pind) == 0)
				{
				oldzone = prev_zone;
				if (read_ground(ind) > 0)
				  oldzone = read_ground(ind);
			}
			prev_zone = read_ground(ind);
			prev_zoneb = read_ground(pind);

			fitmode = 2;
			break;
		    // 22-shifting areas, 1.0 every time they are in different areas after a shift
		    //                    the position of the two areas is randomize over the Y axis
		    case 22:

		      if (env[cworld].nfoodzones == 0)
		      {
		        pind = ind;
		        vel1 = pind->speed[0];
		        vel2 = pind->speed[1];
		        ave_mot = (float) ((vel1 / (float) 10.0) + (vel2 / (float) 10.0)) / (float) 2.0;
                        dif_m   = (float) sqrt(fabs( ((vel1 + (float) 10.0) / (float) 20.0) - ((vel2 + (float) 10.0) / (float) 20.0) ) );
		        for(s=0, maxs = (float) 0.0; s<8; s++)
                          if (pind->cifsensors[s] > maxs)
		            maxs = pind->cifsensors[s];
			maxs = maxs + 0.5;
		        if (maxs > 1.0)
			  maxs = 1.0;
		        ind->fitness += ave_mot * (1.0 - dif_m) * maxs * 0.001;
		      }
		      else
		      {
		      if (nteam == 2)
		       {
		        pind = (ind + 1);
		        if ((read_ground(ind) > 0) && (read_ground(pind) > 0) && ((read_ground(ind) % 2) != (read_ground(pind) % 2))
			   && (oldzone == 0 || (oldzone % 2) != (read_ground(ind) % 2)))
				{
				  if (oldzone > 0)
                                    ind->fitness += 1.0f;
				  oldzone = read_ground(ind);
				}
		       }
		      }
		  }
		}
	  // update the rendering of the robot/environment and network
          if (testindividual_is_running == 1 || drawindividual == 1)
	    {
	     update_rendnetwork();
	     update_rendrobot();
	    }
	  // update world and neurons graphics
          if (testindividual_is_running == 1 && console == 0)
	  {
	     sprintf(drawdata_st,"epoch %d cycle %d fitness %.3f tfitness %.3f ", cepoch,ind->lifecycle, ind->fitness - ind->oldcfitness, ind->fitness);
	     display_stat_message(drawdata_st);
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

		//sprintf(dbgString,"Time: %d mms\r\n",(int)eTime-(int)sTime);
		//display_warning(dbgString);
#ifdef EVOREALWIN
	eTime=clock();
	dtime=(int)eTime-(int)sTime;
	if(real==1)
	while( ((int)clock()-(int)sTime)<100) {}; //waits until 100mms

#endif

	if (userbreak == 1)
	{
	  run = sweeps;
	  playsound(ind,0);
	   //stopping real robots iif connected
	      if ( real==1)
			{
		for(team=0,pind=ind;team<nteam;team++,pind++) setpos_real(pind,0.5f, 0.5f);
			}
	}
   }   // end for-sweeps
   // compute fitness 3 (at the end of the trial  after all individuals moved)
   switch(pipar->ffitness)
   {
   }
}




