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
 * evolution.cpp
 */

#include "defs.h"
#include "public.h"

/*
 * allocate space for individual phenotypes
 * forming the team
 * it also allocate space for phenotype parameters to be loaded
 */
void
create_ind_phenotype()

{
        struct  individual *pind;	
	int     team;
	int     i;
	float   *p;

        /*
         * for each individual forming the team
	 */
	for (team = 0, pind=ind; team < nteam; team++, pind++)
	{
	   pind->freep = (float *)malloc(pind->numchars * sizeof(float));
	}

	phenotype = (float *)malloc(ind->numchars * sizeof(float));
	for (i=0,p=phenotype; i < ind->numchars; i++, p++)
	   *p = 999.0;

}


/*
 * select the best individual (on the basis of statfit)
 * place the results in dindividual
 */
void
select_besti()

{
       int     g;
       double  bestfit;
       int     besti;


       bestfit = -9999.9;
       besti   = -1;
       for(g = 0; g < generations; g++)
	{
	   if (statfit[g][0] == -9999.9)
	      break;
	   if (statfit[g][0] > bestfit)
		{
		  bestfit = statfit[g][0];
		  besti = g;
		}
       }
       if (besti < (nindividuals + additional_ind))
	  dindividual = besti;
}



/*
 * Decode genotype of individuals and let them live
 */
double
eval_team(int gen, int nind, int pop)
{
 
	struct  individual *pind;		
	int team;
	double totfitness;

       // initialize epoch variables
       for(team=0,pind=ind;team<nteam;team++, pind++)
	   {
		pind->fitness   = 0.0;
		pind->oldfitness= 0.0;
		pind->totcycles = 0;
		pind->number = team;
		getgenome(pind,nind,team);
		if (pheno_loaded == 1)
                  overwrite_phenotype(pind);
	   }         
       pind = ind;

       for (cepoch=0, cworld=0;cepoch<amoebaepochs;cepoch++)
	   {
	       loop(gen,pop,nind,cepoch);
	       // we change world if we have more than one
	       if (nworlds > 1)
		{
		  cworld++;
		  if (cworld >= nworlds)
		     cworld = 0;
		 }
	       if (userbreak == 1)
		   {
                    cepoch = amoebaepochs;
		    testindividual_is_running = 0;
		    evolution_is_running = 0;
		   }
	   }
        // we assume a cooperative selection schema
        for(team=0,totfitness=0.0, pind = ind; team < nteam; team++, pind++)
	  totfitness += pind->fitness;
	// we normalize fitness 
	switch(fitmode) 
	    {
		case 1:
		  totfitness = totfitness / (double) (sweeps * amoebaepochs);
		  break;
		case 2:
		  totfitness = totfitness / amoebaepochs;
		  break;
		case 3:
		  totfitness = totfitness;
		  break;
	    }

        tfitness[0][nind] = totfitness;      
	return(totfitness);
}



/*
 * return the mutated version of an integer
 * this function is called by getbestweights()
 */ 
int
mutate(int w)
{
    
      int b[8];
      int val;
      int ii;


      val = w;
      for(ii=0;ii < 8;ii++)
	  {
	    b[ii] = val % 2;
	    val  = val / 2;
	  }    
      for(ii=0;ii < 8;ii++)
	  {
	     if (mrand(100) < percmut)
		 { 
	        b[ii] = mrand(2);
		 } 
	  } 
      w = 0;
      w += b[0] * 1;
      w += b[1] * 2;
      w += b[2] * 4;
      w += b[3] * 8;
      w += b[4] * 16;
      w += b[5] * 32;
      w += b[6] * 64;      
      w += b[7] * 128;   
 
      return(w);
	 
}


/*
* produce a new generations
* if last generations it saves the best but does not generate the new population
*/
void reproduce(int g)
{
	
	
	
	struct  individual *pind;     
	char sroot[64];
	char sbuffer[64];
	FILE *fp;
	int p;
	int f;
	int x;
	int n;
	int k;
	float v;
	
	
	for(p = 0, pind = ind; p < npopulations; p++, pind++)
	{
	  for(f=0;f<bestfathers;f++)
	  {
	    x = 0;
	    v = (float) -9999.0;
	    for(n = 0;n < nindividuals;n++)
	    {
	     if (elitism == 2)
	      {
	       if ((pfitness[n] / (double) plifes[n]) >= v)
	       {
		x = n;
		v = (float) (pfitness[n] / (double) plifes[n]);
	       }
	      }
	       else
	      {
	        if (tfitness[p][n] >= v)
		{
		  x = n;
		  v = (float) tfitness[p][n];
		}
	      }
	    }
	    putbestgenome(x,f,p);	       
	    tfitness[p][x] = -9999.0;
	    if (elitism == 2)
	      {
	       bpfitness[f] = pfitness[x];
	       bplifes[f] = plifes[x];
	       pfitness[x] = -9999.0;
	       plifes[x] = 1;
	      }
	    // if we restarted evolution we do not save the bests of the initial generation
	    if ((f+1) <= savebest && (startgeneration == 0 || g > startgeneration))
		{
		 sprintf(sbuffer,"B%dP%dS%d.gen",f+1,p,seed);
		 if (g == 0)
		 {
		   if ((fp=fopen(sbuffer, "w")) == NULL)
		   {
		     sprintf(sroot,"I cannot open file B%dP%dS%d.gen",f+1,p,seed);
		     display_error(sroot);
		   }
		 }
		 else
		 {
		  if ((fp=fopen(sbuffer, "a")) == NULL)
		  {
		    sprintf(sroot,"I cannot open file B%dP%dS%d.gen",f+1,p ,seed);
		    display_error(sroot);
		  }
		}
	        fprintf(fp,"**NET : s%d_%d.wts\n",g,x);
	        saveagenotype(fp,x,p);
	        fflush(fp);
	        fclose(fp);
		}
	  }
	}
	// reproduce
	if (g+1 < generations)
	{
	  for(p=0, pind = ind; p < npopulations; p++, pind++)
	  {
	    for(f=0;f<bestfathers;f++)
	    {
	       for (k=0;k<nkids;k++)
		{
                  if (elitism > 0 && k == 0)
		    getbestgenome(k+(f*nkids),f,p,0);
		   else
		    getbestgenome(k+(f*nkids),f,p,1);
		   if (elitism == 2)
		   {
		     if (k == 0)
                       pfitness[k+(f*nkids)] = bpfitness[f];
		      else
                       pfitness[k+(f*nkids)] = 0.0;
		     if (k == 0)
                       plifes[k+(f*nkids)] = bplifes[f];
		      else
                       plifes[k+(f*nkids)] = 0;
		   }		       
		}
	    }
	  }
	}
}

/*
* produce a new generations
* if last generations it saves the best but does not generate the new population
*/
void reproduce2(int g)
{
	
	
	
	struct  individual *pind;     
	char sroot[64];
	char sbuffer[64];
	FILE *fp;
	int p;
	int f;
	int x;
	int n;
	int k;
	float v;
	
	
	for(p = 0, pind = ind; p < npopulations; p++, pind++)
	{
	  for(f=0;f<bestfathers;f++)
	  {
	    x = 0;
	    v = (float) -9999.0;
	    for(n = 0;n < nindividuals;n++)
	     if (elitism == 2)
	      {
	       if ((pfitness[n] / (double) plifes[n]) >= v)
	       {
		x = n;
		v = (float) (pfitness[n] / (double) plifes[n]);
	       }
	       else
	       {
	        if (tfitness[p][n] >= v)
		{
		 x = n;
		 v = (float) tfitness[p][n];
		}
	       }
	      }
	    putbestgenome(x,f,p);	       
	    if (elitism == 2)
	      {
	       bpfitness[f] = pfitness[x];
	       bplifes[f] = plifes[x];
	       pfitness[x] = -9999.0;
	       plifes[x] = 1;
	      }
	    tfitness[p][x] = -9999.0;

	    // if we restarted evolution we do not save the bests of the initial generation
	    if ((f+1) <= savebest && (startgeneration == 0 || g > startgeneration))
		{
		 sprintf(sbuffer,"B%dP%dS%d.gen",f+1,p,seed);
		 if (g == 0)
		 {
		   if ((fp=fopen(sbuffer, "w")) == NULL)
		   {
		     sprintf(sroot,"I cannot open file B%dP%dS%d.gen",f+1,p,seed);
		     display_error(sroot);
		   }
		 }
		 else
		 {
		  if ((fp=fopen(sbuffer, "a")) == NULL)
		  {
		    sprintf(sroot,"I cannot open file B%dP%dS%d.gen",f+1,p ,seed);
		    display_error(sroot);
		  }
		}
	        fprintf(fp,"**NET : s%d_%d.wts\n",g,x);
	        saveagenotype(fp,x,p);
	        fflush(fp);
	        fclose(fp);
		}
	  }
	}
	// reproduce
	// if we have elitism, we do not mutate one of the offspring of each reproducing indvidual
	if (g+1 < generations)
	{
	  for(p=0, pind = ind; p < npopulations; p++, pind++)
	  {
	    for(f=0;f<bestfathers;f++)
	    {
	       for (k=0;k<nkids;k++)
		{
                  if (elitism > 0 && k == 0)
		    getbestgenome(k+(f*nkids),f,p,0);
		   else
		    getbestgenome(k+(f*nkids),f,p,1);
		   if (elitism == 2)
		   {
		     if (k == 0)
                       pfitness[k+(f*nkids)] = bpfitness[f];
		      else
                       pfitness[k+(f*nkids)] = 0.0;
		     if (k == 0)
                       plifes[k+(f*nkids)] = bplifes[f];
		      else
                       plifes[k+(f*nkids)] = 0;
		   }

		}
	    }
	  }
	}
}
 
 


/*
 * main evolutionary loop (calling eval_team and loop)
 * also used for testing all individuals
 */
void
evolution()
{

       int    n,g,p,nn,pp;
       double bestfit,avefit;
       char   sbuffer[128];
       FILE   *fp;

       statfit_max = 0;
       statfit_min = 0;
       statfit_n = 0;
       if (elitism == 2)
	 for(n=0;n<nindividuals;n++)
	 {
	   pfitness[n] = 0.0;
	   plifes[n] = 0;
	 }
       for(g=startgeneration;g<generations;g++)
	   {
	     for (p=0;p<npopulations;p++)
	     {
	       if (masterturnament == 1)
		 nindividuals = nindividuals + additional_ind;
	       for(n=0;n<nindividuals;n++)
	       {
		if (pop_seed == 1)
		   set_seed(seed+g);
		eval_team(g,n,p);
	        if (elitism == 2)
		{
		 plifes[n] += 1;
		 pfitness[n] += tfitness[p][n];
		}

		if (userbreak == 1)
		{
		    evolution_is_running = 0;
		    break;
		}
		if (console == 0 || verbose > 1)
			{
		          sprintf(sbuffer, "Seed %d Pop %d Gen.%d  Ind.%d  Fit.%.3f",seed,p,g,n,tfitness[p][n]);
		          display_stat_message(sbuffer);
			}
		}

	     }
	      // save fitness for each individual
	      if (saveifit == 1)
	       for(n = 0;n < npopulations; n++)
		{ 
		  sprintf(sbuffer,"FitP%dS%d.txt",n,seed);
		  if (g == 0)
		    fp=fopen(sbuffer , "w");
		   else
		    fp=fopen(sbuffer , "a");
		  if (fp == NULL)
		    display_error("unable to save individual fitnesses on a file");
		  for(nn = 0; nn < nindividuals; nn++)
		     fprintf(fp,"%.2f ",tfitness[n][nn]);
		  fprintf(fp,"\n");
		  fflush(fp);
		  fclose(fp);
		}
	       // save statistics
	       sprintf(sbuffer,"statS%d.fit",seed);
	       if (g == 0)
		 fp=fopen(sbuffer , "w");
		else
		 fp=fopen(sbuffer , "a");
	       if (fp == NULL)
		  display_error("unable to save statistics on a file");
	       for(pp = 0;pp < npopulations; pp++)
		{
		  bestfit = -99990.0;
		  avefit  = 0.0;
		  if (elitism == 2)
		    for(n = 0;n < nindividuals;n++)	
			{
			  avefit += pfitness[n] / (double) plifes[n];
			  if ((pfitness[n] / (double) plifes[n]) >= bestfit)
			     bestfit = (pfitness[n] / (double) plifes[n]);
			}
		      else
		    for(n = 0;n < nindividuals;n++)	
			{
			  avefit += tfitness[pp][n];
			  if (tfitness[pp][n] >= bestfit)
			     bestfit = tfitness[pp][n];
			}
		   avefit = avefit / (double) nindividuals;
		   if (console == 1 && verbose > 0)
			{
		          sprintf(sbuffer, "gen %d) %.2f %.2f",g,bestfit,avefit);
		          display_stat_message(sbuffer);
			}
		   if (userbreak == 0)
			{
		         fprintf(fp,"%.3f %.3f \n",bestfit,avefit);		   
		          if (g < MAXGENERATIONS)
				{
				  statfit[g][0+pp*2] = bestfit;
				  statfit[g][1+pp*2] = avefit;
				  astatfit[g][0+pp*2] += bestfit;
				  astatfit[g][1+pp*2] += avefit;
				  statfit_n = g + 1;
                                  if (bestfit > statfit_max)
				    statfit_max = bestfit;
				  if (avefit > statfit_max)
				    statfit_max = avefit;
                                  if (bestfit < statfit_min)
				    statfit_min = bestfit;
				  if (avefit < statfit_min)
				    statfit_min = avefit;
				  update_rendevolution(1);
				}
			}

		}
		fflush(fp);
		fclose(fp);
          
		// eventually we save the last generation
		if (g == 0 || userbreak == 1 || (g>0 && (savebest==0 || g+1 == generations)))
		{
		  if (userbreak == 1)
		   saveallg(g-1);
		  else
		   saveallg(g);
		}

		if (userbreak == 1)
		   break;

		if (g < generations && masterturnament == 0)
		   reproduce(g);	 	 
      } // end generations

}

/*
 * test one individual
 */

void test_one_individual()
{


  eval_team(startgeneration, dindividual,0);

  testindividual_is_running = 0;
  userbreak = 0;
}


/*
 * test all best individuals of all replications
 * results are save in file master%s.fit
 * assume that we only have a single population
 */
void test_all_individuals()
{

  int    replication;
  int    n;
  char   sbuffer[256];
  FILE   *fp;
  int    nloadedindividuals;

  for (replication=0; replication < nreplications; replication++)
	{
          statfit_max = 0;
          statfit_min = 0;
	  sprintf(sbuffer,"B1P0S%d.gen",seed + replication);
	  nloadedindividuals = loadallg(-1, sbuffer);
	  if (nloadedindividuals > (nindividuals + additional_ind))
	  {
	    nloadedindividuals = nindividuals + additional_ind;
	    display_warning("please increase the number of additional_ind to allocate enough space");
	  }
	  for(n=0;n<nloadedindividuals;n++)
		{
	        sprintf(drawdata_st,"testing replication %d (%d individuals) individual %d for %d trials",seed + replication,nloadedindividuals,n, amoebaepochs);
	        display_stat_message(drawdata_st);
		eval_team(0,n,0);
		if (userbreak == 1)
		    break;
		drawstat = 1;
		statfit_n = n + 1;
		if (tfitness[0][n] > statfit_max)
		  statfit_max = tfitness[0][n];
		if (tfitness[0][n] < statfit_min)
		  statfit_min = tfitness[0][n];
                statfit[n][0] = tfitness[0][n];
                statfit[n][1] = 0.0;
	        update_rendevolution(1);
		}
	   if (userbreak == 1)
	     break;
           sprintf(sbuffer,"masterS%d.fit",seed + replication);
	   fp=fopen(sbuffer , "w");
	   if (fp == NULL)
             display_error("unable to save statistics on a file");
	   for(n=0;n<nloadedindividuals;n++)
		{
		  fprintf(fp,"%.3f 0.0\n",tfitness[0][n]);		   
		}
	   fflush(fp);
	   fclose(fp);
	}

}


/*
 * allocate space for the genome of the population
 * for the genome of the reproducing individuals
 */
void
create_genome()

{
        struct  individual *pind;	
	int     i;
	int     j;
	int     p;
	int	**gpi;
	int	*gi;	
			
        /*
	 * compute the number of individuals
	 */
        nindividuals=nkids * bestfathers;

        /* 
	 * allocate space for the genome of individuals
	 * and initialize
	 */
 	genome = (int **) malloc(((nindividuals + additional_ind) * npopulations) * sizeof(int *));
	if (genome == NULL) 
	    display_error("System error: genome malloc");

	for (p = 0, pind=ind, gpi=genome; p < npopulations; p++, pind+=nteam)
	{
		  /* we allocate space for individual genome */
		  for (i=0; i < nindividuals; i++,gpi++) 
		  {
				 *gpi = (int *)malloc(pind->numchars * sizeof(int));
				 if (*gpi == NULL)
					display_error("System error: gpi malloc");
					 for (j=0,gi = *gpi; j < pind->numchars; j++,gi++)
					 {
						*gi = mrand(256);
					 }
		  }
		  /* we allocate space for best genome of previous generations */
		  for (i=0; i < additional_ind; i++,gpi++) 
		  {
				 *gpi = (int *)malloc(pind->numchars * sizeof(int));
				 if (*gpi == NULL)
					display_error("System error: previous genome malloc");
					 for (j=0,gi = *gpi; j < pind->numchars; j++,gi++)
					 {
						*gi = mrand(256);
					 }
		  }
	}

        /* 
	 * allocate space for the genome of reproducing ind.
	 */
 	bestgenome = (int **) malloc((bestfathers * npopulations) * sizeof(int *));
	if (bestgenome == NULL)
	   display_error("System error: bestgenome malloc");
	for (p = 0, pind=ind, gpi=bestgenome; p < npopulations; p++, pind+=nteam)
	  {
	    for (i=0; i < bestfathers; i++,gpi++) 
	       {
	         *gpi = (int *)malloc(pind->numchars * sizeof(int));
	         if (*gpi == NULL) 
	             display_error("System error: bestgi malloc");
	        }
	  }


}


/*
 * randomize the genomes of the entire population
 */
void
randomize_pop()

{

	struct  individual *pind;	
	int     i;
	int     j;
	int     p;
	int	**gpi;
	int	*gi;
	
	for (p = 0, pind=ind, gpi=genome; p < 1; p++, pind++)
	{ 
	  for (i=0; i < (nindividuals + additional_ind); i++,gpi++) 
	  {
                 for (j=0,gi = *gpi; j < pind->numchars; j++,gi++)
		    {
		     *gi = mrand(256);
		    }
	  }
	}
}


/*
 * run n replications of the evolutionary process
 */
void runevolution()
{

  int    replication;
  int    g,gg;
  int    ng;
  int    oldseed;
  FILE   *fp;
  char   sbuffer[128];

  oldseed = seed;
  for (replication=0; replication < nreplications; replication++)
	{
	  set_seed(seed);
	  randomize_pop();
	  sprintf(sbuffer,"statS%d.fit",seed);
	  if ((fp=fopen(sbuffer, "r")) == NULL) 
	    {
	      evolution();
	    }
	   else
	    {
	      fclose(fp);
              ng = loadstatistics(sbuffer, 1);
	      if (ng < generations)
	       {
	        startgeneration = ng;
		loadallg(startgeneration - 1, "nofile");
	        evolution();
		startgeneration = 0;
	       }
	    }
	  if (userbreak == 1)
	    {
              evolution_is_running = 0;
	      testindividual_is_running = 0;
	      break;
	    }
	    else
	    {
	      seed++;
	    }
	}
    if (userbreak == 0 && nreplications > 1)
	{
	  if ((fp=fopen("stat.fit", "w")) == NULL) 
	    {
	      display_error("cannot write stat.fit file");
	    }	    
	  for(g=0;g<generations;g++)
	    {
	      for(gg=0;gg < (2*npopulations);gg++)
	        fprintf(fp,"%.3f ",astatfit[g][gg] / (double) nreplications);		
	      fprintf(fp,"\n");				  
	    } 
	  fclose(fp);
	}
      seed = oldseed;
      userbreak = 0;
      evolution_is_running = 0;
      testindividual_is_running = 0;
}
