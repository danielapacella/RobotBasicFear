/* 
 * Evorobot* - evolution.h
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

#define MAXGENERATIONS   10000        // max number of generations 
#define MAXINDIVIDUALS   1000        // max numer of individuals for each generation 

extern int     nreplications;    // n. of replications of the experiment 
extern int     seed;             // seed of the replication
extern double  statfit_min;      // minimum fitness
extern double  statfit_max;      // maximum fitness
extern int     statfit_n;        // n fitness data
extern double  statfit[MAXGENERATIONS][4]; /*  best and mean fit for each gen. & pop */
extern float   fitscale;         // display fitness, y-scale (< 0 means that it is computed automatically)

extern int winadaptationx;       // x coordinate of adaptation window (-1 = undefined)
extern int winadaptationy;       // y coordinate of adaptation window (-1 = undefined)
extern int winadaptationdx;      // dx of controller adaptation window (0 = undefined)
extern int winadaptationdy;      // dy of controller adaptation window (0 = undefined)


int     loadstatistics(char *filew, int mode);
int     loadallg(int gen, char *filew);
void    display_all_stat();
void    create_lineage();

void getgenome(int n, int t);
void saveagenotype(FILE *fp, int n, int p);
void savebestgenotype(int seed, int g, int idbest);
void getbestgenome(int n, int nn, int p, int domutate);
void putbestgenome(int n, int nn, int p);
void copyandmutategenome(int n, int nn);
void copygenome(int n, int nn);
int  loadallg(int gen, char *filew);
void saveallg(int gen);
void loadfixedg();
int  loadstatistics(char *filew, int mode);
double init_and_evaluate(int gen, int nind, int pop, double *varfit);
void randomize_pop();
