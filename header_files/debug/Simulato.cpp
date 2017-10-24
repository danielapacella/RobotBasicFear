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
 * simulato.c
 */

#include "defs.h"
#include "public.h"
/*
 * just set few costants
 */
void
SETP ()

{

  P90 =    asin(1.0);
  P180=2.0*asin(1.0);
  P360=4.0*asin(1.0);
}

/*
 * trasform radiants to degrees
 */
double
RADtoDGR(double r)

{
   double x;

   SETP();
   if (r < 0.0)
    x=360.0-(r*(-1.0)/P90)*90;
     else
    x=(r/P90)*90;
   return(x);
 }


/*
 * transform degrees to radiants
 */
double
DGRtoRAD(double d)

{
    double   x;

    x=(d/90)*asin(1.0);
    return(x);
}

/*
 * compute the deltax given an angle and a distance
 */
float
SetX(float x, float d, double angle)


{
    float   res;

    res =  (float) (x + d * cos(DGRtoRAD(angle)) + 0.000001);
    return(res);
}

/*
 * compute the deltay given an angle and a distance
 */
float
SetY(float y, float d, double angle)

{
    float   res;

    res = (float) (y + d * sin(DGRtoRAD(angle)) + 0.000001);
    return(res);

}


/*
 * return the distance between a point and a line
 */
double
linedist(float px,float py,float ax,float ay,float bx,float by)

{
     double d1,d2,d3;

     d1 = ((px-ax)*(px-ax)) + ((py-ay)*(py-ay));
     d2 = ((px-bx)*(px-bx)) + ((py-by)*(py-by));
     d3 = ((ax-bx)*(ax-bx)) + ((ay-by)*(ay-by));

     return(sqrt(d1 - (((d2 - d3 - d1) * (d2 - d3 - d1)) / (4 * d3))));

}


/*
 * return the distance between two points
 */

double
mdist(float x, float y, float x1, float y1)

{
   double qdist;

   qdist = ((x-x1)*(x-x1)) + ((y-y1)*(y-y1));
   return(sqrt(qdist));
}



/*
 * normalize and angle between 0 and 360
 */
double
normang(double a)

{

   int v;

   v = 0;
   while((a < 0.0 || a > 360.0) && v < 10)
    {
      if (a < 0.0)
	a += 360.0;
      if (a > 360.0)
	a -= 360.0;
      v++;
    }
   if (v == 10)
	display_warning("normang: angle out of range");
   return(a);
}


/*
 * return the angle between two points with a given distance
 */

double
mang(int x, int y, int x1, int y1, double dist)

 {

   double    angolo;
   int       dxang,dyang;
   double    buffer;
   double    alfa,pi2;
   double    temp;

   if  (x==x1 && y==y1)
      angolo=0.0;
     else
      {
       dxang=(x1-x);
       dyang=(y1-y);
       buffer=abs(dyang);
       pi2 =asin(1.0);
       temp = buffer/dist;
       if (temp>1.0) temp=1.0;
       alfa=asin(temp);
       alfa=(alfa/pi2)*90;
       if (dxang>0)
	     {
              if (dyang>0);
	      if (dyang<0)
                 alfa=360-alfa;
	     }
        if (dxang<0)
	     {
              if (dyang>0) alfa=180-alfa;
	      if (dyang<0) alfa=alfa+180;
	      if (dyang==0)alfa=180;
	      }
	 if (dxang==0)
	     {
		 if (dyang>0) ;
		 if (dyang<0) alfa=270;
		 if (dyang==0)alfa=0;
	     }
	  angolo=alfa;
       }
     return(angolo);
}




/*
 * it calculate the relative angle
 */
double
mangrel(double absolute,double kepera)



{

    double  relative;


    relative = absolute - kepera;

    if (relative > 359.0)
        relative -= 360.0;
    if (relative < 0.0)
        relative += 360.0;

    return(relative);

}



/*
 * it calculate the relative angle
 */
double
mangrel2(double absolute,double kepera)

{

    double  relative;


    relative = 360 - (kepera - absolute);

    if (relative > 359.0)
        relative -= 360.0;
    if (relative < 0.0)
        relative += 360.0;

    return(relative);

}

/*
 * return the right output value
 */

float
oselect(struct individual *pind, int out)
 
{


   return(pind->activation[pind->nneurons-pind->noutputs+out]);    


}


/*
 * initialize world variables
 */
void
init_world()


{

   int    i,ii,iii;
   int    w;

   for(w=0;w<5;w++)
   {
     env[w].dx = 500;
     env[w].dy = 500;
     env[w].nobstacles = 0;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 4; ii++)
          env[w].obstacles[i][ii]  = 0.0f;
     env[w].nroundobst = 0;
     env[w].radius = 0.0f;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 2; ii++)
          env[w].roundobst[i][ii]  = 0.0f;
     env[w].nsroundobst = 0;
     env[w].sradius = 0.0f;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 3; ii++)
          env[w].sroundobst[i][ii] = 0.0f;
     env[w].nfoodzones = 0;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 3; ii++)
          env[w].foodzones[i][ii]  = 0.0f;
     env[w].nbulblights = 0;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 2; ii++)
          env[w].bulblights[i][ii]  = 0.0f;
     env[w].nstarts = 0;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 20; ii++)
         for (iii=0; iii < 3; iii++)
           env[w].starts[i][ii][iii]  = 0.0f;
     env[w].nlandmarks = 0;
     for (i=0; i < 20; i++)
       for (ii=0; ii < 6; ii++)
          env[w].landmark[i][ii]  = 0.0f;
   }
}



/*
 * it load worlds description from file world?.env
 */
int
load_world()


{
   FILE *fp;
   int    i,ii;
   int    w;
   int    team;
   char   worldname[256];
   char   sbuffer[256];
   int    realnworlds;

   realnworlds = 0;
   for(w=0;w<nworlds;w++)
   {
     env[w].dx = 0;
     env[w].dy = 0;
     sprintf(worldname,"world%d.env",w+1);
     if ((fp = fopen(worldname,"r")) != NULL)
      {
	// walls
        fscanf(fp,"walls %d\n", &env[w].nobstacles);
        for (i=0; i < env[w].nobstacles; i++)
         {
          fscanf(fp,"%f %f %f %f\n",
            &env[w].obstacles[i][0],&env[w].obstacles[i][1],&env[w].obstacles[i][2],&env[w].obstacles[i][3]);
          if (env[w].obstacles[i][0] > env[w].dx)
            env[w].dx = (int) env[w].obstacles[i][0];
          if (env[w].obstacles[i][2] > env[w].dx)
            env[w].dx = (int) env[w].obstacles[i][2];
          if (env[w].obstacles[i][1] > env[w].dy)
            env[w].dy = (int) env[w].obstacles[i][1];
          if (env[w].obstacles[i][3] > env[w].dy)
            env[w].dy = (int) env[w].obstacles[i][3];
          }
	// round obstacles
        fscanf(fp,"rounds %d radius %f\n", &env[w].nroundobst, &env[w].radius);
        for (i=0; i < env[w].nroundobst; i++)
          fscanf(fp,"%f %f\n", &env[w].roundobst[i][0],&env[w].roundobst[i][1]);
  	// small round obstacles
        fscanf(fp,"srounds %d radius %f\n", &env[w].nsroundobst, &env[w].sradius);
        for (i=0; i < env[w].nsroundobst; i++)
          {
           fscanf(fp,"%f %f\n", &env[w].sroundobst[i][0],&env[w].sroundobst[i][1]);
           env[w].sroundobst[i][2] = (float) 0.0;
          }
        // foodzones
        fscanf(fp,"foodzones %d\n", &env[w].nfoodzones);
	for (i=0; i < env[w].nfoodzones; i++)
          fscanf(fp,"%f %f %f\n",
             &env[w].foodzones[i][0],&env[w].foodzones[i][1],&env[w].foodzones[i][2]);
        // lights
        fscanf(fp,"lights %d\n", &env[w].nbulblights);
        for (i=0; i < env[w].nbulblights; i++)
          fscanf(fp,"%f %f\n", &env[w].bulblights[i][0],&env[w].bulblights[i][1]);
        // starting positions
        fscanf(fp,"starts %d\n", &env[w].nstarts);
        for (i=0; i < env[w].nstarts; i++)
         {
          for (team=0; team < nteam; team++)
	  {
           for (ii=0; ii < 3; ii++)
            fscanf(fp,"%f", &env[w].starts[i][team][ii]);
            fscanf(fp,"\n");
          }
         }
        // landmarks
        fscanf(fp,"landmarks %d\n", &env[w].nlandmarks);
        for (i=0; i < env[w].nlandmarks; i++)
         {
            fscanf(fp,"%f %f %f %f %f", &env[w].landmark[i][0],&env[w].landmark[i][1],&env[w].landmark[i][2],&env[w].landmark[i][3],&env[w].landmark[i][4]);
            fscanf(fp,"\n");
         }
        fscanf(fp, "%s",sbuffer);
        if (strcmp(sbuffer,"END") != 0)
	   display_warning("unable to find END in the world's file");

        fclose(fp);
	realnworlds++;
       }
      else
       {
	return(-1);
       }
  }

   nworlds = realnworlds;
   if (nworlds == 1)
     sprintf(sbuffer,"loaded file world1.env");
    else
     sprintf(sbuffer,"loaded n. %d world[n].env files", nworlds);

   display_stat_message(sbuffer);

   return(1);
}


/*
 * it save worlds characteristics in one or more files
 *
 */
void
save_world()


{

   FILE	  *fp;
   int    i,ii;
   int    w;
   int    team;
   char   sbuffer[256];
   char   worldname[256];


   for(w=0;w<nworlds;w++)
   {
     sprintf(worldname,"world%d.env",w+1);
     if ((fp = fopen(worldname,"w")) != NULL)
      {
	// walls
        fprintf(fp,"walls %d\n", env[w].nobstacles);
        for (i=0; i < env[w].nobstacles; i++)
         {
          fprintf(fp,"%.1f %.1f %.1f %.1f\n",
            env[w].obstacles[i][0],env[w].obstacles[i][1],env[w].obstacles[i][2],env[w].obstacles[i][3]);
          }
	// round obstacles
        fprintf(fp,"rounds %d radius %.1f\n", env[w].nroundobst, env[w].radius);
        for (i=0; i < env[w].nroundobst; i++)
          fprintf(fp,"%.1f %.1f\n", env[w].roundobst[i][0],env[w].roundobst[i][1]);
  	// small round obstacles
        fprintf(fp,"srounds %d radius %.1f\n", env[w].nsroundobst, env[w].sradius);
        for (i=0; i < env[w].nsroundobst; i++)
          {
           fprintf(fp,"%.1f %.1f\n", env[w].sroundobst[i][0],env[w].sroundobst[i][1]);
          }
        // foodzones
        fprintf(fp,"foodzones %d\n", env[w].nfoodzones);
	for (i=0; i < env[w].nfoodzones; i++)
          fprintf(fp,"%.1f %.1f %.1f\n",
             env[w].foodzones[i][0],env[w].foodzones[i][1],env[w].foodzones[i][2]);
        // lights
        fprintf(fp,"lights %d\n", env[w].nbulblights);
        for (i=0; i < env[w].nbulblights; i++)
          fprintf(fp,"%.1f %.1f\n", env[w].bulblights[i][0],env[w].bulblights[i][1]);
        // starting positions
        fprintf(fp,"starts %d\n", env[w].nstarts);
        for (i=0; i < env[w].nstarts; i++)
         {
	  for(team=0; team < nteam; team++)
	  {
           for (ii=0; ii < 3; ii++)
            fprintf(fp,"%.1f ", env[w].starts[i][team][ii]);
            fprintf(fp,"\n");
	  }
         }
        // landmarks
        fprintf(fp,"landmarks %d\n", env[w].nlandmarks);
        for (i=0; i < env[w].nlandmarks; i++)
         {
            fprintf(fp,"%.1f %.1f %.1f %.1f %.1f", env[w].landmark[i][0],env[w].landmark[i][1],env[w].landmark[i][2],env[w].landmark[i][3],env[w].landmark[i][4]);
            fprintf(fp,"\n");
         }
        fprintf(fp, "END");
        fclose(fp);

        if (nworlds == 1)
          sprintf(sbuffer,"save world data in file world1.env");
         else
          sprintf(sbuffer,"save world data in n.%d world[n].env files", nworlds);
	 display_stat_message(sbuffer);
       }
      else
       {
	  sprintf(sbuffer,"unable to save the environment %s",worldname);
	  display_warning(sbuffer);
       }
  }
}



/*
 * load an obstacle from a .sam file
 */
void
load_obstacle(char *filename, int **object, int  *objectcf)

{

   int  **ob;
   int  *o;
   FILE *fp;
   int  i,v,s;
   char sbuffer[128];

   if ((fp = fopen(filename,"r")) != NULL)
	  {
	  }
	  else
	  {
	    sprintf(sbuffer,"I cannot open file %s", filename);
	    display_error(sbuffer);
	  }
   /*
    * we read the object configuration data
    * 0-nsensors, 1-nangles, 2-ndistances,3-initdist,4-distinterval
    */
   for (i=0, o=objectcf; i < 5; i++, o++)
     fscanf(fp,"%d ",o);
   fscanf(fp,"\n",o);

   for (v=0, ob=object; v < objectcf[2]; v++, ob++)
    {
     fscanf(fp,"TURN %d\n",&v);
     for (i = 0, o = *ob; i < objectcf[1]; i++)
      {
       for (s=0; s < objectcf[0]; s++,o++)
        {
	   fscanf(fp,"%d ",o);
        }
       fscanf(fp,"\n");
      }
    }
   fscanf(fp, "%s\n",sbuffer);
   if (strcmp(sbuffer,"END") != 0)
     {
      sprintf(sbuffer,"loading file %s: file might be inconsistent", filename);
      display_error(sbuffer);
     }
   fflush(fp);
   fclose(fp);
}


/*
 * save an obstacle in a .sam file
 */
void
save_obstacle(char *filename, int **object, int  *objectcf)

{

   int  **ob;
   int  *o;
   FILE *fp;
   int  i,v,s;
   char sbuffer[64];

   if ((fp = fopen(filename,"w")) != NULL)
	  {
	  }
	  else
	  {
	    sprintf(sbuffer,"I cannot open file %s", filename);
	    display_error(sbuffer);
	  }
   /*
    * we write the object configuration data
    * 0-nsensors, 1-nangles, 2-ndistances,3-initdist,4-distinterval
    */
   for (i=0, o=objectcf; i < 5; i++, o++)
     fprintf(fp,"%d ",*o);
   fprintf(fp,"\n");

   for (v=0, ob=object; v < objectcf[2]; v++, ob++)
    {
     fprintf(fp,"TURN %d\n",v);
     for (i = 0, o = *ob; i < objectcf[1]; i++)
      {
       for (s=0; s < objectcf[0]; s++,o++)
        {
	   fprintf(fp,"%d ",*o);
        }
       fprintf(fp,"\n");
      }
    }
   fprintf(fp, "END\n");
   fflush(fp);
   fclose(fp);
}


/*
 * load motor samples
 * the first line should be the number of samples
 * for each sample we compute 3 simmitrical
 */
void
load_motor()

{

	extern  float **motor;
	float  **mo;
	float  *m;
	FILE   *fp;
	int    v,vv;
#ifdef EVOWINDOWS
	if ((fp = fopen("../bin/sample_files/motor.sam","r")) != NULL)
#else
	if ((fp = fopen("..\bin\sample_files\motor.sam","r")) != NULL)

#endif
  {
  }
	else
	{
		display_error("cannot open file motor.sam");
	}
	for (v=0,mo = motor; v < 25; v++,mo++)
	{
		for (vv=0,m = *mo; vv < 25; vv++,m++)
		{
			fscanf(fp,"%f\t",m);
			m++;
		}
		fscanf(fp,"\n");
	}
	for (v=0,mo = motor; v < 25; v++,mo++)
		{
			for (vv=0,m = *mo; vv < 25; vv++,m++)
			{
				m++;
				fscanf(fp,"%f\t",m);
			}
			fscanf(fp,"\n");
		}
		fclose(fp);
	}



/*
 * set sensor activations (i.e. it update pind->input[])
 */
void
set_input(struct individual *pind, struct  iparameters *pipar)

{

	float  p1;
	float  p2;
	double direction;
	int    i,ii;
	int    f;
	float  fr;
	int    g;
	int    s;
	int    team;
	float  x,y;
	double dist,dire,headx,heady;
	double  angle;
	int     relang;
	int     reldist;
	int     nsensor;
	int     *w;
	int     *o;
	double  ang;
	struct individual *cind;
	double max_dist_l;
	int    most_dist_l;
	float  land_first[2];
	float  land_second[2];
	double dist_first;
	double dist_second;
	double ang_first;
	double ang_second;
	double delta_ang1;
	double delta_ang2;
	double ang_d;
	int    t;
	float  IRSensor[4], max;
	int    index;
	float  old_max_irs[4][2];
	int    ostacoli;
	float  arb_max;
	int    arb_ind;
	tPointi ostacoli_1, ostacoli_2;
	tPointi a, b;
	tPointd p;

	p1        = pind->pos[0];
	p2        = pind->pos[1];
	direction = pind->direction;

	a[0] = p1;
	a[1] = p2;

	nsensor = 0;

	for(i=0;i<8;i++)
	{
	  pind->cifsensors[i]    = 0.0f;
	  pind->clightsensors[i] = 0.0f;
	}

	// **********************************
	// motor states at time t-1
	if (pipar->motmemory > 0)
	{
	  pind->input[nsensor]   = oselect(pind, 0);
	  pind->input[nsensor+1] = oselect(pind, 1);
	  nsensor += 2;
	}

	// **********************************
	// infrared sensors
	// from walls
	for(i=0;i < env[cworld].nobstacles;i++)
	{
	  if (p1 >= env[cworld].obstacles[i][0] && p1 <= env[cworld].obstacles[i][2])
	     x = p1;
	   else
	     x = env[cworld].obstacles[i][0];
	  if (p2 >= env[cworld].obstacles[i][1] && p2 <= env[cworld].obstacles[i][3])
	     y = p2;
	   else
	     y = env[cworld].obstacles[i][1];
	   dist = mdist((float)p1,(float)p2,(float)x,(float)y);
	   if (dist < (wallcf[3] + ROBOTRADIUS + (wallcf[2] * wallcf[4])))     // min.dist + khepera rad. + max. dist
		{                     // 5 + ROBOTRADIUS + 40 = 70.5
		  if (x == p1)
		  {
		    if (y < p2)
			angle = 270.0;
		       else
			angle = 90.0;
		  }
		  else
		  {
		   if (x < p1)
		      angle = 180.0;
			else
		      angle = 0.0;
		  }
		  relang  = (int) (mangrel(angle,direction) / 2.0);
		  dist -= ROBOTRADIUS;          // khepera rad. = ROBOTRADIUS
		  if (dist < wallcf[3])        // min. dist. = 5.0
		    dist = wallcf[3];
		  reldist = (int) (dist - wallcf[3]);  /* 5 for addressing */
		  reldist = (int) reldist / wallcf[4]; /* for addressing each 2 millimeters */

		  w = *(wall + reldist);
		  w = (w + (relang * 8));
		  for(ii=0;ii<8;ii++,w++)
		   pind->cifsensors[ii] += (*w / (float) 1024.0);
		}
	}

	// from round obstacles
	for(i=0;i< env[cworld].nroundobst;i++)
	{
		dist = mdist((float)p1,(float)p2,(float)env[cworld].roundobst[i][0],(float)env[cworld].roundobst[i][1]) - env[cworld].radius;
		ang = mang((int)p1,(int)p2,(int)env[cworld].roundobst[i][0],(int)env[cworld].roundobst[i][1],(double)dist+env[cworld].radius);
		relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
		if (dist < (obstcf[3] + ROBOTRADIUS + (obstcf[2] * obstcf[4])))       // min.dist + khepera rad. + (dists*inter)
		{                     // 72.5 = 5 + ROBOTRADIUS + 40 = 72.5
			ang = mang((int)p1,(int)p2,(int)env[cworld].roundobst[i][0],(int)env[cworld].roundobst[i][1],(double)dist+env[cworld].radius);
			relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
			dist -= ROBOTRADIUS;          // khepera rad.
			if (dist < obstcf[3])  // min. dist. = 5.0
				dist = obstcf[3];
			reldist = (int) (dist - obstcf[3]);  // 5 for addressing
			reldist = (int) reldist / obstcf[4]; // for addressing each 2 millimeters

			o = *(obst + reldist);
			o = (o + (relang * 8));
			for(ii=0;ii<8;ii++,o++)
				pind->cifsensors[ii] += (*o / (float) 1024.0);
		}
	}


	// from small round obstacles
	for(i=0;i<env[cworld].nsroundobst;i++)
	{
		if (env[cworld].sroundobst[i][2] == 0.0)
		{
			dist = mdist((float)p1,(float)p2,(float)env[cworld].sroundobst[i][0],(float)env[cworld].sroundobst[i][1]) - env[cworld].sradius;
			if (dist < (sobstcf[3] + ROBOTRADIUS + (sobstcf[2] * sobstcf[4])))       // min.dist + khepera rad. + max. dist
			{                     // 5 + ROBOTRADIUS + 40 = 72.5

				ang = mang((int)p1,(int)p2,(int)env[cworld].sroundobst[i][0],(int)env[cworld].sroundobst[i][1],(double)dist+env[cworld].sradius);
				relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
				dist -= ROBOTRADIUS;          // khepera rad. = ROBOTRADIUS
				if (dist < sobstcf[3])        // min. dist. = 5.0
					dist = sobstcf[3];
				reldist = (int) (dist - sobstcf[3]);
				reldist = (int) reldist / (int) sobstcf[4];

				o = *(sobst + reldist);
				o = (o + (relang * 8));
				for(ii=0;ii<8;ii++,o++)
					pind->cifsensors[ii] += (*o / (float) 1024.0);
			}
		}
	}

	// from other robots
	// we used the sample of round obstacles assuming they are similar to robots
	for (t=0,cind=ind; t<nteam; t++, cind++)
	{
	 if ((p1 != cind->pos[0] || p2 != cind->pos[1]))
	 {
	   dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]) - ROBOTRADIUS;
	   if (dist < (obstcf[3] + ROBOTRADIUS + (obstcf[2] * obstcf[4])))
		{
		  ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist+ROBOTRADIUS);
		  relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
		  dist -= ROBOTRADIUS;
		  if (dist < obstcf[3])
		     dist = obstcf[3];
		  reldist = (int) (dist - obstcf[3]);
		  reldist = (int) reldist / (int) obstcf[4];

		  o = *(obst + reldist);
		  o = (o + (relang * 8));
		  for(ii=0;ii<8;ii++,o++)
		    pind->cifsensors[ii] += (*o / (float) 1024.0);
		}
	 }
	}

	// value over limits are truncated
	for(ii = 0; ii < 8; ii++)
	{
	  if (pind->cifsensors[ii] > 1.0)
	   pind->cifsensors[ii] = 1.0f;
	}

	// add noise to infra-red sensors if requested
	if (sensornoise > 0.0)
	  for(i=0; i < 8; i++)
	  {
	   pind->cifsensors[i] += rans(sensornoise);
	   if (pind->cifsensors[i] < 0.0) pind->cifsensors[i] = (float) 0.0;
	   if (pind->cifsensors[i] > 1.0) pind->cifsensors[i] = (float) 1.0;
          }

	// we copy 4, 6, or 8 if sensors on the input vector
	if (pipar->nifsensors == 4)
	{
	  pind->input[nsensor]   = (pind->cifsensors[0] + pind->cifsensors[1]) / 2.0f;
	  pind->input[nsensor+1] = (pind->cifsensors[2] + pind->cifsensors[3]) / 2.0f;
	  pind->input[nsensor+2] = (pind->cifsensors[4] + pind->cifsensors[5]) / 2.0f;
	  pind->input[nsensor+3] = (pind->cifsensors[6] + pind->cifsensors[7]) / 2.0f;
	  nsensor += 4;
	}
	if (pipar->nifsensors == 6)
	{
	  for(ii=0;ii<6;ii++,nsensor++)
	   pind->input[nsensor] = pind->cifsensors[ii];
	}
	if (pipar->nifsensors == 8)
	{
	  for(ii=0;ii<8;ii++,nsensor++)
	   pind->input[nsensor] = pind->cifsensors[ii];
	}

	// ***************************************
	// light sensors
	if (pipar->lightsensors > 0)
	{
	   for(i=0;i< env[cworld].nbulblights;i++)
	   {
	      dist = mdist((float)p1,(float)p2,(float) env[cworld].bulblights[i][0],(float)env[cworld].bulblights[i][1]);
	      ang = mang((int)p1,(int)p2,(int)env[cworld].bulblights[i][0],(int)env[cworld].bulblights[i][1],(double)dist);
	      relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
	      if (dist <= (lightcf[3] + 27 + (lightcf[2] * lightcf[4])))    // 560 = 13 + 27 + 520 (26*20)
		{
		   dist  -= 27;                         // khepera rad
		   if (dist < lightcf[3])
		     dist = lightcf[3];
		   reldist = (int) (dist - lightcf[3]); // 13 for adressing
		   reldist = (int) (dist / lightcf[4]); // for addressing each 20mm       o = *(light + reldist);
		   o = *(light + reldist);
		   o = (o + (relang * 8));
		   for(ii=0;ii<8;ii++,o++)
		     pind->clightsensors[ii] += (*o / (float) 1024.0);
		}
	   }
	  // we normalize between limits
	  for(ii = 0; ii < 8; ii++)
	  {
	     if (pind->clightsensors[ii] > 1.0)
	       pind->clightsensors[ii] = 1.0f;
	  }
	  // noise
	  if (sensornoise > 0.0)
	    for(i=0; i < 8; i++)
	    {
	      pind->clightsensors[i] += rans(sensornoise);
	      if (pind->clightsensors[i] < 0.0) pind->clightsensors[i] = (float) 0.0;
	      if (pind->clightsensors[i] > 1.0) pind->clightsensors[i] = (float) 1.0;
	    }
	  // we copy 2, 3 or 8 sesors on the input vector
	  if (pipar->lightsensors == 2)
	  {
	     pind->input[nsensor] = pind->clightsensors[2];
	     nsensor++;
	     pind->input[nsensor] = pind->clightsensors[6];
	     nsensor++;
	  }
	  if (pipar->lightsensors == 3)
	  {
	     pind->input[nsensor] = (pind->clightsensors[0] + pind->clightsensors[1]) / 2.0f;
	     nsensor++;
	     pind->input[nsensor] = (pind->clightsensors[2] + pind->clightsensors[3]) / 2.0f;
	     nsensor++;
	     pind->input[nsensor] = (pind->clightsensors[4] + pind->clightsensors[5]) / 2.0f;
	     nsensor++;
	  }
	  if (pipar->lightsensors == 8)
	  {
	     for(ii=0;ii<8;ii++,nsensor++)
		pind->input[nsensor] = pind->clightsensors[ii];
	  }
	}

	//***************************************
        // signals from nearby robots
	if (pipar->signalss > 0)
	{
	   for(i=0;i< pipar->signalss;i++)
	   {
	     pind->detected_signal[i][0] = 0.0f;
	     pind->detected_signal[i][1] = 999999.0f;
	   }
	   for (t=0, cind=ind; t<nteam; t++, cind++)
	     if (pind->n_team != cind->n_team)
		{
	           dist = mdist((float)pind->pos[0],(float)pind->pos[1],(float) cind->pos[0],(float)cind->pos[1]) - ROBOTRADIUS;
		   if (dist < (double) max_dist_attivazione)
		   {
		     if (pipar->signalss == 1)
		     // non-directional signals
		     {
		       if (dist < pind->detected_signal[0][1])
		        for(s=0;s < pipar->signalso; s++)
			{
                         pind->detected_signal[s][0] = oselect(cind,ipar->wheelmotors+s);
                         pind->detected_signal[s][1] = (float) dist;
			}
		     }
		     else
		     // directional signals
		     {
		       ang = mang((int)pind->pos[0],(int)pind->pos[1],(int)cind->pos[0],cind->pos[1],dist);
		       relang = (int) mangrel2((double)ang,(double)direction);
		       if (relang > 315 || relang <= 45)
		       {
		        if (dist < pind->detected_signal[0][1])
		         for(s=0;s < pipar->signalso; s++)
			 {
                          pind->detected_signal[s*pipar->signalss+0][0] = oselect(cind,ipar->wheelmotors+s);
                          pind->detected_signal[s*pipar->signalss+0][1] = (float) dist;
			 }
		       }
		       if (relang > 45 && relang <= 135)
		       {
		        if (dist < pind->detected_signal[1][1])
		         for(s=0;s < pipar->signalso; s++)
			 {
                          pind->detected_signal[s*pipar->signalss+1][0] = oselect(cind,ipar->wheelmotors+s);
                          pind->detected_signal[s*pipar->signalss+1][1] = (float) dist;
			 }
		       }
		       if (relang > 135 && relang <= 225)
		       {
		        if (dist < pind->detected_signal[2][1])
		         for(s=0;s < pipar->signalso; s++)
			 {
                          pind->detected_signal[s*pipar->signalss+2][0] = oselect(cind,ipar->wheelmotors+s);
                          pind->detected_signal[s*pipar->signalss+2][1] = (float) dist;
			 }
		       }
		       if (relang > 225 && relang <= 315)
		       {
		        if (dist < pind->detected_signal[3][1])
		         for(s=0;s < pipar->signalso; s++)
			 {
                          pind->detected_signal[s*pipar->signalss+3][0] = oselect(cind,ipar->wheelmotors+s);
                          pind->detected_signal[s*pipar->signalss+3][1] = dist;
			 }
		       }
		     }

		   }

		}
	     for(s=0;s < pipar->signalss*pipar->signalso; s++, nsensor++)
		{
                 pind->input[nsensor] = pind->detected_signal[s][0];
		 if (pipar->signalssb == 1)
                  if (pind->input[nsensor] > 0.5)
                    pind->input[nsensor] = 1.0;
		  else
                    pind->input[nsensor] = 0.0;
		 for (f=0,fr = 0.0;f < pipar->signalssf; f++, fr += (1.0 / (float) pipar->signalssf))
		   if (pind->input[nsensor] >= fr && pind->input[nsensor] < (fr + (1.0 / (float) pipar->signalssf)))
                     pind->input[nsensor+1+f] = 1.0;
		   else
                     pind->input[nsensor+1+f] = 0.0;
                 nsensor += pipar->signalssf;
		}
	}


	//***************************************
        // sensors encoding previously produced signals
	if (pipar->signalso > 0)
	{
	     for(s=0;s < pipar->signalso; s++,nsensor++)
               pind->input[nsensor] = oselect(pind,ipar->wheelmotors+s);
	}
	//********************************************
	// signals from other robots (versione DAVIDE)
	// stefano: we should used a pind variable here
	//IRSensor[i] = attivazione sensori direzionali
	//old_max_irs[i][0] = distanza
	// pind->othersensor[4][10]
	if (ninput_other > 0)
	{
	   index = 0;
	   for(i=0;i<4;i++)
	   {
	      IRSensor[i] = 0.0f;
	      old_max_irs[i][0] = -1.0f;
	   }
	   for(i=0;i<4;i++)
	      for(ii=0;ii<ninput_other;ii++)
		 pind->othersensor[i][ii] = 0.0f;

	   for (t=0, cind=ind; t<nteam; t++, cind++)
		{
		  if ((p1 != cind->pos[0] || p2 != cind->pos[1]))
		  {
		    b[0] = cind->pos[0];
		    b[1] = cind->pos[1];

		    // controllo che non ci siano ostacoli fra i robot
		    ostacoli = 0;
		    for (i=4; i<env[cworld].nobstacles; i++)
			{
			  ostacoli_1[0] = env[cworld].obstacles[i][0];
			  ostacoli_1[1] = env[cworld].obstacles[i][1];
			  ostacoli_2[0] = env[cworld].obstacles[i][2];
			  ostacoli_2[1] = env[cworld].obstacles[i][3];
			  ostacoli += SegSegInt(a, b, ostacoli_1,  ostacoli_2, p);
			}
		    if (ostacoli == 0)
			{
			  dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]) - ROBOTRADIUS;
			  if (dist < (double) max_dist_attivazione) // attivo i sensori solo entro "max_dist_attivazione" centimetri in funzione della distanza
			  {
			     dire = direction;
			     if (dire<0.0f)
			       dire+=360;
			     if (dire>359.0f)
			       dire-=360;

			     headx = SetX(p1, ROBOTRADIUS, dire);
			     heady = SetY(p2, ROBOTRADIUS, dire);

			     IRSensor[0]= (10000.0f * (( max_dist_attivazione - (float) mdist(headx,heady,cind->pos[0],cind->pos[1]) ) / max_dist_attivazione));
			     if (IRSensor[0] < 0)
				IRSensor[0] = 0;


			     dire=direction+90.0f;
			     if (dire<0.0f)
				dire+=360;
			     if (dire>359.0f)
				dire-=360;

			     headx = SetX(p1, ROBOTRADIUS, dire);
			     heady = SetY(p2, ROBOTRADIUS, dire);

			     IRSensor[1]= (10000.0f * (( max_dist_attivazione - (float) mdist(headx,heady,cind->pos[0],cind->pos[1]) ) / max_dist_attivazione));
			     if (IRSensor[1] < 0)
				 IRSensor[1] = 0;


			     dire=direction+180.0f;
			     if (dire<0.0f)
				 dire+=360;
			     if (dire>359.0f)
				 dire-=360;

			     headx = SetX(p1, ROBOTRADIUS, dire);
			     heady = SetY(p2, ROBOTRADIUS, dire);

			     IRSensor[2]= (10000.0f * (( max_dist_attivazione - (float) mdist(headx,heady,cind->pos[0],cind->pos[1]) ) / max_dist_attivazione));
			     if (IRSensor[2] < 0)
				IRSensor[2] = 0;


			     dire=direction+270.0f;
			     if (dire<0.0f)
				 dire+=360;
			     if (dire>359.0f)
				 dire-=360;

			     headx = SetX(p1, ROBOTRADIUS, dire);
			     heady = SetY(p2, ROBOTRADIUS, dire);

			     IRSensor[3]= (10000.0f * (( max_dist_attivazione - (float) mdist(headx,heady,cind->pos[0],cind->pos[1]) ) / max_dist_attivazione));
			     if (IRSensor[3] < 0)
				IRSensor[3] = 0;

			     max=-1.0f;

			     for(ii=0;ii<4;ii++)
				{
				  if(IRSensor[ii] > max)
				  {
				    max=IRSensor[ii];
				    index = ii;
				  }
				}
			     // CONTROLLO: quando due o più robot attivano lo stesso sensore e prendo l'output di quello più vicino
			     if (IRSensor[index] > old_max_irs[index][0])
				{
				   for (ii=0; ii<ninput_other; ii++)
				      pind->othersensor[index][ii] = oselect(cind, 2+ii);
				   old_max_irs[index][0] = IRSensor[index];
				   old_max_irs[index][1] = (float) cind->number;
				}
				else
				{
				   struct individual *c = ind;
				   c += (int) old_max_irs[index][1];

				   for (ii=0; ii<ninput_other; ii++)
					 pind->othersensor[index][ii] = oselect(c, 2+ii);
				}
			  } // endif dist < threshold
		    }  // endif ostacoli = 0
		  } // if cind != ind
		} // for cind
		for(i=0;i<4;i++)
			for (ii=0; ii<ninput_other; ii++,nsensor++)
				pind->input[nsensor] = pind->othersensor[i][ii];
			// per dare in input un solo valore, quello del robot più vicino - pind->othersensor[index][ii] -

	}

  // *********************************
  // camera (360 or 320 pixels mode)
  //
  if (pipar->cameraunits > 0)
  {
    /*
    // we update the position of the landmarks
    // corresponding to the robots
    if (env[cworld].nlandmarks < nteam)
      env[cworld].nlandmarks = nteam;
    for(team=0, cind=ind, i = env[cworld].nlandmarks - nteam; team < nteam; team++,cind++,i++)
	  {
	   env[cworld].landmark[i][0] = cind->pos[0];
	   env[cworld].landmark[i][1] = cind->pos[1];
	   env[cworld].landmark[i][2] = ROBOTRADIUS;
	   env[cworld].landmark[i][3] = 0.0; // cylinder
           if (ipar->signaltype == 1 && ipar->signalso > 0 && pind->lifecycle > 0)
	     env[cworld].landmark[i][4] = oselect(cind,ipar->wheelmotors); // varying colour
	    else
	     env[cworld].landmark[i][4] = 1.0; // fixed colour
	  }
    */
    int degree = 320;
    for(i=0; i<degree; i++)
      pind->virtual_camera[i] = background_color;

    float value;

    // normalize to 320 pixels
    if (degree == 360)
       value = 1;
      else
       value = 0.888;

    // camera put in front of robot
    float x_cam    = pind->pos[0] + cos( DGRtoRAD( pind->direction) )* ROBOTRADIUS;
    float y_cam    = pind->pos[1] + sin( DGRtoRAD( pind->direction) )* ROBOTRADIUS;
    float dir_cam  = pind->direction;

    // we compute the distances (between the center of epuck and the center of landmarks)
    // ( column 5 in landmak stands for the distance! )
    for (i=0; i <= env[cworld].nlandmarks; i++)
      env[cworld].landmark[i][5] = (float) mdist(x_cam, y_cam, env[cworld].landmark[i][0], env[cworld].landmark[i][1]);


    // we look for the most distant and we update the virtual camera
    // after updating we set the distance to -1.0 (which means we then ignore it)
    for (i=0; i <= env[cworld].nlandmarks; i++)
	  {
	    // max distance among all landmarks
	    max_dist_l  = -1.0;

	    // index of landmark characterized by max distance
	    most_dist_l = -1;

	    for (ii=0; ii <= env[cworld].nlandmarks; ii++)
	    {
	      if (env[cworld].landmark[ii][5] > max_dist_l)
		{
		  max_dist_l = env[cworld].landmark[ii][5];
		  most_dist_l = ii;
		}
	    }

	    if (most_dist_l >= 0)
	    {
	      // cylinder
	      // the two points are orthogonal to the line between the robot and the cylinder
	      if (env[cworld].landmark[most_dist_l][3] == 0.0)
		{
		  dist = mdist(x_cam, y_cam, env[cworld].landmark[most_dist_l][0], env[cworld].landmark[most_dist_l][1]);
		  ang = mang( (int) x_cam, (int) y_cam, (int) env[cworld].landmark[most_dist_l][0], (int) env[cworld].landmark[most_dist_l][1], dist);
		  land_first[0]  = SetX( env[cworld].landmark[most_dist_l][0], env[cworld].landmark[most_dist_l][2], (double) ang + 90.0);
		  land_first[1]  = SetY( env[cworld].landmark[most_dist_l][1], env[cworld].landmark[most_dist_l][2], (double) ang + 90.0);
	          land_second[0] = SetX( env[cworld].landmark[most_dist_l][0], env[cworld].landmark[most_dist_l][2], (double) ang + 270.0);
		  land_second[1] = SetY( env[cworld].landmark[most_dist_l][1], env[cworld].landmark[most_dist_l][2], (double) ang + 270.0);
		}

	      // orizontal surface
	      if (env[cworld].landmark[most_dist_l][3] == 1.0)
		{
		  land_first[0]   = env[cworld].landmark[most_dist_l][0] - env[cworld].landmark[most_dist_l][2];
		  land_first[1]   = env[cworld].landmark[most_dist_l][1];

		  land_second[0]  = env[cworld].landmark[most_dist_l][0] + env[cworld].landmark[most_dist_l][2];
		  land_second[1]  = env[cworld].landmark[most_dist_l][1];
		}
	      // vertical surface
	      if (env[cworld].landmark[most_dist_l][3] == 2.0)
		{
		  land_first[0]   = env[cworld].landmark[most_dist_l][0];
		  land_first[1]   = env[cworld].landmark[most_dist_l][1] - env[cworld].landmark[most_dist_l][2];
		  land_second[0]  = env[cworld].landmark[most_dist_l][0] ;
		  land_second[1]  = env[cworld].landmark[most_dist_l][1] + env[cworld].landmark[most_dist_l][2];
		}

	      // calculate two distance:  between the camera and the first point, then between camera
	      // and second point:
	      dist_first      = mdist( x_cam,  y_cam, land_first[0] ,  land_first[1]  );
	      dist_second     = mdist( x_cam,  y_cam, land_second[0],  land_second[1] );

	      // now, calculate the absolute angles that this two point forms with the camera:
	      ang_first       = mang( (int)x_cam, (int)y_cam, (int) land_first[0] , (int) land_first[1] , dist_first);
	      ang_second      = mang( (int)x_cam, (int)y_cam, (int) land_second[0], (int) land_second[1], dist_second);

	      //we chose the smallest arc between the two possibilities
	      delta_ang1 = ang_second - ang_first;

	      if (delta_ang1 > 360.0)
		delta_ang1 -= 360.0;

	      if (delta_ang1 < 0.0)
		delta_ang1 += 360.0;

	      delta_ang2 = ang_first - ang_second;

	      if (delta_ang2 > 360.0)
		delta_ang2 -= 360.0;

	      if (delta_ang2 < 0.0)
		 delta_ang2 += 360.0;

	      // we set the real camera converting 360 degrees into 320 virtual pixels moltiplying for 0.888 (value)
	      // the value set in the real camera consist in the color value of the landmarks

	      if (delta_ang1 < delta_ang2)
		{
		 for (ii = (int) ang_first; ii <= (int) (ang_first + delta_ang1) ;ii++)
		 {
		   if (ii < 360)
		     pind->virtual_camera[(int) ((double)ii * value)] = env[cworld].landmark[most_dist_l][4];
		    else
		     pind->virtual_camera[(int) (((double) ii - 360.0) * value)] = env[cworld].landmark[most_dist_l][4];
		 }
		}
		else
		{
		  for (ii = (int) ang_second; ii <= (int) (ang_second + delta_ang2); ii++)
		    if (ii < 360)
		      pind->virtual_camera[(int) ((double)ii * value)]     = env[cworld].landmark[most_dist_l][4];
		     else
		      pind->virtual_camera[(int) (((double) ii - 360.0) * value)] = env[cworld].landmark[most_dist_l][4];
		}

		env[cworld].landmark[most_dist_l][5] = - (float) 1.0;
	    }
	  }

    // at this stage we set the input values
    // on the basis of the number of camera units,
    // the range of the camera

    //we identify the angle from which the camera starts to scan (from left to right)
    ang_d = dir_cam - ((double) pipar->camerarange / 2.0);

    if (ang_d < 0)
	ang_d += 360.0;

    if (ang_d > 359)
        ang_d -= 360.0;

    // we compute the angular range of each unit
    float incr = ((float)pipar->camerarange / (float)pipar->cameraunits);

    // initial angle
    int index = (int)ang_d;

    float sum;
    float media;
    int iterazioni;

    for (i=0; i < pipar->cameraunits; i++, nsensor++)
    {
     // we compute the average number of pixel activated for each sector
     sum = 0;
     iterazioni=0;
     for( int j=(int)(index *value); j<( (int)((index + (int)incr)*value) ); j++)
	{
	  sum += pind->virtual_camera[j];
	  iterazioni ++;
	}
     media = sum / (float)iterazioni;
     pind->input[nsensor] = media;
     pind->real_camera[i] = pind->input[nsensor];
     if (pind->input[nsensor] > 1.0)
	pind->input[nsensor] = 1.0f;

     if (pind->input[nsensor] < 0.0)
	pind->input[nsensor] = 0.0f;


     index += (int)incr;
     if (index > 359)
	index -= 360.0;
    }

   }
	//***********************************************
	// elman sensors (internal neurons at time t-1)
	if (pipar->elman_net == 1 && pipar->nhiddens > 0)
	{
	 for (i=0;i < pipar->nhiddens; i++,nsensor++)
	   if (pind->lifecycle == 0)
	     pind->input[nsensor] = (float) 0.0;
            else
	     pind->input[nsensor] = pind->activation[pind->ninputs+i];
	}
	//*************************************************
	// the visually detected angle of other robots
	if (pipar->simplevision == 1)
	{
         pind->input[nsensor]   = 0.0;
         pind->input[nsensor+1] = 0.0;
         pind->input[nsensor+2] = 0.0;
         pind->input[nsensor+3] = 0.0;
         pind->input[nsensor+4] = 0.0;
	 for (t=0,cind=ind; t<nteam; t++, cind++)
	 {
	  if (pind->n_team != cind->n_team)
	  {
	   dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]);
	   ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist);
	   relang = (int) mangrel2((double)ang,(double)direction);
	   if (relang >= 342 && relang <= 351)
              pind->input[nsensor] = 1.0;
	   if (relang > 351 && relang <= 360)
              pind->input[nsensor+1] = 1.0;
	   if (relang >= 0 && relang <= 9)
              pind->input[nsensor+2] = 1.0;
	   if (relang > 9 && relang <= 18)
              pind->input[nsensor+3] = 1.0;
	   if (relang > 18 && relang < 342)
              pind->input[nsensor+4] = 1.0;
	  }
	 }
	 nsensor += 5;
	}
	//***********************************************
	// epuck camera is simulated through computing the relevant angle for a left and right input
	// if no other epuck is perceived, both inputs are 0
	// max angle of epuck camera is defined by pipar->camera
	// activation of neurons is 'relative angle_left*(100/(degrees/2))' and 'relative angle_right*(100/18)',
	// if other robot is in front, activation for both neurons is 100
	//
	if (pipar->simplevision == 2)
	{
		for (t=0,cind=ind; t<nteam; t++, cind++)
		{
			if ((p1 != cind->pos[0] || p2 != cind->pos[1]))
			{
				float input_left;
				float input_right;
				float input_not_visible;
				float degrees = 18;

				//compute distance and angle
				dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]) - 27.5;
				ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist+27.5);
				relang  = mangrel2(ang,direction);

				//other epuck is in view
				if (relang < degrees || relang > (360-degrees))
				{
						if ( relang < degrees )
						{
							input_left = (float) ((degrees-relang)*(100/degrees))/100;
							input_right = (float) 0.0f;
							input_not_visible = 0.0f;
						}
						else
						{
							input_left = (float) 0.0;
							input_right = (float) ((relang-(360-degrees))*(100/degrees))/100;
							input_not_visible = 0.0f;
						}
					}

				//no other epuck is visible
				else
				{
					input_left = 0.0;
					input_right = 0.0;
					input_not_visible = 1.0f;
				}

				if (input_left > 1.0)
					input_left = 1.0;
				if (input_left < 0.0)
					input_left = 0.0;
				if (input_right > 1.0)
					input_right = 1.0;
				if (input_right < 0.0)
					input_right = 0.0;

				pind->input[nsensor] = input_left;
				pind->input[nsensor+1] = input_right;
				pind->input[nsensor+2] = input_not_visible;
			}
		}
		nsensor += 3;
	}

	/*
	if (pipar->simplevision == 1)
	{
         pind->input[nsensor]   = 0.0;
         pind->input[nsensor+1] = 0.0;
         pind->input[nsensor+2] = 0.0;
	 for (t=0,cind=ind; t<nteam; t++, cind++)
	 {
	  if (pind->n_team != cind->n_team)
	  {
	   dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]);
	   ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist);
	   relang = (int) mangrel2((double)ang,(double)direction);
	   if (relang >= 342 && relang <= 360)
	   {
              pind->input[nsensor] = 1.0;
              pind->input[nsensor+1] = 0.5f - (((360.0f - relang) / 18.0f) * 0.5f);
	   }
	   if (relang >= 0 && relang <= 18)
	   {
              pind->input[nsensor] = 1.0;
              pind->input[nsensor+1] = 0.5f + ((relang / 18.0f) * 0.5f);
	   }
	  }
	 }
	 nsensor += 3;
	}
	*/

	//************************************************
	// additional ground sensors (last visited area)
	if (pipar->a_groundsensor2 > 0)
	{
	  for (i=0;i < pipar->a_groundsensor2; i++, nsensor++)
           pind->input[nsensor] = pind->ground[i];

	  g = read_ground(pind);
	  if (g > 0)
	  for (i=0;i < pipar->a_groundsensor2; i++)
	   {
	    if (i == (g - 1))
              pind->ground[i] = 1.0;
	    else
	      pind->ground[i] = 0.0;
	   }
	}
	//************************************************
	// ground sensors
	if (pipar->groundsensor > 0)
	{
	  for (i=0;i < pipar->groundsensor; i++)
	  {
            pind->input[nsensor+i] = (float) 0.0;
	  }

	  g = read_ground(pind);
          if (g > 0)
	  {
	    g -= 1;
	    pind->input[nsensor+(g % pipar->groundsensor)] = 1.0f;
	  }

	  nsensor += pipar->groundsensor;

	}

	//************************************************
	// additional ground sensors (delta)
	if (pipar->a_groundsensor > 0)
	{
	  g = read_ground(pind);
	  for (i=0;i < pipar->a_groundsensor; i++)
	  {
	   if (g > 0 && i == (g - 1))
             pind->ground[i] = pind->ground[i] * 0.99 + 0.01;
	   else
	     pind->ground[i] = pind->ground[i] * 0.99;
	  }
	  for (i=0;i < pipar->a_groundsensor; i++, nsensor++)
           pind->input[nsensor] = pind->ground[i];
	}


	//*************************************************
	// the state of additional output units at time t-1
	if (pipar->input_outputs > 0)
	{
	  for (i=0;i < pipar->input_outputs; i++,nsensor++)
		pind->input[nsensor] = oselect(pind, (pipar->wheelmotors + pipar->signalso + i));

	}


	//*************************************************
	// proprioceptors of eaten nutrients
	if (pipar->nutrients > 0)
	{
         pind->input[nsensor] = pind->eatenfood[0] / 100.0f;
         pind->input[nsensor+1] = pind->eatenfood[1] / 100.0f;
         nsensor += 2;
	}

	//*************************************************
	// a compass sensor
	if (pipar->compass > 0)
	{
          pind->input[nsensor] = (float) (sin(DGRtoRAD(pind->direction)) + 1.0) / 2.0;
	  nsensor++;
          pind->input[nsensor] = (float) (cos(DGRtoRAD(pind->direction)) + 1.0) / 2.0;
	  nsensor++;
	}

	//*************************************************
	// energy sensor
	if (pipar->energy > 0)
	{
	  if (read_ground(pind) > 0)
            pind->input[nsensor] = 1.0f;
	  else
            pind->input[nsensor] = pind->input[nsensor] * 0.99f;
	  nsensor++;
	}

        //**************************************************
	// the most activated arbitration output at time t-1
	if (pipar->arbitration > 0)
	{
	  arb_max = -99.0f;
	  for (i=0;i < pipar->arbitration; i++)
	      if (oselect(pind, (pipar->wheelmotors + pipar->signalso + pipar->input_outputs + i)) > arb_max)
		{
		  arb_max = oselect(pind, (pipar->wheelmotors + pipar->signalso + pipar->input_outputs + i));
		  arb_ind = i;
		}
	  for (i=0;i < pipar->arbitration; i++,nsensor++)
		if (i == arb_ind)
		  pind->input[nsensor] = 1.0f;
		 else
		  pind->input[nsensor] = 0.0f;
	}


}

void
set_input_real(struct individual *pind, struct  iparameters *pipar)
{
//used to read robot sensors

int i;
int ii;
int floor1,floor2,floor3;
int nsensor = 0;
int ground1,ground2,ground3, mean, mean_time;
char debugstring[200];
int sMode = 0;   //0 old khepera mode 1 epuck numeration
struct individual *cind;
int g;
int epuck_angle;
int t,s;
float localground[10];

	//motor states at time t-1
	if (pipar->motmemory > 0)
	{
	pind->input[nsensor]= oselect(pind,0);
	pind->input[nsensor]= oselect(pind,1);
	nsensor += 2;
	}

	if(sMode == 1)
	{
		for (i=0;i<8;i++)
		{
		pind->cifsensors[i]   =     (float)(robots[pind->number].IR[i])/4096.0f;
		pind->clightsensors[i]=1 - ( (float)robots[pind->number].LI[i]/4096.0f);
		}
	}else
	{
		//old khepera mode
		pind->cifsensors[0]=(float)(robots[pind->number].IR[5])/4096.0f;
		pind->cifsensors[1]=(float)(robots[pind->number].IR[6])/4096.0f;
		pind->cifsensors[2]=(float)(robots[pind->number].IR[7])/4096.0f;
		pind->cifsensors[3]=(float)(robots[pind->number].IR[0])/4096.0f;
		pind->cifsensors[4]=(float)(robots[pind->number].IR[1])/4096.0f;
		pind->cifsensors[5]=(float)(robots[pind->number].IR[2])/4096.0f;
		pind->cifsensors[6]=(float)(robots[pind->number].IR[3])/4096.0f;
		pind->cifsensors[7]=(float)(robots[pind->number].IR[4])/4096.0f;

		pind->clightsensors[0]=1 - ( (float)robots[pind->number].LI[5]/4096.0f);
		pind->clightsensors[1]=1 - ( (float)robots[pind->number].LI[6]/4096.0f);
		pind->clightsensors[2]=1 - ( (float)robots[pind->number].LI[7]/4096.0f);
		pind->clightsensors[3]=1 - ( (float)robots[pind->number].LI[0]/4096.0f);
		pind->clightsensors[4]=1 - ( (float)robots[pind->number].LI[1]/4096.0f);
		pind->clightsensors[5]=1 - ( (float)robots[pind->number].LI[2]/4096.0f);
		pind->clightsensors[6]=1 - ( (float)robots[pind->number].LI[3]/4096.0f);
		pind->clightsensors[7]=1 - ( (float)robots[pind->number].LI[4]/4096.0f);
		//finish old khepera sensor mode
	}


	// add noise to infra-red sensors if requested
	if (sensornoise > 0.0)
	  for(i=0; i < 8; i++)
	  {
	   pind->cifsensors[i] += rans(sensornoise);
	   if (pind->cifsensors[i] < 0.0) pind->cifsensors[i] = (float) 0.0;
	   if (pind->cifsensors[i] > 1.0) pind->cifsensors[i] = (float) 1.0;
      }



//infrared sensors
if (pipar->nifsensors == 4)
	{
	  pind->input[nsensor]   = (pind->cifsensors[0] + pind->cifsensors[1]) / 2.0f;
	  pind->input[nsensor+1] = (pind->cifsensors[2] + pind->cifsensors[3]) / 2.0f;
	  pind->input[nsensor+2] = (pind->cifsensors[4] + pind->cifsensors[5]) / 2.0f;
	  pind->input[nsensor+3] = (pind->cifsensors[6] + pind->cifsensors[7]) / 2.0f;
	  nsensor += 4;
	}
	if (pipar->nifsensors == 6)
	{
	  for(i=0;i<6;i++,nsensor++)
	   pind->input[nsensor] = pind->cifsensors[i];
	}
	if (pipar->nifsensors == 8)
	{
	  for(i=0;i<8;i++,nsensor++)
	   pind->input[nsensor] = pind->cifsensors[i];


	}
//light sensors
	if (pipar->lightsensors == 2)
	  {
	     pind->input[nsensor] = pind->clightsensors[2];
	     nsensor++;
	     pind->input[nsensor] = pind->clightsensors[6];
	     nsensor++;
	  }
	  if (pipar->lightsensors == 3)
	  {
	     pind->input[nsensor] = (pind->clightsensors[0] + pind->clightsensors[1]) / 2.0f;
	     nsensor++;
	     pind->input[nsensor] = (pind->clightsensors[2] + pind->clightsensors[3]) / 2.0f;
	     nsensor++;
	     pind->input[nsensor] = (pind->clightsensors[4] + pind->clightsensors[5]) / 2.0f;
	     nsensor++;
	  }
	  if (pipar->lightsensors == 8)
	  {
	     for(i=0;i<8;i++,nsensor++)
		pind->input[nsensor] = pind->clightsensors[i];
	  }
	  	  //communication
	  	  	if (pipar->signalss > 0)
	  	{
	  	   for(i=0;i< pipar->signalss;i++)
	  	   {
	  	     pind->detected_signal[i][0] = 0.0f;
	  	     pind->detected_signal[i][1] = 999999.0f;
	  	   }
	  	   for (t=0, cind=ind; t<nteam; t++, cind++)
	  	     if (pind->n_team != cind->n_team)
	  		{
	  		     if (pipar->signalss == 1)
	  		     // non-directional signals
	  		     {
	  		       //if (dist < pind->detected_signal[0][1])
	  		        for(s=0;s < pipar->signalso; s++)
	  						{
	                           pind->detected_signal[s][0] = oselect(cind,ipar->wheelmotors+s);
	                           //pind->detected_signal[s][1] = (float) dist;
	  						}
	  			 }
	  		}
	  	}
	  	for(s=0;s < pipar->signalss*pipar->signalso; s++, nsensor++)
	                   pind->input[nsensor] = pind->detected_signal[s][0];
	//end Communnication to be checked

	//camera Joachim

		if (pipar->simplevision == 2)
		{

				epuck_angle = robots[pind->number].MR[0]; //inside MicroRetina


						if (epuck_angle > 0  && epuck_angle<30)
						{
							pind->input[nsensor] = 0.0f;
							robots[pind->number].old_input[0] = 0.0f;
							pind->input[nsensor+1] = (float) ((float)epuck_angle/29.0);
							robots[pind->number].old_input[1] = (float) ((float)epuck_angle/29.0);
							pind->input[nsensor+2] = 0.0f;
							robots[pind->number].old_input[2] = 0.0f;
						}
						if (epuck_angle >= 30 )
						{
							pind->input[nsensor] = (float) (1.0-(((float)epuck_angle-30.0)/29.0));
							robots[pind->number].old_input[0] = (float) (1.0-(((float)epuck_angle-30.0)/29.0));
							pind->input[nsensor+1] = 0.0f;
							robots[pind->number].old_input[1] = 0.0f;
							pind->input[nsensor+2] = 0.0f;
							robots[pind->number].old_input[2] = 0.0f;
						}
						if (epuck_angle == 0 && robots[pind->number].epuck_angle_old == 0)
						{
							pind->input[nsensor] = 0.0f;
							robots[pind->number].old_input[0] = 0.0f;
							pind->input[nsensor+1] = 0.0f;
							robots[pind->number].old_input[1] = 0.0f;
							pind->input[nsensor+2] = 1.0f;
							robots[pind->number].old_input[2] = 1.0f;
						}
						else
						{
							pind->input[nsensor] = robots[pind->number].old_input[0];
							pind->input[nsensor+1] = robots[pind->number].old_input[1];
							pind->input[nsensor+2] = robots[pind->number].old_input[2];
						}

					nsensor += 3;
		robots[pind->number].epuck_angle_old = epuck_angle;
		}

        // compute grand sensors
		if (pipar->groundsensor > 0)
			{
				// 1 ground sensor input
				if (pipar->groundsensor == 1)
				{
					ground2 = robots[pind->number].FL[1];

					if (ground2 < 500)
					{
						localground[0] = 1.0f;
					
					}
					else
					{
						localground[0] = 0.0f;
					
					}
				
						

				}

				//2 different ground sensor inputs
				if (pipar->groundsensor == 2)
				{
					ground2 = robots[pind->number].FL[1];

					//range method
					if (ground2 < 415)
					{
                       localground[0] = 0.0f;
                       localground[1] = 0.0f;

					}
					if (ground2 >= 415 && ground2 <= 900)
					{
                       localground[0] = 0.0f;
                       localground[1] = 1.0f;

					}
					if (ground2 > 900)
					{
                       localground[0] = 1.0f;
                       localground[1] = 0.0f;

					}
				}
			}

	if (pipar->a_groundsensor2 > 0)
	{
	  for (i=0;i < pipar->a_groundsensor2; i++, nsensor++)
		 if (localground[0] == 0.0 && localground[1] == 0.0)
           pind->input[nsensor] = pind->ground[i];
		 else
           pind->input[nsensor] = localground[i];
	}

    for (i=0;i < pipar->groundsensor; i++, nsensor++)
	{

        pind->ground[i] = localground[i];
		pind->input[nsensor]= localground[i];
	}



}//end of set_input_real
/*
 *  return the id number of the target area in which
 *  the robot is located or 0
 */
int
read_ground(struct individual *pind)
{

	double distzone;
	int z;
	float p1;
	float p2;

	//calculate location of floor sensor = in front of robot body, as in ePuck
	p1 = SetX(pind->pos[0], 37.0, pind->direction);
	p2 = SetY(pind->pos[1], 37.0, pind->direction);
	if (env[cworld].nfoodzones > 0)
	{
	  for(z=0; z < env[cworld].nfoodzones; z++)
		{
		  distzone = (p1 - env[cworld].foodzones[z][0])*(p1 - env[cworld].foodzones[z][0])+
                    (p2 - env[cworld].foodzones[z][1])*(p2 - env[cworld].foodzones[z][1]);
		  distzone = sqrt(distzone);
		  if (distzone < env[cworld].foodzones[z][2])
			return(z+1);
		}
	}
	return(0);
}



/*
 * checks whether a crash occurred
 */
int
check_crash(float p1,float p2)

{
  int i;
  float   x,y;
  double  dist;
  double  mindist;
  struct individual *pind;

  mindist = 9999.0;
  for(i=0;i<env[cworld].nobstacles;i++)
   {
    if (p1 >= env[cworld].obstacles[i][0] && p1 <= env[cworld].obstacles[i][2])
       x = p1;
     else
       x = env[cworld].obstacles[i][0];
    if (p2 >= env[cworld].obstacles[i][1] && p2 <= env[cworld].obstacles[i][3])
       y = p2;
     else
       y = env[cworld].obstacles[i][1];
    dist = mdist(p1,p2,x,y);
    if (dist < mindist)
      mindist = dist;
   }
  for(i=0;i<env[cworld].nroundobst;i++)
   {
    dist = mdist(p1,p2,env[cworld].roundobst[i][0],env[cworld].roundobst[i][1])-env[cworld].radius;
    if (dist < mindist)
      mindist = dist;
   }
  for(i=0;i<env[cworld].nsroundobst;i++)
    {
     if (env[cworld].sroundobst[i][2] == 0.0)
      {
       dist = mdist(p1,p2,env[cworld].sroundobst[i][0],env[cworld].sroundobst[i][1])-env[cworld].sradius;
       if (dist < mindist)
	 mindist = dist;
      }
    }
   // check if a crash occurred with another individuals
   // by ignoring individuals that have the same idential positions - theirself
   for (pind=ind,i=0; i < nteam; i++, pind++)
	  {
	    if (p1 != pind->pos[0] || p2 != pind->pos[1])
		{
		  dist = mdist(p1,p2,pind->pos[0],pind->pos[1])-ROBOTRADIUS;
		  if (dist < mindist)
			{
			  mindist = dist;
			  if (dist < ROBOTRADIUS)
			    individual_crash = 1;
			}
		}
	  }

   if (mindist < ROBOTRADIUS)
     return(1);
   else
     return(0);
}




/*
 * move the robot on the basis of sampled motion
 */
 void setpos(struct individual *pind,float o1, float o2, float speed)
 {

	 int    v1,v2;
	 float  **mo;
	 float  *m;
	 float  ang, dist;
	 extern float mod_speed;
	 extern int   selected_ind_mod_speeed;


	 pind->oldpos[0] = pind->pos[0];
	 pind->oldpos[1] = pind->pos[1];
	 pind->oldirection = pind->direction;

	 v1 = (int) (o1 * 20.0);
	 v2 = (int) (o2 * 20.0);
	 // approximate to the closest integer value
	 if ( ((o1 * 200) - (v1 * 20)) > 5)
		 v1++;
	 if ( ((o2 * 200) - (v2 * 20)) > 5)
	 	 v2++;

	 v1 -= 10;
	 v2 -= 10;

	 if (speed < 1.0f)
	 {
	  v1 = (int) ((float) v1 * speed);
	  v2 = (int) ((float) v2 * speed);
	 }

	 pind->speed[0] = (float) v1;
	 pind->speed[1] = (float) v2;

	 mo = motor;
	 m = *(mo + (v1 + 12));
	 m = (m + ((v2 + 12) * 2));
	 ang = *m;
	 m++;
	 dist = *m * (float) 10.0;

	 pind->pos[0] = SetX(pind->pos[0],dist,pind->direction);
	 pind->pos[1] = SetY(pind->pos[1],dist,pind->direction);


	 pind->direction += ang;
	 if (pind->direction >= 360.0)
		 pind->direction -= 360;
	 if (pind->direction < 0.0)
		 pind->direction += 360;

 }

 void setpos_real(struct individual *pind,float o1, float o2)
{
		float leftM,rightM;
		float range;
		range=594;//max 1000; 594 has been computed measuring displacement in simulated and real environment at the maximum speed(1000) for 50 cycles
		if(dtime>100) range=range* 100.0f/(float)dtime; //scale speed upon delay in sesnsors reading
		leftM=o1*range*2-range;
		rightM=o2*range*2-range;



		setRobotSpeed(&robots[pind->number],leftM,rightM);

}

/*
 * we replace the robot in the previous position
 */
void
reset_pos_from_crash(struct individual *pind)

{

    pind->direction = pind->oldirection;
    pind->pos[0]    = pind->oldpos[0];
    pind->pos[1]    = pind->oldpos[1];

    pind->speed[0]=0.0f;
    pind->speed[1]=0.0f;
}





/*
 * set the initial position of the robots
 */
void
set_initial_position()
{

    struct individual *pind;
    int    volte;
    int    startset;
    int    team;


    for(team=0,pind=ind; team < nteam; team++, pind++)
     if (pind->dead == 0)
     {
	startset = 0;

	// set pre-specified positions
	if (env[cworld].nstarts > 0 && startset == 0)
	{
	   pind->pos[0]    = env[cworld].starts[cepoch % env[cworld].nstarts][team][0];
	   pind->pos[1]    = env[cworld].starts[cepoch % env[cworld].nstarts][team][1];
	   pind->direction = env[cworld].starts[cepoch % env[cworld].nstarts][team][2];
	   startset = 1;
	}

	/*
	 * for collective navigations robots start randomly outside
	 * target areas
	 */
	if ((ipar->ffitness == 3 || ipar->ffitness == 4) && startset == 0)
	{
           startset = 1;
	   volte=0;
	   do
	   {
	     volte++;
	     pind->pos[0] = (float) fabs(rans((float) (env[cworld].dx - 80))) + 40;
	     pind->pos[1] = (float) fabs(rans((float) (env[cworld].dy - 80))) + 40;
             pind->direction = fabs(rans(360.0));
	   }
	   while (volte == 0 || (volte < 1000 && ((check_crash(pind->pos[0],pind->pos[1]) == 1) || (read_ground(pind) > 0)) ));
	}
	/*
	 * start out of foodzones
	 */
	 if (startfarfrom == 1 && startset == 0)
		{
		  pind->pos[0]    = 50.0 + ( double ) mrand(env[cworld].dx - 100);
		  pind->pos[1]    = 50.0 + ( double ) mrand(env[cworld].dy - 100);
		  pind->direction = (double) mrand(360);
		  volte = 0;
		  while (volte < 1000 && ((check_crash(pind->pos[0],pind->pos[1]) == 1) || (read_ground(pind) > 0)))
		  {
		    volte++;
		    pind->pos[0]    = 50.0 + ( double ) mrand(env[cworld].dx - 100);
		    pind->pos[1]    = 50.0 + ( double ) mrand(env[cworld].dy - 100);
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
	     pind->pos[0] = (float) fabs(rans((float) (env[cworld].dx - 80))) + 40;
	     pind->pos[1] = (float) fabs(rans((float) (env[cworld].dy - 80))) + 40;
             pind->direction = fabs(rans(360.0));
	   }
	   while (volte == 0 || (volte < 1000 && (check_crash(pind->pos[0],pind->pos[1]) == 1) ));
	}
     }

}


/*
 * initialize varying environmental parameters
 */
void
set_world_parameters(int gen,int pop, int nind,int epoch)


{

    int    i;
    int    ta,tt;
    int    tw;
    double d,mind;
    double w1, w2;
    int    volte;
    double min_dist_ob;

    //randomize foodzone positions (joachim)
    if (random_foodzones == 1)
	{
	  for (ta=0; ta<env[cworld].nfoodzones; ta++)
	  {
		mind = 0.0;
		volte = 0;
		min_dist_ob = env[cworld].foodzones[ta][2]*3.0f;
		while (mind < min_dist_ob && volte < 20000)
		{
			mind = 9999.9;
			volte++;
			if ((volte > 199) && (volte % 10) == 0)
				min_dist_ob -= 1;
			env[cworld].foodzones[ta][0] = (float) (fabs(rans((float) env[cworld].dx - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			env[cworld].foodzones[ta][1] = (float) (fabs(rans((float) env[cworld].dy - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			//check for other foodzone
			for (tt=0; tt < ta; tt++)
			{
				d = mdist(env[cworld].foodzones[tt][0],env[cworld].foodzones[tt][1],env[cworld].foodzones[ta][0],env[cworld].foodzones[ta][1]);
				if (d < mind)
					mind = d;
			}
			//check for wall
			if (env[cworld].obstacles[4][0] > 0.0)
			{
				for(tw=0; tw < env[cworld].nfoodzones; tw++)
				{
					w1 = mdist(env[cworld].foodzones[tw][0],env[cworld].foodzones[tw][1],env[cworld].obstacles[4][0],env[cworld].obstacles[4][1]);
					w2 = mdist(env[cworld].foodzones[tw][0],env[cworld].foodzones[tw][1],env[cworld].obstacles[4][2],env[cworld].obstacles[4][3]);
					if (w1 < mind)
						mind = w1;
					if (w2 < mind)
						mind = w2;
				}
			}

		}
	  }

     }
    //randomize foodzone positions
    if (random_foodzones == 2)
	{
	  for (ta=0; ta<env[cworld].nfoodzones; ta++)
	  {
		mind = 0.0;
		volte = 0;
		min_dist_ob = env[cworld].foodzones[0][2]*2.0f;
		min_dist_ob += min_dist_ob * 0.1;
		while (mind < min_dist_ob && volte < 2000)
		{
			mind = 9999.9;
			volte++;
			if ((volte > 199) && (volte % 10) == 0)
				min_dist_ob -= 1;
			env[cworld].foodzones[ta][0] = (float) fabs(rans((float) env[cworld].dx - min_dist_ob)) + (min_dist_ob / 2);
			env[cworld].foodzones[ta][1] = (float) fabs(rans((float) env[cworld].dy - min_dist_ob)) + (min_dist_ob / 2);
			for (tt=0; tt < ta; tt++)
			{
				d = mdist(env[cworld].foodzones[tt][0],env[cworld].foodzones[tt][1],
				env[cworld].foodzones[ta][0],env[cworld].foodzones[ta][1]);
				if (d < mind)
					mind = d;
			}
		}
	  }
	}
    //randomize foodzone positions (tipically close to the walls)
    if (random_foodzones == 3)
	{
	  for (ta=0; ta<env[cworld].nfoodzones; ta++)
	  {
		mind = 0.0;
		volte = 0;
		min_dist_ob = env[cworld].foodzones[ta][2]*5.0f;
		while (mind < min_dist_ob && volte < 20000)
		{
			mind = 9999.9;
			volte++;
			if ((volte > 199) && (volte % 10) == 0)
				min_dist_ob -= 1;
			env[cworld].foodzones[ta][0] = (float) (fabs(rans((float) env[cworld].dx - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			env[cworld].foodzones[ta][1] = (float) (fabs(rans((float) env[cworld].dy - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			for (tt=0; tt < ta; tt++)
			{
				d = mdist(env[cworld].foodzones[tt][0],env[cworld].foodzones[tt][1],
				env[cworld].foodzones[ta][0],env[cworld].foodzones[ta][1]);
				if (d < mind)
					mind = d;
			}

		}
	  }
	}
    //randomize foodzone positions (the first in a corner)
    if (random_foodzones == 4)
	{
	  env[cworld].foodzones[0][0] = 50.0f;
	  env[cworld].foodzones[0][1] = 50.0f;
	  for (ta=1; ta<env[cworld].nfoodzones; ta++)
	  {
		mind = 0.0;
		volte = 0;
		min_dist_ob = env[cworld].foodzones[ta][2]*5.0f;
		while (mind < min_dist_ob && volte < 20000)
		{
			mind = 9999.9;
			volte++;
			if ((volte > 199) && (volte % 10) == 0)
				min_dist_ob -= 1;
			env[cworld].foodzones[ta][0] = (float) (fabs(rans((float) env[cworld].dx - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			env[cworld].foodzones[ta][1] = (float) (fabs(rans((float) env[cworld].dy - (env[cworld].foodzones[ta][2] * 2.0f))) + env[cworld].foodzones[ta][2]);
			for (tt=0; tt < ta; tt++)
			{
				d = mdist(env[cworld].foodzones[tt][0],env[cworld].foodzones[tt][1],
				env[cworld].foodzones[ta][0],env[cworld].foodzones[ta][1]);
				if (d < mind)
					mind = d;
			}

		}
	  }
	}
    // we randomize sround positions
    if (random_sround == 1)
	{
	  for (ta=0; ta<env[cworld].nsroundobst; ta++)
		{
		  mind = 0.0;
		  volte = 0;
		  min_dist_ob = 150;
		  while (mind < min_dist_ob && volte < 1000)
		  {
		    volte++;
		    if ((volte > 199) && (volte % 10) == 0)
		      min_dist_ob -= 1;
		    env[cworld].sroundobst[ta][0] = (float) (fabs(rans((float) env[cworld].dx - (double) (120 * 2))) + 120);
		    env[cworld].sroundobst[ta][1] = (float) (fabs(rans((float) env[cworld].dy - (double) (120 * 2))) + 120);
		    env[cworld].sroundobst[ta][2] = (float) 0.0;
		    for (tt=0, mind = 9999.9; tt < ta; tt++)
			{
			 d = mdist(env[cworld].sroundobst[tt][0],env[cworld].sroundobst[tt][1],
                         env[cworld].sroundobst[ta][0],env[cworld].sroundobst[ta][1]);
			 if (d < mind)
			   mind = d;
			}

		  }
		}
	}

    // we randomize round positions
    if (random_round == 1)
	{
	  for (ta=0; ta<env[cworld].nroundobst; ta++)
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
		  env[cworld].roundobst[ta][0] = (float) (fabs(rans((float) env[cworld].dx - 275)) + 132.0);
		  env[cworld].roundobst[ta][1] = (float) (fabs(rans((float) env[cworld].dy - 275)) + 132.0);
		  env[cworld].roundobst[ta][2] = (float) 0.0;
		  for (tt=0; tt < ta; tt++)
			{
			  d = mdist(env[cworld].roundobst[tt][0],env[cworld].roundobst[tt][1],
                          env[cworld].roundobst[ta][0],env[cworld].roundobst[ta][1]);
			  if (d < mind)
			    mind = d;
			}
		  for (tt=0; tt < env[cworld].nsroundobst; tt++)
			{
			  d = mdist(env[cworld].sroundobst[tt][0],env[cworld].sroundobst[tt][1],
                          env[cworld].roundobst[ta][0],env[cworld].roundobst[ta][1]);
			  if (d < mind)
			    mind = d;
			}
		}
	  }
	}

     // reset foodzone visited counter (ffitness=5)
     if (ipar->ffitness == 5)
       for (i=0; i < env[cworld].nfoodzones; i++)
         env[cworld].foodzones[i][3] = 0.0f;


}

/*
 * allocate space for sampled objects
 */
void
create_world_space()

{
    int 	**wa;
    int		**ob;
    float	**mo;
    int         *v;
    int         i;
    int         ii;

     /*
     * wall
     * allocate space for 30 distances for each sensor
     */
    wall = (int **) malloc(30 * sizeof(int *));
    wallcf = (int *) malloc(5 * sizeof(int));

    if (wall == NULL || wallcf == NULL)
	 display_error("system error: wall malloc");

    /*
     * allocate space for directions
     */
    wa = wall;
    for (i=0; i<30; i++,wa++)
	  {
	   *wa = (int *)malloc((8 * 180) * sizeof(int));
	   if (*wa == NULL)
	      display_error("system error: wa malloc");
	   for (ii=0,v=*wa; ii < (8 *180); ii++,v++)
		*v = 0;
	  }

    /*
     * round obstacles
     * allocate space for 30 distances for each sensor
     */
     obst = (int **) malloc(30 * sizeof(int *));
     obstcf = (int *) malloc(5 * sizeof(int));

     if (obst == NULL || obstcf == NULL)
	  display_error("system error: obst malloc");
      /*
       * allocate space for directions
       */
      ob = obst;
      for (i=0; i<30; i++,ob++)
	  {
	   *ob = (int *)malloc((8 * 180) * sizeof(int));
	   if (*ob == NULL)
	      display_error("system error: ob malloc");
	   for (ii=0,v=*ob; ii < (8 *180); ii++,v++)
		*v = 0;
	  }
    /*
     * small round obstacles
     * allocate space for 30 distances for each sensor
     */
     sobst = (int **) malloc(30 * sizeof(int *));
     sobstcf = (int *) malloc(5 * sizeof(int));

     if (sobst == NULL || sobstcf == NULL)
	 display_error("system error: sobst malloc");
      /*
       * allocate space for directions
       */
      ob = sobst;
      for (i=0; i<30; i++,ob++)
	  {
	   *ob = (int *)malloc((8 * 180) * sizeof(int));
	   if (*ob == NULL)
	     display_error("system error: ob malloc");
	   for (ii=0,v=*ob; ii < (8 *180); ii++,v++)
		*v = 0;
	  }
    /*
     * light
     * allocate space for 30 distances for each sensor
     */
    light = (int **) malloc(30 * sizeof(int *));
    lightcf = (int *) malloc(5 * sizeof(int));

    if (light == NULL || lightcf == NULL)
	 display_error("system error: light malloc");
      /*
       * allocate space for directions
       */
    ob = light;
    for (i=0; i<30; i++,ob++)
	  {
	   *ob = (int *)malloc((8 * 180) * sizeof(int));
	   if (*ob == NULL)
	    display_error("system error: light malloc");
	   for (ii=0,v=*ob; ii < (8 *180); ii++,v++)
		*v = 0;
	  }
    /*
     * allocate space for 25(v1) different speeds
     */
    motor = (float **) malloc(25 * sizeof(float *));
    if (motor == NULL)
	    display_error("system error: motor malloc");
      /*
       * allocate space for 25(v1) different speeds (ang & dist)
       */
    mo = motor;
    for (i=0; i<25; i++,mo++)
	  {
	   *mo = (float *)malloc((25 * 2) * sizeof(float));
	   if (*mo == NULL)
	    display_error("system error: mo malloc");
	  }


}


