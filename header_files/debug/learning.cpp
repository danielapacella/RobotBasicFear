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
 * individual_learning.cpp
 */

#include "defs.h"
#include "public.h"

/*
 * save the best individuals
 */
void
savebestg(int g)
{

  char sbuffer[128];
  char sroot[128];

  FILE *fp;

  sprintf(sbuffer,"B%dP%dS%d.gen",1,0,seed);
  if (g == 0)
  {
     if ((fp=fopen(sbuffer, "w")) == NULL)
	{
	  sprintf(sroot,"I cannot open file B%dP%dS%d.gen",1,0,seed);
          display_error(sroot);
	}
  }
  else
  {
     if ((fp=fopen(sbuffer, "a")) == NULL)
	{
	   sprintf(sroot,"I cannot open file B%dP%dS%d.gen",1,0,seed);
	   display_error(sroot);
	}
  }

  fprintf(fp,"**NET : s%d_%d.wts\n",g,0);
  saveagenotype(fp,0,0);
  fflush(fp);
  fclose(fp);
}


/*
 * individual learning through simulated annealing
 * (we assume that the number of allocated genome and bestgenome
 *  is at least equal to the number of learning individuals)
 */ 
void
individual_learning()
{
         
    int t,i,ii,g,c;
    int *cgeno;
    int *cbestgeno;
    int nlindividuals;
    int adaptivev;
    int evaluatedn;
    int nrejected;
    struct individual *pind;
    double newperformance, oldperformance;
    double perf, totperformance;
    char sbuffer[128];

    // number of individuals to be trained
    if (homogeneous == 1)
      nlindividuals = 1;
     else
      nlindividuals = nteam;

    // before starting the training process:
    //   we copy the genotypes into bestgenomes to store a copy before variations
    //   we evaluate initial performance

    cgeno = *genome;
    cbestgeno = *bestgenome;
    for(ii=0; ii < ind->numchars; ii++, cgeno++, cbestgeno++)
        *cbestgeno = *cgeno;	 
    oldperformance = eval_team(0, 0, 0);
    totperformance = oldperformance;
    evaluatedn = 1;
    nrejected = 0;
    savebestg(0);

    // we train individuals
    for(c=0,g=0; c < learningc; c++) 
	{

	  // we perturbe parameters
	  for (i=0, cgeno = *genome; i < ind->numchars; i++, cgeno++)
		*cgeno = mutate(*cgeno);
		
	  // evaluate the individual
	  newperformance = eval_team(0, 0, 0);

          if (console == 0 || verbose > 1) 
	  {
	    sprintf(sbuffer, "Seed %d cycle %d nrejected %d Perform.%.3f -> %.3f", seed, c, nrejected, oldperformance, newperformance);
			display_stat_message(sbuffer);
	  }

          // if performance did not increase we reject variations
	  if (oldperformance > newperformance)
	  {
            cgeno = *genome;
            cbestgeno = *bestgenome; 
            for(ii=0; ii < ind->numchars; ii++, cgeno++, cbestgeno++)
              *cgeno = *cbestgeno;
	    adaptivev = 0;
	    nrejected++;
	  }
	  else
	  {
            cgeno = *genome;
            cbestgeno = *bestgenome; 
            for(ii=0; ii < ind->numchars; ii++, cgeno++, cbestgeno++)
              *cbestgeno = *cgeno;
	    oldperformance = newperformance;
	    totperformance = newperformance;
	    adaptivev = 1;
	    evaluatedn = 1;
	    nrejected = 0;
	  }
	  
	  if (userbreak == 1) 
	  {
	    evolution_is_running = 0;
	    break;
	  }

	  // display statistics (fitness curve)
	  if (userbreak == 0) 
	  {
	    if (g < (MAXGENERATIONS - 1)) 
	    {
	      if (adaptivev == 1)
	      {
	       g++;
               savebestg(g);
	      }
	      statfit[g][0] = oldperformance;
	      statfit[g][1] = 0.0;
	      statfit_n = g + 1;
	      if (oldperformance > statfit_max)
		statfit_max = newperformance;
	      if (oldperformance < statfit_min)
		statfit_min = newperformance;
	      if (console == 0) 
	      {
		 update_rendevolution(1);
	      }
	    }
	  }

	  // we periodically re-test performace of individuals before variations
          if (nrejected > 0 && ((nrejected % 10) == 0))
	  {
	   totperformance += eval_team(0, 0, 0);
	   evaluatedn++;
	   oldperformance = totperformance / (double) evaluatedn;
	  }
	  
	}

}


/*
 * run n replications of the learning process
 */
void runlearning()
{
    int    replication;
    int    g,gg;
    int    oldseed;
    FILE   *fp;
    char   sbuffer[128];

    oldseed = seed;
    
    // experiment replications
    for (replication=0; replication < nreplications; replication++) {
        set_seed(seed);
        randomize_pop();
        if (startgeneration > 0) {
            loadallg(startgeneration, "nofile");
            sprintf(sbuffer,"stats%d.fit",seed);
            loadstatistics(sbuffer, 1);
        }
        

	individual_learning();
	
	if (userbreak == 1) {
            evolution_is_running = 0;
            testindividual_is_running = 0;
            break;
        } else {
            seed++;
        }
    }
    if (userbreak == 0 && nreplications > 1) {
        if ((fp=fopen("stat.fit", "w")) == NULL) {
            display_error("cannot write stat.fit file");
        }
        for(g=0;g<generations;g++) {
            for(gg=0;gg < (2*npopulations);gg++)
                fprintf(fp,"%.3f ",astatfit[g][gg] / (double) nreplications);
            fprintf(fp,"\n");
        }
        fclose(fp);
    }
    seed               = oldseed;
    userbreak          = 0;
    evolution_is_running = 0;
    testindividual_is_running = 0;
}