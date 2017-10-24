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

/*
 * receive a weights as an integer between 0 and 255
 * and return a floating point value within a given range
 * if the range is negative randomly generate the value
 * (255+1 in order to have 128=0.0)
 */

float
getbits(int v, float range)
{
	
	float value;
	
	if (range < 0.0)
	{
		value = rans(0.0 - range);
		return(value);
	}
	else
	{
		// this is for compatibility with old weights file (which include also negative values)
		if (v < 0)
			v = 256 + v;
		
		value = (float) (v + 1) / (float) 256.0;
		value = range - (value * (range * (float) 2.0));
		
		return(value);
	}

}



/*
 * set the free parameter of individual n, team t
 */
void
getgenome(struct  individual *pind, int n, int t)

{
	
	int     j;
	int	*gi;	
	float   *w;
	float   range;
	int     numc;

	/* we set weights range */
	range = wrange;

        gi = *(genome + n);

	if (homogeneous == 0)
	{
	  numc = pind->numchars / nteam;
          gi = (gi + (numc * t));
	}
	 else
	{
          numc = pind->numchars; 
	}

	for (j=0, w = pind->freep; j < numc; j++,w++, gi++)
	{
          *w  = getbits((int) *gi, range);
	}

}



/*
 * save the net n of population p on the fp file 
 */ 
void
saveagenotype(FILE *fp, int n, int p)

{
    int	j;
    struct  individual *pind;	
    int     *gi;

    /* we get the right population and genotype */
    pind = (ind + p);
    gi = *(genome + (n + (p * (nindividuals + additional_ind))));
     
    fprintf(fp, "DYNAMICAL NN\n");
    for (j=0; j < pind->numchars; j++, gi++) 
      fprintf(fp, "%d\n", *gi);
    fprintf(fp, "END\n");
}



/*
 * load the genome of individual n of population p from the fp file
 */ 
void
loadagenotype(FILE *fp, int n, int p)
{

    struct  individual *pind;	
    int	*gi;
    int	j;

    pind = (ind + p);
    gi = *(genome + (n + (p * (nindividuals + additional_ind))));

    fscanf(fp, "DYNAMICAL NN\n");
    for (j=0; j < pind->numchars; j++, gi++)
	{
	  fscanf(fp, "%d\n", gi);
	}
    fscanf(fp, "END\n");
}





/*
 * save the population genome
 */ 
void
saveallg(int gen)

{

	FILE	*fp;
        char    filename[64];
        char    errorm[64];	
        int     n;
	int     p;

	for(p=0;p<npopulations;p++)
	 {
 	  sprintf(filename,"G%dP%dS%d.gen",gen,p,seed);
	  if ((fp=fopen(filename, "w+")) == NULL) 
	  {
		sprintf(errorm,"cannot open %s file",filename);
		display_error(errorm);
	  }
	  for(n=0;n<nindividuals;n++)	   
	  {
	        fprintf(fp,"**NET : %d_%d_%d.wts\n",gen,p,n);		
	        saveagenotype(fp,n,p);
	  } 
	  fflush(fp);	       
	  fclose(fp);
	 }
}



/*
* load the populations genome
* if called a number >=0 load all populations
* if called with a filename load only the population indicated in the filename
*  in this case it can load up to (individuals+additional_ind) individuals 
* we assume we have a population only
*/ 
int
loadallg(int gen, char *filew)

{
	
	struct  individual *pind;
	FILE	*fp;
	char    filename[64];
	char    message[128];
	char    flag[64];
	int     n;
	int     p;
	int     nloaded;
	int     max;
	
	for(p=0, pind=ind; p < 1; p++, pind++)
	{
	  max = nindividuals;
	  if (gen == -1)
	    max = nindividuals + additional_ind;

	  if (gen >= 0)
		sprintf(filename,"G%dP%dS%d.gen",gen,p,seed);
		else
		sprintf(filename,"%s",filew);
		if ((fp=fopen(filename, "r")) != NULL) 
			{
			  for(n=0,nloaded=0;n<max;n++)	   
				{
				  sprintf(flag,""); //flag = NULL;
				  fscanf(fp, "%s : %s\n", flag, message);
				  if (strcmp(flag,"**NET") == 0)
					{
					  loadagenotype(fp,nloaded,p);
					  dindividual = nloaded;
					  nloaded++;
					}
					else
					{
					  break;
					}
				}
			    sprintf(message,"loaded pop: %d ind: %d",p,nloaded);
			    display_stat_message(message);
			    fclose(fp);
			}
			else
			{
			  sprintf(message,"file %s could not be opened\n",filename); 
			  display_stat_message(message);
			}
	}
   return(nloaded);
	
}



/*
 * copy the bestgenome nn into genome n making mutations
 */
void
getbestgenome(int n, int nn, int p, int domutate)

{

        struct individual *pind;	
	int     *gi;
	int     *bgi;	
        int     i;

	/* we get the right population and genotype */
	pind = (ind + p);
        gi = *(genome + (n + (p * (nindividuals + additional_ind))));
        bgi = *(bestgenome + (nn + (p * bestfathers)));
	
	for (i=0;i < pind->numchars;i++,gi++,bgi++)
	   if (domutate == 0)
              *gi = *bgi;
	     else
              *gi = mutate(*bgi);


}



/*
 * copy genome n in the bestgenome nn 
 */
void
putbestgenome(int n, int nn, int p)

{


    struct individual *pind;	
    int    *gi;
    int    *bgi;	
    int     i;

    /* we get the right population and genotype */
    pind = (ind + p);
    gi = *(genome + (n + (p * (nindividuals + additional_ind))));
    bgi = *(bestgenome + (nn + (p * bestfathers)));
	
    for (i=0;i < pind->numchars;i++,gi++,bgi++)
              *bgi = *gi;	      

}


/*
 * load fitness statistics (best and average performance)
 * mode=1 display graph, mode=0 do not display the graph
 */ 
int
loadstatistics(char *files, int mode)

{


     FILE	*fp;
     char       line[128];
     char       word[128];
     int n;
     int nc;

     statfit_max = 0.0;
     statfit_min = 0.0;
     statfit_n = 0;
     if ((fp=fopen(files, "r")) != NULL) 
	{
	  n = 0;
	  while (fgets(line,128,fp) != NULL && n < MAXGENERATIONS)
	  {
            statfit[n][0] = 0;
            statfit[n][1] = 0;
            nc = getword(word,line,' ');
	    if (nc > 0)
	      statfit[n][0] = atof(word);
            nc = getword(word,line,' ');
	    if (nc > 0)
	      statfit[n][1] = atof(word);
	    if (statfit[n][0] > statfit_max)
	      statfit_max = statfit[n][0]; 
	    if (statfit[n][1] > statfit_max)
	      statfit_max = statfit[n][1]; 
	    if (statfit[n][0] < statfit_min)
	      statfit_min = statfit[n][0]; 
	    if (statfit[n][1] < statfit_min)
	      statfit_min = statfit[n][1]; 
	    n++;
	  }
	  statfit[n][0] = -9999.0;
	  statfit_n = n;
	  if (mode == 1)
	  {
           sprintf(line,"loaded n: %d data from file %s",n, files);
           display_stat_message(line);
	   if (mode == 1)
             update_rendevolution(1);
	  }
	  return(n);
        }
       else
       {
	 statfit_n = 0;
         sprintf(line,"Error: the file %s could not be opended",files);
	 display_stat_message(line);
       }

}



/*
 * load multiseed statistics of the current population
 * display all graph and the ranking of replications
 * if evolution is running, simply show the statistics of the current running seed
 */ 
void
display_all_stat()

{


     double max_all;
     double min_all;
     int    n_all;
     char   filen[128];
     double max_seed[20];
     int    best_ids[20];
     int    nrep;
     int    r;
     int    rr;
     double seed_max;
     int    seed_id;
     char   message[256];

     if (evolution_is_running ==0)
     {

     // we load all files to compute the fitness range and the best seeds
     max_all = 0.0;
     min_all = 0.0;
     n_all = 0;
     for (r=0; r < 20; r++)
     {
       max_seed[r] = 0.0;
       best_ids[r] = 0;
     }

     if (nreplications < 20)
       nrep = nreplications;
      else
       nrep = 20;
     for (r=0; r < nrep; r++)
     {
       sprintf(filen,"statS%d.fit",seed + r);
       loadstatistics(filen, 0);
       max_seed[r] = statfit_max;
       if (statfit_max > max_all)
	  max_all = statfit_max;
       if (statfit_min < min_all)
	  min_all = statfit_min;
       if (statfit_n > n_all)
	  n_all = statfit_n;
     }

     // we compute and display the ranking
     for (r=0; r < nrep; r++)
        {
	 seed_max = 0.0f;
	 seed_id  = 0;
         for (rr=0; rr < nrep; rr++)
          {
            if (max_seed[rr] > seed_max)
	       {
		seed_max = max_seed[rr];
		seed_id = rr;
	       }
          }
         best_ids[r] = seed_id + seed;
	 max_seed[seed_id] = 0.0f;
        }
        if (nrep <= 10)
          {
            sprintf(message,"best seeds: %d %d %d %d %d %d %d %d %d %d - %.1f %.1f %.1f",
	             best_ids[0], best_ids[1], best_ids[2], best_ids[3], best_ids[4], 
	             best_ids[5], best_ids[6], best_ids[7], best_ids[8], best_ids[9], 
		     max_seed[0], max_seed[1], max_seed[2]);
	    display_stat_message(message);
          }
	  else
          {
            sprintf(message,"best seeds: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
	             best_ids[0], best_ids[1], best_ids[2], best_ids[3], best_ids[4], 
	             best_ids[5], best_ids[6], best_ids[7], best_ids[8], best_ids[9],
	             best_ids[10], best_ids[11], best_ids[12], best_ids[13], best_ids[14], 
	             best_ids[15], best_ids[16], best_ids[17], best_ids[18], best_ids[19]);
	    display_stat_message(message);          
	  }
     //we now update the renderarea
     statfit_max = max_all;
     statfit_min = min_all;
     statfit_n = n_all;
     update_rendevolution(2);
     }
     else
     {
      update_rendevolution(1);
     }
}



/*
 * Save the phenotype of the first individual of the team
 * taken from the genotype of individual dindividual
 * does not handle nonhomogeneous teams
 */
void
save_phenotype(char *filen)

{

    float *p;
    struct individual *pind;

    pind = ind;
    getgenome(pind,dindividual,0);
    p   = pind->freep;

    // if the user select an file called empty
    // we save default dont care parameters for all values
    if (strstr(filen, "empty") != NULL)
      p = phenotype;
    
    save_phenotype_data(filen, p);

}


/*
 * Load a phenotype from a file into an additional individual (nteam + 1)
 */
void
load_phenotype(char *filen)

{
    if (load_phenotype_data(filen, phenotype) == 1)
      pheno_loaded = 1;

}



/*
 * overwrite a phenotype or part of it on the basis
 *  of loaded parameters (999.0 = don't care)
 */
void
overwrite_phenotype(struct individual *pind)

{
    int i;
    float *p;
    float *pp;

    p    = pind->freep;
    pp   = phenotype;

    for(i=0; i < pind->numchars; i++, p++, pp++)
     if (*pp != 999.0)
      *p = *pp;

}


/*
* generate the file lineage.gen from best.gen files
*/ 
void
create_lineage()

{
     int     g;
     int     v1;
     int     v2;
     char    message[128];
     char    flag[128];
     int     gg;
     int     f;
     FILE    *fp;
     char    filename[64];
     int     replication;
     int     s;

     for (replication=0,s=seed; replication < nreplications; replication++, s++)
       {
	// we load the lineage starting from the last gen
	f = 1; 
	for(g = (generations - 1); g >= 0; g--)
	 {
	   sprintf(filename,"B%dP%dS%d.gen",f, 0, s);
	   if ((fp=fopen(filename, "r")) != NULL)
	    {
	      for(gg=0; gg <= g; gg++)
	        {
		   sprintf(flag,"");
		   fscanf(fp, "%s : s%d_%d.wts\n", flag, &v1,&v2);
		   if (strcmp(flag,"**NET") == 0)
	             loadagenotype(fp,gg,0);
		   else
	            {
	             sprintf(message,"unable to load seed %d best %d generation %d",s,f,gg);
	             display_stat_message(message);
	             fclose(fp);
		     return;
	            }
		 }
	      sprintf(message,"loaded seed %d best %d generation %d",s,f,g);
	      display_stat_message(message);
	      fclose(fp);
	     }
	     else
	     {
	       sprintf(drawdata_st,"file %s could not be opened\n",filename); 
	       display_warning(drawdata_st);
	       return;
	     }
	   f = (v2 / nkids) + 1;
	 }
	// we save the lineage 
 	sprintf(filename,"lineageS%d.gen",s);
	if ((fp=fopen(filename, "w")) != NULL) 
	  {
	    for(g=0;g<generations;g++)	   
	     {
	        fprintf(fp,"**NET : s%d_%d.wts\n",g,0);		
	        saveagenotype(fp,g,0);
	     } 
	     fflush(fp);	       
	     fclose(fp);
	     sprintf(message,"lineageS%d.gen have been saved",s);
	     display_stat_message(message);
	  }
	  else
	  {
	    sprintf(drawdata_st,"cannot open %s file",filename);
	    display_warning(drawdata_st);
	  }
      }

}

/*

	int     b;
	int     lastg;
	int     stop;
	int     index[50][10000];


        // check index allocation
	if (bestfathers > 50)
	 {
	  display_warning("insufficient index allocation");
	  return;
	 }

	// we load indexes from best .gen files
	lastg = 0;
	for(b=0; b < bestfathers; b++)
	 {
	   sprintf(filename,"B1P0S%d.gen",seed);
	   if ((fp=fopen(filename, "r")) != NULL) 
		{
		  g = 0;
		  stop = 0;
		  while(stop == 0 && g < 10000 && g < (nindividuals + additional_ind))	   
		   {
		     sprintf(flag,"");
		     fscanf(fp, "%s : s%d_%d.wts\n", flag, &v1,&v2);
		     if (strcmp(flag,"**NET") == 0)
			{
			  loadagenotype(fp,g,0);
			  index[b][g] = v2;
			  g++;
			  if (g > lastg)
			   lastg = g;
			}
			else
			{
			  stop = 1;
			}
		   }
		   sprintf(message,"loaded seed %d best %d generations %g",seed,b,g);
		   display_stat_message(message);
		   fclose(fp);
		  }
		  else
		  {
		    sprintf(drawdata_st,"file %s could not be opened\n",filename); 
		    display_warning(drawdata_st);
		    return;
		  }
	     }
*/