/* 
 * Evorobot* - evolution.cpp
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
 * evolution.cpp
 */

#include "public.h"
#include "evolution.h"

int     seed=0;                      // random seed                              
int     generations=0;               // number of generations                    
int     nkids=0;                     // number of kids for father                
int     bestfathers=0;               // number of fathers that make kids         
int     startgeneration = 0;         // start generation (>=0 means load weig.)  
int     percmut=2;                   // percentage of mutations                  
int     pnullvalue=0;                // probability to chose a null value
int     elitism=0;                   // one copy of each best is preserved intact, if 2 we save previous fitness data 
int     evoalgorithm=0;              // 0 = truncation selection, 1= steady-state
int     savebest=1;                  // save only the best N individual each gen. 
int     saveifit=0;                  // save individuals' fitness
long int learningc=0;                // learning cycles
int     homogeneous=1;               // whether the team is formed by homogeneous individuals or not
int     nindividuals;                // number of individuals
int     npopulations=1;              // number of populations   
int     pop_seed=0;                  // whether seed is reinitialized for individuals of the same generation
int     additional_ind=0;            // n. of additional genome to be allocated
int     nreplications=0;             // n. of replications 
int     nteam=1;                     // number of individual situated in the same environment
int     dindividual=0;               //  current individual of the population
int     cindividual=0;               //  current individual of the team 
int     ngenes=0;                    // number of genes for each individual genome
int     **genome;                    // genome of the population
int     **bestgenome;                // genome of the best individuals
int     **fixedgenome;               // genome of fixed individuals
int     loadfixedgenotypes=0;        // control whether we load fixed genotypes
int     *loadfixedgn;                // the id of the fixed individuals 
int     masterturnament=0;           // master turnament                      
int     testindividual_is_running=0; // one individual is currently tested
int     evolution_is_running=0;      // evolution is currently running
int     fitmovaverage=1;             // display fitness, moving average over n. individuals
float   fitscale = -1.0;             // display fitness, y-scale (< 0 means that it is computed automatically)
int     adaptcurrent = 0;            // adapt the current phenotype
int     genoloaded = 0;              // flag that take trace of whether we should map the genotype into the phenotype before ìtesting it


double  tfitness[2][MAXINDIVIDUALS]; // populations fitness                  
double  pfitness[MAXINDIVIDUALS];    // fitness gathered in previous generations
int     plifes[MAXINDIVIDUALS];      // previous life periods in which fitness was gathered 
double  bpfitness[MAXINDIVIDUALS];   // best individuals - fitness gathered in previous generations
int     bplifes[MAXINDIVIDUALS];     // best individuals - previous life periods in which fitness was gathered 
double  statfit[MAXGENERATIONS][4];  //  best and mean fit for each gen. & pop   
double  statfit_min;                 // minimum fitness
double  statfit_max;                 // maximum fitness
int     statfit_n;                   // n fitness data
double  astatfit[MAXGENERATIONS][4]; // statfit averaged for replications 

int  winadaptationx = -1;            // x coordinate of adaptation window (-1 = undefined)
int  winadaptationy = -1;            // y coordinate of adaptation window (-1 = undefined)
int  winadaptationdx = 0;            // dx of controller adaptation window (0 = undefined)
int  winadaptationdy = 0;            // dy of controller adaptation window (0 = undefined)

/*
 * create the evolution parameters
 */
void
create_evolution_par()

{

   create_iparameter("npopulations","adaptation",&npopulations, 0, 0, "n. of evolving populations");
   create_iparameter("nteam","adaptation",&nteam, 0, 0, "number of individuals forming a team");
   create_iparameter("homogeneous","adaptation",&homogeneous, 0, 1, "whether the individual of the team are homogeneous or not");
   create_iparameter("ngenerations","adaptation",&generations, 0, 0, "number of generations");
   create_iparameter("nreproducing","adaptation",&bestfathers, 0, 0, "number of individuals allowed to reproduce");
   create_iparameter("offspring","adaptation",&nkids, 0, 0, "number of offspring");
   create_iparameter("learningc","adaptation",&learningc, 0, 1, "number of learning cycles");
   create_iparameter("nreplications","adaptation",&nreplications, 0, 0, "number of replications of the adaptive process");
   create_iparameter("seed","adaptation",&seed, 0, 0, "the seed of the random number generator");
   create_iparameter("pop_seed","adaptation",&pop_seed, 0, 0, "whether seed is the same for the individuals of the same generation");
   create_iparameter("mutation_rate","adaptation",&percmut, 0, 0, "percentage of bits that replaced with new random value");
   create_iparameter("pnullvalue","adaptation",&pnullvalue, 0, 0, "probability to replace with a null value");
   create_iparameter("elitism","adaptation",&elitism, 0, 2, "1=elitism 2=elitism and preservation of previous performace measure");
   create_iparameter("evoalgorithm","adaptation",&evoalgorithm, 0, 1, "0=truncation 1=steady-state");
   create_iparameter("savenbest","adaptation",&savebest, 0, 0, "number of best individuals saved for each generation");
   create_iparameter("saveifit","adaptation",&saveifit, 0, 0, "save individuals' performance");
   create_iparameter("additional_ind","adaptation",&additional_ind, 0, 0, "number of additional individuals to be allocated");
   create_iparameter("dindividual","adaptation",&dindividual, 0, 0, "current individual of the population");
   create_iparameter("cindividual","adaptation",&cindividual, 0, 0, "current individual of the team");
   create_iparameter("loadfixedgenotypes","adaptation",&loadfixedgenotypes, 0, 1, "load files evorobot?.gen for fixed individuals");
   create_iparameter("adaptcurrent","adaptation",&adaptcurrent, 0, 1, "adapt the current individual");

   create_iparameter("fitmovaverage","display",&fitmovaverage, 0, 0, "display fitness: moving average over n. individuals");
   create_fparameter("fitscale","display",&fitscale, 0, 0, "y-scale of fitness display, when manually set");
   create_iparameter("winadaptationx", "display",&winadaptationx, 0, 0, "x coordinate of adaptation window");
   create_iparameter("winadaptationy", "display",&winadaptationy, 0, 0, "y coordinate of adaptation window");
   create_iparameter("winadaptationdx", "display",&winadaptationdx, 0, 0, "dx of adaptation window");
   create_iparameter("winadaptationdy", "display",&winadaptationdy, 0, 0, "dy of adaptation window");
}

/*
 * allocate space for the genome of the population and for the reproducing individuals
 */
void
init_evolution()

{
    int     i;
    int     j;
    int     p;
    int    **gpi;
    int    *gi;    
    int     team;
    int     g, gg;

    // update the number of free parameters for allocating enough space
    ncontroller_read_nfreep(&ngenes);
    if (homogeneous != 1)
      ngenes = ngenes * ngenes;
    ngenes += 20;
        
    // compute the population size
        nindividuals=nkids * bestfathers;

    // allocate space for the genome of the population and initialize
     genome = (int **) malloc(((nindividuals + additional_ind) * npopulations) * sizeof(int *));
    if (genome == NULL) 
        display_error("System error: genome malloc");

    for (p = 0, gpi=genome; p < npopulations; p++)
    {
       for (i=0; i < nindividuals; i++,gpi++) 
           {
          *gpi = (int *)malloc(ngenes * sizeof(int));
          if (*gpi == NULL)
          display_error("System error: gpi malloc");
          for (j=0,gi = *gpi; j < ngenes; j++,gi++)
            {
			   if (pnullvalue == 0 || mrand(100) >= pnullvalue)
                 *gi = mrand(256);
			   else
                 *gi = 127;
            }
           }
        // we allocate space for the genome of additional individuaks
        for (i=0; i < additional_ind; i++,gpi++) 
         {
           *gpi = (int *)malloc(ngenes * sizeof(int));
           if (*gpi == NULL)
             display_error("System error: previous genome malloc");
           for (j=0,gi = *gpi; j < ngenes; j++,gi++)
             {
               *gi = mrand(256);
             }
          }
    }

    // allocate space for the genome of reproducing individuals
     bestgenome = (int **) malloc((bestfathers * npopulations) * sizeof(int *));
    if (bestgenome == NULL)
       display_error("System error: bestgenome malloc");
    for (p = 0, gpi=bestgenome; p < npopulations; p++)
      {
        for (i=0; i < bestfathers; i++,gpi++) 
           {
             *gpi = (int *)malloc(ngenes * sizeof(int));
             if (*gpi == NULL) 
                 display_error("System error: bestgi malloc");
            }
      }

    // allocate space for the genome of fixed individuals
    if (loadfixedgenotypes == 1)
        {
      fixedgenome = (int **) malloc(nteam * sizeof(int *));
     if (fixedgenome == NULL)
       display_error("System error: fixedgenome malloc");

     for (i=0, gpi=fixedgenome; i < nteam; i++, gpi++) 
           {
             *gpi = (int *)malloc(ngenes * sizeof(int));
             if (*gpi == NULL) 
                 display_error("System error: fixedgi malloc");
            }
      loadfixedgn = (int *) malloc(nteam * sizeof(int));
     if (loadfixedgn == NULL)
       display_error("System error: fixedgenome malloc");
     for (i=0, gi=loadfixedgn; i < nteam; i++, gi++) 
           *gi = 0;
         }

        // load fixed genotype from the file robot.gen
    if (loadfixedgenotypes == 1)
          loadfixedg();

    // initialize free parameters (to allow the use of test->individual before evolution)
    for(team=0; team<nteam; team++)
       getgenome(0,team);

        // load the parameters from the evorobot.net file, if present
        load_controller_parameters();

    // initialise statistics 
    for (g=0; g < MAXGENERATIONS; g++)
       for (gg=0; gg < 4; gg++)
          astatfit[g][gg] = 0.0;

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
 * Initialize the individuals' genotype
 * and call evaluate() 
 */
double
init_and_evaluate(int gen, int nind, int pop, double *varfit)
{
 
      int team;
      double totfitness;
      double vf;

       // initialize individuals' genotype
       for(team=0; team<nteam; team++)
       {
         getgenome(nind,team);
       }

       // evaluate
       totfitness = evaluate_epuck(&vf);
       *varfit = vf;

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
    
    
    
    char sroot[64];
    char sbuffer[64];
    FILE *fp;
    int p;
    int f;
    int x;
    int n;
    int k;
    float v;
    
    
    for(p = 0; p < npopulations; p++)
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
        fprintf(fp,"GENOTYPE %d %d\n",g,x);
        saveagenotype(fp,x,p);
        fflush(fp);
        fclose(fp);
        }
      }
    }
    // reproduce
    if (g+1 < generations)
    {
      for(p=0; p < npopulations; p++)
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
    
    
    
    char sroot[64];
    char sbuffer[64];
    FILE *fp;
    int p;
    int f;
    int x;
    int n;
    int k;
    float v;
    
    
    for(p = 0; p < npopulations; p++)
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
         sprintf(sbuffer,"Best%d.gen",seed);
         if (g == 0)
         {
           if ((fp=fopen(sbuffer, "w")) == NULL)
           {
             sprintf(sroot,"I cannot open file Best%d.gen",seed);
             display_error(sroot);
           }
         }
         else
         {
          if ((fp=fopen(sbuffer, "a")) == NULL)
          {
            sprintf(sroot,"I cannot open file Best%d.gen",f+1,p ,seed);
            display_error(sroot);
          }
        }
            fprintf(fp,"GENOTYPE %d %d\n",g,x);
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
      for(p=0; p < npopulations; p++)
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
 * stocastic hill climber
 * generations & g = numero variazioni da testare
 * il genotipo corrente e' contenuto in individuo n.0 
 * e nel bestgenome 0.
 */
void
adapt()
{

       int    g;
       double bestfit,avefit;
       char   sbuffer[256];
       int    *gi;
       FILE   *fp, *fpb;
       double varfit;
       int    g_percmut;
       double tcur_fit;        // total fitness of current solution
       int    ncur_fit;        // number of times the current solution has been evaluated
       double var_fit;         // fitness of the current variation
       int    n;               // number of retained variations
       int    nn;              // number of time current solution has been retained
       double stderror;        // standard error current variation

       if (adaptcurrent == 1)
        {
         gi = *(genome + 0);
         phenotype_to_genotype(0, gi, 8, homogeneous);
        }
       statfit_max = 0;
       statfit_min = 0;
       statfit_n = 0;
       n = 0;
       nn = 0;
       g_percmut = percmut;
       percmut = 100;
       tcur_fit = init_and_evaluate(0,0,0, &varfit);
       ncur_fit = 1;
       putbestgenome(0,0,0);
       statfit[n][0] = tcur_fit;
       stderror = varfit / sqrt((double) amoebaepochs);
       statfit[n][1] = stderror;
       statfit_n++;
       n++;
       statfit_max = tcur_fit;
       if (tcur_fit < 0)
         statfit_min = tcur_fit;

       sprintf(sbuffer,"statS%d.fit",seed);
       fp=fopen(sbuffer , "w");
       if (fp == NULL)
         display_error("unable to save individual fitnesses on a file");
       fprintf(fp,"%.2f\n",tcur_fit);

       sprintf(sbuffer,"Best%d.gen",seed);
       if ((fpb=fopen(sbuffer, "w")) == NULL)
        {
         sprintf(sbuffer,"I cannot open file Best%d.gen", seed);
         display_error(sbuffer);
        }
       fprintf(fpb,"GENOTYPE %d %d\n",n,0);
       saveagenotype(fpb,0,0);

       for(g=startgeneration;g<generations;g++)
       {
        // we reduce the percentage of variations over time up to g_percmut
        if ((g % 10) == 0 && percmut > g_percmut)
           percmut--;
        getbestgenome(0,0,0,1);
        // we display network architecture to show how parameters change during learning
        evolution_is_running = 0;
        update_rendnetwork(0);
        evolution_is_running = 1;

        var_fit = init_and_evaluate(0,0,0, &varfit);
        if (var_fit >= (tcur_fit / (double) ncur_fit))
          {
           putbestgenome(0,0,0);
           tcur_fit = var_fit;
           ncur_fit = 1;
           n++;
           nn = 0;
           fprintf(fp,"%.2f\n",tcur_fit);
           statfit[n][0] = tcur_fit;
           stderror = varfit / sqrt((double) amoebaepochs);
           statfit[n][1] = stderror;
           statfit_n++;
           if (tcur_fit > statfit_max)
             statfit_max = tcur_fit;
           if (statfit_min > tcur_fit)
             statfit_min = tcur_fit;
           update_rendevolution(1);
           fprintf(fpb,"GENOTYPE %d %d\n",g,n);
           saveagenotype(fpb,0,0);
          }
         else
          {
           nn++;
           // every ten maladaptive changes we re-evaluate current solution
           if ((nn % 10) == 0)
            {
             getbestgenome(0,0,0,0);       
             tcur_fit += init_and_evaluate(0,0,0,&varfit);
             ncur_fit++;
             statfit[n-1][0] = tcur_fit / (double) ncur_fit;
             update_rendevolution(1);
           }
          }
         sprintf(sbuffer, "Seed %d Mut %d Variation %d  Succ. variation %d Fit-B.%.3f (%d) Fit-A.%.3f st.err %.3f",seed,percmut,g,n,tcur_fit / (double) ncur_fit, ncur_fit, var_fit, stderror);
         display_stat_message(sbuffer);
         if (userbreak == 1)
          {
           evolution_is_running = 0;
           break;
          }
       }
       fclose(fp);
       fclose(fpb);
       percmut = g_percmut;
} 

/*
 * main adaptation loop (calling eval_team and loop)
 * also used for testing all individuals
 */
void
evolution_truncation()
{

       int    n,g,p,nn,pp;
       double bestfit,avefit;
       char   sbuffer[128];
       FILE   *fp;
       double varfit;

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
        init_and_evaluate(g,n,p,&varfit);
            if (elitism == 2)
        {
         plifes[n] += 1;
         pfitness[n] += tfitness[p][n];
        }

        sprintf(sbuffer, "Seed %d Pop %d Gen.%d  Ind.%d  Fit.%.3f",seed,p,g,n,tfitness[p][n]);
        display_stat_message(sbuffer);

        if (userbreak == 1)
        {
            evolution_is_running = 0;
            break;
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

           sprintf(sbuffer, "gen %d) %.2f %.2f",g,bestfit,avefit);
           display_stat_message(sbuffer);

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
 * steady_state evolutionary algorithms
 * we evaluate all individuals once
 * than we allow to each individual to make a child and to replace the worst
 * performing individual with the child (if the child outpeforms it) 
 */
void
evolution_steady()
{

       int    n,g,p,nn,pp;
       double bestfit,avefit;
       char   sbuffer[128];
       FILE   *fp;
       double varfit;
       double ifitness;
	   int    idbest;
       double fworse;
       int    iworse;
       int    gpercmut;

       statfit_max = 0;
       statfit_min = 0;
       statfit_n = 0;
       for(n=0;n<nindividuals;n++)
        {
         pfitness[n] = 0.0;
         plifes[n] = 0;
        }
       if (masterturnament == 1)
         nindividuals = nindividuals + additional_ind;

       gpercmut = percmut;
       percmut = 100;
       for(g=startgeneration;g<generations;g++)
       {
           for(n=0;n<nindividuals;n++)
           {
            // we evaluate individual n
            ifitness = init_and_evaluate(g,n,p,&varfit);
            evolution_is_running = 0;
            update_rendnetwork(0);
            evolution_is_running = 1;
            plifes[n] += 1;
            pfitness[n] += ifitness;
            sprintf(sbuffer, "Seed.%d Gen.%d Mut.%d Ind.%d Fit.%.3f N.Eval.%d",seed,g,percmut, n,pfitness[n] / (double) plifes[n], plifes[n]);
            display_stat_message(sbuffer);
            if (userbreak == 1)
             {
             evolution_is_running = 0;
             break;
             }
            // we generate and evaluate the child of individual n
            if (g > startgeneration)
            {
            copyandmutategenome(n,nindividuals);
            ifitness = init_and_evaluate(g,nindividuals,p,&varfit);
            evolution_is_running = 0;
            update_rendnetwork(0);
            evolution_is_running = 1;
            // we look for the worst individual of the population
            for(nn=0,iworse=0, fworse=+99999999.9; nn < nindividuals;nn++)
             {
              if ((pfitness[nn] / (double) plifes[nn]) < fworse)
              {
               fworse = (pfitness[nn] / (double) plifes[nn]);
               iworse = nn;
              }
             }
            // eventually we replace the worst individual with the new variation
            if (ifitness >= fworse)
             {
              copygenome(nindividuals,iworse);
              pfitness[iworse] = ifitness;
              plifes[iworse] = 1;
              sprintf(sbuffer, "Seed.%d Gen.%d Mut.%d Repl.%d->%d Fit.%.3f N.Eval.%d",seed,g,percmut,n,iworse, ifitness, 1);
              display_stat_message(sbuffer);
             }
            else
             {
              sprintf(sbuffer, "Seed %d Gen.%d Mut.%d Disc.%d Fit.%.3f N.Eval.%d",seed,g,percmut,n,ifitness, 1);
              display_stat_message(sbuffer);
             }
           }
           }
         // we now compute and display, save statistics
         sprintf(sbuffer,"statS%d.fit",seed);
         if (g == 0)
           fp=fopen(sbuffer , "w");
          else
           fp=fopen(sbuffer , "a");
         if (fp == NULL)
          display_error("unable to save statistics on a file");
         
         for(n = 0, bestfit = -9999990.0, idbest=0, avefit  = 0.0;n < nindividuals;n++)    
            {
             ifitness = pfitness[n] / (double) plifes[n];
             avefit += ifitness;
             if (ifitness > bestfit)
			  {
                bestfit = ifitness;
				idbest = n;
			  }
            }
         avefit = avefit / (double) nindividuals;
         sprintf(sbuffer, "gen %d) %.2f %.2f",g,bestfit,avefit);
         display_stat_message(sbuffer);
         if (userbreak == 0)
            {
              fprintf(fp,"%.3f %.3f \n",bestfit,avefit);           
              if (g < MAXGENERATIONS)
                {
                  statfit[g][0] = bestfit;
                  statfit[g][1] = avefit;
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

        fflush(fp);
        fclose(fp);

		// we save the best genotype
        savebestgenotype(seed, g, idbest);
          
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
       if (percmut > 25)
         percmut--; 
       if (percmut > gpercmut)
         percmut--;
      } // end generations
     percmut = gpercmut;

}

/*
 * test one individual
 */

void test_one_individual()
{

  double varfit;

  init_and_evaluate(startgeneration, dindividual,0, &varfit);

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
  double varfit; 

  for (replication=0; replication < nreplications; replication++)
    {
      statfit_max = 0;
      statfit_min = 0;
      sprintf(sbuffer,"Best%d.gen",seed + replication);
      nloadedindividuals = loadallg(-1, sbuffer);
      if (nloadedindividuals > (nindividuals + additional_ind))
      {
        nloadedindividuals = nindividuals + additional_ind;
        display_warning("please increase the number of additional_ind to allocate enough space");
      }
      for(n=0;n<nloadedindividuals;n++)
        {
        sprintf(sbuffer,"testing replication %d (%d individuals) individual %d",seed + replication,nloadedindividuals,n);
        display_stat_message(sbuffer);
        init_and_evaluate(0,n,0,&varfit);
        if (userbreak == 1)
            break;
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
       sprintf(sbuffer,"Best%d.fit",seed + replication);
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
 * test individual
 *  mode=0: test the current individual team
 *  mode=1; test the best individial team of all generations (on the basis of the current statistics, i.e. without loading the .fit file)
 *  mode=2; test all individuals of the population
 */
void test(int mode)
{

  double varfit;

  switch (mode) 
   {
     case 0:
      testindividual_is_running = 1;
      evolution_is_running = 0;
      if (genoloaded > 0) 
       {
        init_and_evaluate(startgeneration, dindividual,0,&varfit);
        genoloaded = 0;
       }
      else
       {
        evaluate_epuck(&varfit);
       }
      testindividual_is_running = 0;
      userbreak = 0;     
      break;
     case 1:
      testindividual_is_running = 1;
      evolution_is_running = 0;
      select_besti();
      init_and_evaluate(startgeneration, dindividual,0,&varfit);
      testindividual_is_running = 0;
      userbreak = 0;    
      break;
     case 2:
      evolution_is_running=1;
      testindividual_is_running = 0;
      test_all_individuals();
      evolution_is_running=0;
      break;
  }
}



/*
 * randomize the genomes of the entire population
 */
void
randomize_pop()

{

    int     i;
    int     j;
    int     p;
    int    **gpi;
    int    *gi;
    
    for (p = 0, gpi=genome; p < npopulations; p++)
    { 
      for (i=0; i < (nindividuals + additional_ind); i++,gpi++) 
      {
                 for (j=0,gi = *gpi; j < ngenes; j++,gi++)
            {
             *gi = mrand(256);
            }
      }
    }
}

/*
 * randomize the genome and the parameters of the current individual/team
 */
void randomizefreep()
{

 int team;

 randomize_pop();
 for(team=0; team<nteam; team++)
   getgenome(0,team);
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

  evolution_is_running=1;
  testindividual_is_running=0;

  oldseed = seed;
  for (replication=0; replication < nreplications; replication++)
    {
      set_seed(seed);
      randomize_pop();
      sprintf(sbuffer,"statS%d.fit",seed);
      if ((fp=fopen(sbuffer, "r")) == NULL) 
        {
          if (evoalgorithm == 0)
              evolution_truncation();
			else
			  evolution_steady();
        }
       else
        {
          fclose(fp);
          ng = loadstatistics(sbuffer, 1);
          if (ng < generations)
           {
            startgeneration = ng;
            loadallg(startgeneration - 1, "nofile");
            if (evoalgorithm == 0)
              evolution_truncation();
			else
			  evolution_steady();
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

/*
 * run n replications of the adaptive process
 */
void runadapt()
{

  int    replication;
  int    g,gg;
  int    ng;
  int    oldseed;
  FILE   *fp;
  char   sbuffer[128];

  evolution_is_running=1;
  testindividual_is_running=0;

  oldseed = seed;
  for (replication=0; replication < nreplications; replication++)
    {
      set_seed(seed);
      randomize_pop();
      sprintf(sbuffer,"statS%d.fit",seed);
      if (((fp=fopen(sbuffer, "r")) == NULL) || adaptcurrent == 1)
        {
          adapt();
        }
       else
        {
          fclose(fp);
          ng = loadstatistics(sbuffer, 1);
          if (ng < generations)
           {
            startgeneration = ng;
            loadallg(startgeneration - 1, "nofile");
            adapt();
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
getgenome(int n, int t)

{
    
    int     j;
    int    *gi;
    int     fixedi;
    float   *w;
    float   range;
    int     numc;


    if (loadfixedgenotypes == 1 && *(loadfixedgn + t) == 1)
      {
        gi = *(fixedgenome + t);
      }
    else
      {
        gi = *(genome + n);
      }

    genotype_to_phenotype(t, gi, 8, homogeneous);

}


/*
 * save the net n of population p on the fp file 
 */ 
void
saveagenotype(FILE *fp, int n, int p)

{
    int    j;
    int *gi;

    gi = *(genome + (n + (p * (nindividuals + additional_ind))));
     
    for (j=0; j < ngenes; j++, gi++) 
      fprintf(fp, "%d\n", *gi);
    fprintf(fp, "END\n");
}

/*
 * save a genotype in the best file
 * it create the file when gen is 0 otherwise it append 
 */
void 
savebestgenotype(int seed, int g, int idbest)

{

  char filename[256];
  FILE *fp;

  sprintf(filename,"Best%d.gen",seed);
  if (g == 0)
    {
      if ((fp=fopen(filename, "w")) == NULL)
        {
          sprintf(filename,"I cannot open file Best%d.gen",seed);
          display_error(filename);
        }
    }
    else
    {
      if ((fp=fopen(filename, "a")) == NULL)
        {
            sprintf(filename,"I cannot open file Best%d.gen",seed);
            display_error(filename);
        }
     }
    fprintf(fp,"GENOTYPE %d %d\n",g, idbest);        
    saveagenotype(fp,idbest,0);
    fflush(fp);
    fclose(fp);
}

/*
 * load the genome of individual n of population p from the fp file
 */ 
void
loadagenotype(FILE *fp, int n, int p)
{

    int    *gi;
    int    j;

    gi = *(genome + (n + (p * (nindividuals + additional_ind))));

    for (j=0; j < ngenes; j++, gi++)
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

    FILE    *fp;
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
        fprintf(fp,"GENOTYPE %d %d\n",gen,n);        
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
    
    FILE    *fp;
    char    filename[1024];
    char    message[1024];
    char    flag[64];
    int     n;
    int     p;
    int     nloaded;
    int     max;
	int     v1;
	int     v2;
    
    for(p=0; p < 1; p++)
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
                  fscanf(fp, "%s %d %d\n", flag, &v1,&v2);
                  if (strcmp(flag,"GENOTYPE") == 0)
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
   genoloaded = nloaded;
   return(nloaded);
    
}


/*
 * load the genome of pre-evolved fixed individuals
 */ 
void
loadfixedg()

{
    
    FILE    *fp;
    int     n;
    char    flag[64];
    char    message[128];
    char    filename[128];
    int    *gi;
    int     nloaded;
    int     j;
	int     v1;
	int     v2;

    for(n=0, nloaded=0; n < nteam; n++)       
       {
          sprintf(filename, "evorobot%d.gen", n + 1);
          if ((fp=fopen(filename, "r")) != NULL) 
             {
          sprintf(flag,"");
          fscanf(fp, "%s %d %d\n", flag, &v1,&v2);
          if (strcmp(flag,"GENOTYPE") == 0)
            {
              gi = *(fixedgenome + n); 
              for (j=0; j < ngenes; j++, gi++)
                    {
                      fscanf(fp, "%d\n", gi);
                    }
              fscanf(fp, "END\n");
              *(loadfixedgn + n) = 1;
              nloaded++;
             }
          sprintf(message,"loaded %d fixed genome", nloaded);
          display_stat_message(message);
          fclose(fp);
         }
         }
    
}


/*
 * copy the bestgenome nn into genome n making mutations
 */
void
getbestgenome(int n, int nn, int p, int domutate)

{
    int     *gi;
    int     *bgi;    
        int     i;

        gi = *(genome + (n + (p * (nindividuals + additional_ind))));
        bgi = *(bestgenome + (nn + (p * bestfathers)));
    
    for (i=0; i < ngenes; i++, gi++, bgi++)
       if (domutate == 0)
              *gi = *bgi;
         else
              *gi = mutate(*bgi);


}



/*
 * copy a genome n into genome nn by applying mutation
 * differentiatial probability to select a null value
void
copyandmutategenome(int n, int nn)

{
    int     *gin;
    int     *ginn;    
    int     i;
    int     p;

    p = 0;
    gin = *(genome + n);
    ginn = *(genome + nn);
    
    for (i=0; i < ngenes; i++, gin++, ginn++)
	{
	  if (mrand(100) < percmut)
	   {
//	    if (pnullvalue == 0 || mrand(100) >= pnullvalue) 
//          *ginn = mrand(256);
//		else
//          *ginn = 127;
	    // only 25% probability to change from null to valie
	    // 50% probability to become null
	    if (*ginn == 127)
		{
		 if (mrand(100) < 25)
           *ginn = mrand(256);
		  else
           *ginn = 127;
		}
		else
		{
		 if (mrand(100) < 50)
           *ginn = mrand(256);
		  else
           *ginn = 127;
		}
	   }
	   else
	   {
		*ginn = *gin;
	   }
	 }
	
}
*/

/*
 * copy a genome n into genome nn
 */
void
copygenome(int n, int nn)

{
    int     *gin;
    int     *ginn;    
    int     i;
    int     p;

    p = 0;
    gin = *(genome + n);
    ginn = *(genome + nn);
    
    for (i=0; i < ngenes; i++, gin++, ginn++)
      *ginn = *gin;
}

/*
 * copy a genome n into genome nn by applying mutation
 */
void
copyandmutategenome(int n, int nn)

{
    int     *gin;
    int     *ginn;    
    int     i;
    int     p;

    p = 0;
    gin = *(genome + n);
    ginn = *(genome + nn);
    
    for (i=0; i < ngenes; i++, gin++, ginn++)
      *ginn = mutate(*gin);
}


/*
 * copy genome n in the bestgenome nn 
 */
void
putbestgenome(int n, int nn, int p)

{


    int    *gi;
    int    *bgi;    
    int     i;

    gi = *(genome + (n + (p * (nindividuals + additional_ind))));
    bgi = *(bestgenome + (nn + (p * bestfathers)));
    
    for (i=0;i < ngenes; i++, gi++, bgi++)
              *bgi = *gi;          

}


/*
 * load fitness statistics (best and average performance)
 * mode=1 display graph, mode=0 do not display the graph
 */ 
int
loadstatistics(char *files, int mode)

{


     FILE    *fp;
     char       line[128];
     char       word[128];
     double     bfit[MAXGENERATIONS];
     int n;
     int nc;
     int nn;

     statfit_n = 0;

     if (fitscale > 0.0)
      {
       statfit_max = fitscale;
       statfit_min = 0.0;
      }
     else
      {
       statfit_max = 0.0;
       statfit_min = 0.0;
      }

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
        if (fitscale < 0.0)
         {
         if (statfit[n][0] > statfit_max)
           statfit_max = statfit[n][0]; 
         if (statfit[n][1] > statfit_max)
           statfit_max = statfit[n][1]; 
         if (statfit[n][0] < statfit_min)
           statfit_min = statfit[n][0]; 
         if (statfit[n][1] < statfit_min)
           statfit_min = statfit[n][1]; 
         }
        n++;
      }
      statfit[n][0] = -9999.0;
      statfit_n = n;

      // moving average
      if (fitmovaverage > 1)
       {

       for(n=0; n < statfit_n; n++)
        {
         bfit[n] = statfit[n][0];
        }

       for(n=0; n < statfit_n; n++)
        {
          statfit[n][0] = bfit[n];
          if (n >= fitmovaverage)
           {

            for(nn=0; nn < (fitmovaverage-1); nn++)
              statfit[n][0] += bfit[n-nn];
            statfit[n][0] = statfit[n][0] / (double) fitmovaverage;
           }
        }
       }

      if (mode == 1)
      {
           sprintf(line,"loaded n: %d (%.1f %.1f %.1f) data from file %s",n, fitscale, statfit_min, statfit_max, files);
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
     statfit_n = n_all;
     if (fitscale < 0)
      {
       statfit_max = max_all;
       statfit_min = min_all;
      }
     else
      {
       statfit_max = fitscale;
       statfit_min = 0;
      }
     update_rendevolution(2);
     }
     else
     {
      update_rendevolution(1);
     }
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
     char    message[256];
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
             fscanf(fp, "GENOTYPE %d %d \n",&v1,&v2);
             loadagenotype(fp,gg,0);
        }
        sprintf(message,"loaded seed %d best %d generation %d",s,f,g);
        display_stat_message(message);
        fclose(fp);
     }
     else
     {
      sprintf(message,"file %s could not be opened\n",filename); 
      display_warning(message);
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
            fprintf(fp,"GENOTYPE %d %d\n",g,0);        
            saveagenotype(fp,g,0);
         } 
         fflush(fp);           
         fclose(fp);
         sprintf(message,"lineageS%d.gen have been saved",s);
         display_stat_message(message);
      }
      else
      {
        sprintf(message,"cannot open %s file",filename);
        display_warning(message);
      }
    }

}