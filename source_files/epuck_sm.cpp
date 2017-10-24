/* 
 * Evorobot* - Simulator.cpp
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

#include "public.h"
#include "epuck.h"
#include "environment.h"

MusclePair *xMuscle=NULL;
MusclePair *yMuscle=NULL;
MusclePair *zMuscle=NULL;

double  P90,P180,P360;      // Pgreco constants

extern char robotwstatus[]; // stat message within rend_robot widget

float **motor;              // motor sampling, 12 rows each with 12*2(ang and dist) columnsi
int   **wall;               // infrared wall sampling, (180 degrees and 38 distances)
int   **obst;               // infrared round obstacle sampling, (180 d. and 38 distances
int   **sobst;              // infrared small round obstacle sampling (180 d. and 20 distances
int   **light;              // light sampling (180 degrees and 27 distances)
int   *wallcf;              /* wall sampling configuration                          */
int   *obstcf;              /* obst sampling configuration                          */
int   *sobstcf;             /* sobst sampling configuration                          */
int   *lightcf;             /* light sampling configuration                          */

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
* estimate the distance between a point (p) and a segment (a-b) 
* by computing dist(p,a) + dist(p,b) - dist(a,b)
* if one of the two distances are greater than the segment
* assume that the point is too far or is outside the segment
*/
double
segmentdist(float px,float py,float ax,float ay,float bx,float by) 
{
  double dab,dpa,dpb; 

  dab = sqrt(((ax-bx)*(ax-bx)) + ((ay-by)*(ay-by)));
  dpa = sqrt(((px-ax)*(px-ax)) + ((py-ay)*(py-ay)));
  dpb = sqrt(((px-bx)*(px-bx)) + ((py-by)*(py-by)));

  if (dpa < dab && dpb < dab)
    return((dpa+dpb)-dab);
  else
    return(99999.999);
}

/*
 * computes the end point of a segment:
 *
 * x1,y1 = starter point; 
 * directangle =  direction (in degreas) of segment;
 * displacement = lenght of segment;
 *
 */
void ToPoint(float x1, float y1, float directangle, float displacement, float *x2, float *y2)
{
    *x2 =  x1 + (displacement * cos(DGRtoRAD(directangle)));
    *y2 =  y1 + (displacement * sin(DGRtoRAD(directangle)));
}

/*
 * computes distance between two points
 */
double PointsDist(float x1, float y1, float x2,float y2)
{

     double d1 = sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));     

     return(d1);
}


/* this function computes the intersection of the sent lines
 * and returns the intersection point, note that the function assumes
 * the lines intersect. the function can handle vertical as well
 * as horizontal lines. note the function isn't very clever
 */
int intersect_lines(float x0, float y0, float x1, float y1 , float x2, float y2, float x3, float y3, float *xi, float *yi)
{


   double rm1, rm2;
   double a1,b1,c1, // constants of linear equations
          a2,b2,c2,
          det_inv,  // the inverse of the determinant of the coefficient matrix
          m1,m2;    // the slopes of each line

   // compute slopes, note the cludge for infinity, however, this will
   // be close enough

   if ((x1-x0)!=0)
     m1 = (y1-y0)/(x1-x0);
    else
     m1 = (double)1e+10;   // close enough to infinity

   if ((x3-x2)!=0)
     m2 = (y3-y2)/(x3-x2);
    else
     m2 = (double)1e+10;   // close enough to infinity

   // compute constants
   a1 = m1;
   a2 = m2;
   b1 = -1;
   b2 = -1;
   c1 = (y0-m1*x0);
   c2 = (y2-m2*x2);
   // compute the inverse of the determinate
   det_inv = 1/(a1*b2 - a2*b1);
   // use Kramers rule to compute xi and yi
   *xi=((b1*c2 - b2*c1)*det_inv);
   *yi=((a2*c1 - a1*c2)*det_inv);
   if(*xi>0 && *xi<0.000001f) *xi=0.0f;
   if(*yi>0 && *yi<0.000001f) *yi=0.0f;
   rm1=PointsDist(*xi,*yi,x2,y2)+PointsDist(*xi,*yi,x3,y3);
   rm2=PointsDist(*xi,*yi,x0,y0)+PointsDist(*xi,*yi,x1,y1);
   if ( (rm1<=PointsDist(x2,y2,x3,y3)+0.1) && (rm2<=PointsDist(x0,y0,x1,y1)+0.1))
     return 1;
    else
     return 0;
}


/*
 * Computes the frontal intersection point between lines and circles: 
 *
 * &xi , &yi = frontal intersection point, if exist
 * x1,y1 - x2,y2 = segment
 * cx, cy, r =  the center and the ray of the circle
 * return:  0 = non intersection
 *        1 = intersection
 *        2 = segment is inside the circle
 */
int intersect_line_circle(float x1,float y1, float x2,float y2, float cx,float cy, float r , float *xi,float *yi)
{
    //onofrio modifica
    *xi = 0.0f;
    *yi = 0.0f;
    int intersection = 0;
    float a  = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    float b  = 2 * ((x2 - x1) * (x1 - cx) +(y2 - y1) * (y1 - cy));
    float cc = (cx * cx) + (cy * cy) + (x1 * x1) + (y1 * y1) - 2 * (cx * x1 + cy * y1) - (r * r);
    float deter = (b * b) - 4 * a * cc;

    if (deter <= 0 )
    {
      intersection = 0;
    }
    else 
    {
        float e  = sqrt(deter);
        float u1  = ( - b + e ) / (2 * a );
        float u2  = ( - b - e ) / (2 * a );
        if ((u1 < 0 || u1 > 1) && (u2 < 0 || u2 > 1))
        {
            if ((u1 < 0 && u2 < 0) || (u1 > 1 && u2 > 1))
                intersection = 0;
            else 
                intersection = 2; 
        } 
        else 
        {
                *xi = x1 + u2 * (x2 - x1);
                *yi = y1 + u2 * (y2 - y1);
                intersection = 1;
        }
    }

    return(intersection);
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
 *  return the id number of the target area in which
 *  the robot is located (0 if it is located outside target areas) 
 */
int
read_ground(struct individual *pind)
{

    double distzone;
    int z;
    float p1;
    float p2;
        struct envobject *obj;

    //calculate location of floor sensor = in front of robot body, as in ePuck
    p1 = SetX(pind->pos[0], 37.0, pind->direction);
    p2 = SetY(pind->pos[1], 37.0, pind->direction);
    if (envnobjs[GROUND] > 0)
    {
      for(z=0, obj = *(envobjs + GROUND); z < envnobjs[GROUND]; z++, obj++)
        {
          if (obj->subtype == 0)
           {
            distzone = ((p1 - obj->x) * (p1 - obj->x)) + ((p2 - obj->y) * (p2 - obj->y));
            distzone = sqrt(distzone);
            if (distzone < obj->r)
            return(z+1);
           }
          else
           {
            if ( (p1 >= obj->x) && (p1 < (obj->x + obj->X)) &&
                         (p2 >= obj->y) && (p2 < (obj->y + obj->Y)) )
            return(z+1);
           }

        }

    }
    return(0);
}


/*
 *  return the color of the ground 
 *  if a ground image is defined, takes the color from the image
 *  otherwise, return the color of the corresponding targetarea (0.0 outside target areas)
 */
float
read_ground_color(struct individual *pind)
{

    double distzone;
    int z;
    float p1;
    float p2;
        struct envobject *obj;
    int   pi1;
    int   pi2;

    //calculate location of floor sensor 
    switch (robottype)
           {
         case 0:
          p1 = SetX(pind->pos[0], 0.0, pind->direction);
          p2 = SetY(pind->pos[1], 0.0, pind->direction);
              break;
         case 1:
          p1 = SetX(pind->pos[0], 37.0, pind->direction);
          p2 = SetY(pind->pos[1], 37.0, pind->direction);
              break;
         case 2:
          p1 = SetX(pind->pos[0], 70.0, pind->direction);
          p2 = SetY(pind->pos[1], 70.0, pind->direction);
              break;
           }
    // if present, always check the first image
    if (envnobjs[IMAGE] > 0)
    {
         obj = *(envobjs + IMAGE);
     pi1 = p1;
     pi2 = p2;
     if (((pi1 > 0) && (pi1 < obj->iw)) && ((pi2 > 0) && (pi2 < obj->iw))) 
       return(obj->i[pi1+(pi2*obj->iw)]);
     else
       return(0.0);
    }

    if (envnobjs[GROUND] > 0)
    {
      for(z=0, obj = *(envobjs + GROUND); z < envnobjs[GROUND]; z++, obj++)
        {
          if (obj->subtype == 0)
           {
            distzone = ((p1 - obj->x) * (p1 - obj->x))+((p2 - obj->y) * (p2 - obj->y));
            distzone = sqrt(distzone);
            if (distzone < obj->r)
            return(obj->c[0]);
           }
          else
           {
            if ( (p1 >= obj->x) && (p1 < (obj->x + obj->X)) &&
                         (p2 >= obj->y) && (p2 < (obj->y + obj->Y)) )
            return(obj->c[0]);
           }
        }
    }
    return(0.0);
}



/*
 * checks whether a crash occurred between a robot and an obstacle
 * for robot-robot collision individual_crash is set to 1
 */
int
check_crash(float p1,float p2)

{
  int i;
  float   x,y;
  double  dist;
  double  mindist;
  struct individual *pind;
  struct envobject *obj;

  individual_crash = 0;
  mindist = 9999.0;
  for(i=0, obj = *(envobjs + SWALL);i<envnobjs[SWALL];i++, obj++)
   {
    if (p1 >= obj->x && p1 <= obj->X)
       x = p1;
     else
       x = obj->x;
    if (p2 >= obj->y && p2 <= obj->Y)
       y = p2;
     else
       y = obj->y;
    dist = mdist(p1,p2,x,y);
    if (dist < mindist)
      mindist = dist;
   }
  for(i=0,obj = *(envobjs + SROUND);i<envnobjs[SROUND];i++,obj++)
   {
    dist = mdist(p1,p2,obj->x,obj->y)-obj->r;
    if (dist < mindist)
      mindist = dist;
   }
  for(i=0,obj = *(envobjs + ROUND);i<envnobjs[ROUND];i++,obj++)
   {
    dist = mdist(p1,p2,obj->x,obj->y)-obj->r;
    if (dist < mindist)
      mindist = dist;
   }

   // check if a crash occurred with another individuals
   // by ignoring individuals that have the same idential positions - theirself
   for (pind=ind,i=0; i < nteam; i++, pind++)
      {
        if (p1 != pind->pos[0] || p2 != pind->pos[1])
        {
          dist = mdist(p1,p2,pind->pos[0],pind->pos[1])-robotradius;
          if (dist < mindist)
            {
              mindist = dist;
              if (dist < robotradius)
                individual_crash = 1;
            }
        }
      }

   if (mindist < robotradius)
     return(1);
   else
     return(0);
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
 * update the state of the infrared sensors 
 */
void
update_infrared_s(struct individual *pind)

{

    float  p1;
    float  p2;
    double direction;
    int    i,ii;
    float  x,y;
    double dist;
    double  angle;
    int     relang;
    int     reldist;
    int     *w;
    int     *o;
    double  ang;
    struct individual *cind;
    int    t;
        struct envobject *obj;



    p1        = pind->pos[0];
    p2        = pind->pos[1];
    direction = pind->direction;

    for(i=0;i<8;i++)
      pind->cifsensors[i] = 0.0f;

    // walls
    for(i=0, obj = *(envobjs + SWALL);i < envnobjs[SWALL];i++, obj++)
    {
      if (p1 >= obj->x && p1 <= obj->X)
         x = p1;
       else
         x = obj->x;
      if (p2 >= obj->y && p2 <= obj->Y)
         y = p2;
       else
         y = obj->y;
       dist = mdist((float)p1,(float)p2,(float)x,(float)y);
       if (dist < (wallcf[3] + robotradius + (wallcf[2] * wallcf[4])))     // min.dist + khepera rad. + max. dist
        {                     // 5 + robotradius + 40 = 70.5
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
          dist -= robotradius;          // khepera rad. = robotradius
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

    // round obstacles
    for(i=0,obj = *(envobjs + ROUND);i < envnobjs[ROUND];i++, obj++)
    {
        dist = mdist((float)p1,(float)p2,(float)obj->x,(float)obj->y) - obj->r;
//        ang = mang((int)p1,(int)p2,(int)obj->x,(int)obj->y,(double)dist+obj->r);
//        relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
        if (dist < (obstcf[3] + robotradius + (obstcf[2] * obstcf[4])))       // min.dist + khepera rad. + (dists*inter)
        {                     // 72.5 = 5 + robotradius + 40 = 72.5
            ang = mang((int)p1,(int)p2,(int)obj->x,(int)obj->y,(double)dist+obj->r);
            relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
            dist -= robotradius;          // khepera rad.
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


    // small round obstacles
    for(i=0, obj = *(envobjs + SROUND);i < envnobjs[SROUND]; i++, obj++)
    {
            dist = mdist((float)p1,(float)p2,(float)obj->x,(float)obj->y) - obj->r;
            if (dist < (sobstcf[3] + robotradius + (sobstcf[2] * sobstcf[4])))       // min.dist + khepera rad. + max. dist
            {                     // 5 + robotradius + 40 = 72.5

                ang = mang((int)p1,(int)p2,(int)obj->x,(int)obj->y,(double)dist+obj->r);
                relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
                dist -= robotradius;          // khepera rad. = robotradius
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

    // other robots based on sround
    // we used the sample of round obstacles assuming they are similar to robots
    for (t=0,cind=ind; t<nteam; t++, cind++)
    {
     if ((p1 != cind->pos[0] || p2 != cind->pos[1]))
     {
       dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]) - robotradius;
       if (dist < (obstcf[3] + robotradius + (obstcf[2] * obstcf[4])))
        {
          ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist+robotradius);
          relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
          dist -= robotradius;
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


}

/*
 * update the state of the infrared sensors
 * geometrical version
 */
void
update_ginfrared_s(struct individual *pind, struct  iparameters *pipar)

{
    
   int    s;
   int    w;
   float  ang;
   float  x2,y2;
   float  xi,yi;
   struct envobject *obj;
   double dist;
   float  val;

   for(s=0; s < pipar->ngifsensors; s++)
     pind->cifsensors[s] = 0.0f;

   for(s=0; s < pipar->ngifsensors; s++)
    {
     ang = pind->direction + pind->cifang[s];
     if (ang < 0) ang += 360.0;
     if (ang > 360) ang -= 360.0;
     ToPoint(pind->pos[0], pind->pos[1], (float) ang, 300.0 + robotradius, &x2, &y2);
     obj = *(envobjs + SWALL);
     for (w=0, obj = *(envobjs + SWALL); w < envnobjs[SWALL]; w++, obj++)
      {
       if (intersect_lines(pind->pos[0], pind->pos[1], x2, y2 , obj->x, obj->y, obj->X, obj->Y, &xi, &yi) == 1)
        {
         dist = PointsDist(pind->pos[0], pind->pos[1], xi, yi);
     if (dist < pipar->ifrange)
          { 
       val = 1.0 - (dist / (float) pipar->ifrange); 
           if (pind->cifsensors[s] < val)
             pind->cifsensors[s] = val;
          }
        }
      }

     obj = *(envobjs + ROUND);
     for (w=0, obj = *(envobjs + ROUND); w < envnobjs[ROUND]; w++, obj++)
      {
       if (intersect_line_circle(pind->pos[0], pind->pos[1], x2, y2 , obj->x, obj->y, obj->r, &xi, &yi) == 1)
        {
         dist = PointsDist(pind->pos[0], pind->pos[1], xi, yi);
     if (dist < pipar->ifrange)
          { 
       val = 1.0 - (dist / (float) pipar->ifrange); 
           if (pind->cifsensors[s] < val)
             pind->cifsensors[s] = val;
          }
        }
      }

     obj = *(envobjs + SROUND);
     for (w=0, obj = *(envobjs + SROUND); w < envnobjs[SROUND]; w++, obj++)
      {
       if (intersect_line_circle(pind->pos[0], pind->pos[1], x2, y2 , obj->x, obj->y, obj->r, &xi, &yi) == 1)
        {
         dist = PointsDist(pind->pos[0], pind->pos[1], xi, yi);
     if (dist < pipar->ifrange)
          { 
       val = 1.0 - (dist / (float) pipar->ifrange); 
           if (pind->cifsensors[s] < val)
             pind->cifsensors[s] = val;
          }
        }
      }


    }

}


/*
 * update the state of the light sensors 
 * simulate the 8 khepera light sensors on the basis of the sample file light.sam
 */
void
update_lights_s(struct individual *pind)

{

     float  p1;
     float  p2;
     double direction;
     int    i;
     int    ii;
     double ang;
     int    relang;
     double dist;
     int    reldist;
     int    *o;
     struct envobject *obj;

     p1        = pind->pos[0];
     p2        = pind->pos[1];
     direction = pind->direction;

     for(i=0;i<8;i++)
      pind->clightsensors[i] = 0.0f;

     for(i=0, obj = *(envobjs + LIGHT);i < envnobjs[LIGHT];i++, obj++)
       {
          dist = mdist((float)p1,(float)p2,(float) obj->x,(float)obj->y);
          ang = mang((int)p1,(int)p2,(int)obj->x,(int)obj->y,(double)dist);
          relang = (int) mangrel2((double)ang,(double)direction) / (int) 2.0;
          dist  -= robotradius;
              if (obj->type > 0)                  // sensitivity magnifier
          dist = dist / (float) (obj->type + 1);
          if (dist <= (lightcf[3] + robotradius + (lightcf[2] * lightcf[4])))    // 560 = 13 + 27 + 520 (26*20)
        {
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
      for(i = 0; i < 8; i++)
      {
         if (pind->clightsensors[i] > 1.0)
           pind->clightsensors[i] = 1.0f;
      }
      // noise
      if (sensornoise > 0.0)
      for(i=0; i < 8; i++)
        {
          pind->clightsensors[i] += rans(sensornoise);
          if (pind->clightsensors[i] < 0.0) 
         pind->clightsensors[i] = (float) 0.0;
          if (pind->clightsensors[i] > 1.0) 
         pind->clightsensors[i] = (float) 1.0;
        }

}


/*
 * it calculate the relative angle for glight sensor
 */
double poliang(int angle)
{
    if (angle >= -20 && angle <= 20)
        return 1.0;

    else return (0.00000000018191734262*pow((double) angle,6.0) - 0.00000004852851366209*pow((double) angle,5.0) + 0.00000468165116765746*pow((double) angle,4.0) - 0.00019034299096084900*pow((double) angle,3.0) + 0.00266021760149552000*pow((double) angle,2.0) - 0.01106785972970260000*angle + 0.99841895404367100000);
}

/*
 * it calculate the relative distance for glight sensor
 */
double polidist(int distance)
{    
    distance = distance / 10; //this function has been calculated with cm, evorobot uses mm
    return (-0.00000000000427527288*pow((double) distance,5.0) + 0.00000000402975389212*pow((double) distance,4.0) - 0.00000145552017761533*pow((double) distance,3.0) + 0.00025832184800756300*pow((double) distance,2.0) - 0.02482502395931800000*distance + 1.24574587313730000000);
}

/*
 * update the state of the light sensors
 * simulate the interaction betwee 2 lego light sensors mounted at +- 45 degrees 
 * with an incandescent light of 60W ?
 */
void
update_glights_s(struct individual *pind, struct  iparameters *pipar)

{
     float  p1;
     float  p2;
     double direction;
     int    i;
     int    ii;
     int    iii;
     double ang;    
     double angs;
     double angs2;
     int    relang;
     double dist;
     int    reldist;
     int    *o;
     struct envobject *obj;
     int    gliang;
     int    glidang; 

     gliang   = -30;  // relative angle of the first sensor
     glidang  = 60;   // angular interval between two sensors

     p1        = pind->pos[0];
     p2        = pind->pos[1];
     direction = pind->direction;
     //ngli      = pipar->glightsensors;

     
     for(i=0;i<8;i++)
      pind->clightsensors[i] = 0.0f;

     for(i=0, obj = *(envobjs + LIGHT);i < envnobjs[LIGHT];i++, obj++)
       {
         for (iii=0; iii<pipar->lightsensors; iii++)  
        {
          dist = mdist((float)p1,(float)p2,(float) obj->x,(float)obj->y); //absolute distance
          ang = mang((int)p1,(int)p2,(int)obj->x,(int)obj->y,(double)dist); //absolute angle        
          angs = direction + gliang + (glidang * iii); 
          if (angs < 0) angs += 360;
          if (angs >= 360) angs -= 360;
          if (ang == angs) relang = 0;
            else relang = ang - angs;
          dist  -= robotradius;
                  // we assume that sensitivy saturate for distance below 100m 
          if (dist < 100)
            dist = 100; 

          if (relang >= -90 && relang <= 90) // we assume that sensitivity decrease to 0.0 outside the 90°/-90° range 
            pind->clightsensors[iii] += (float) (poliang(abs(relang))*polidist(dist)); 

          if (pind->clightsensors[iii] > 1.0) 
            pind->clightsensors[iii] = (float) 1.0;

        }                        
     }
}

/*
 * update the state of signal sensors 
 */
void
update_signal_s(struct individual *pind, struct  iparameters *pipar)

{

    float p1;
    float p2;
    double direction;
    int i;
    int t;
    int s;
    int f;
    float fr;
    double dist;
    int relang;
    double ang;
    struct individual *cind;
    int nsensor;

    p1        = pind->pos[0];
    p2        = pind->pos[1];
    direction = pind->direction;

    for(i=0;i< pipar->signalss;i++)
     {
       pind->detected_signal[i][0] = 0.0f;
       pind->detected_signal[i][1] = 999999.0f;
     }
    for (t=0, cind=ind; t<nteam; t++, cind++)
    if (pind->n_team != cind->n_team)
     {
       dist = mdist((float)pind->pos[0],(float)pind->pos[1],(float) cind->pos[0],(float)cind->pos[1]) - robotradius;
           if (dist < (double) max_dist_attivazione)
           {
             if (pipar->signalss == 1)
             // non-directional signals
             {
               if (dist < pind->detected_signal[0][1])
                for(s=0;s < pipar->signalso; s++)
            {
                         pind->detected_signal[s][0] =  cind->signal_motor[s];  
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
                          pind->detected_signal[s*pipar->signalss+0][0] = cind->signal_motor[s]; 
                          pind->detected_signal[s*pipar->signalss+0][1] = (float) dist;
             }
               }
               if (relang > 45 && relang <= 135)
               {
                if (dist < pind->detected_signal[1][1])
                 for(s=0;s < pipar->signalso; s++)
             {
                          pind->detected_signal[s*pipar->signalss+1][0] = cind->signal_motor[s]; 
                          pind->detected_signal[s*pipar->signalss+1][1] = (float) dist;
             }
               }
               if (relang > 135 && relang <= 225)
               {
                if (dist < pind->detected_signal[2][1])
                 for(s=0;s < pipar->signalso; s++)
             {
                          pind->detected_signal[s*pipar->signalss+2][0] = cind->signal_motor[s]; 
                          pind->detected_signal[s*pipar->signalss+2][1] = (float) dist;
             }
               }
               if (relang > 225 && relang <= 315)
               {
                if (dist < pind->detected_signal[3][1])
                 for(s=0;s < pipar->signalso; s++)
             {
                          pind->detected_signal[s*pipar->signalss+3][0] = cind->signal_motor[s]; 
                          pind->detected_signal[s*pipar->signalss+3][1] = dist;
             }
               }
             }

           }

        }
    for(s=0,nsensor=0;s < pipar->signalss*pipar->signalso; s++, nsensor++)
        {
                 pind->csignals[nsensor] = pind->detected_signal[s][0];
         if (pipar->signalssb == 1)
                  if (pind->csignals[nsensor] > 0.5)
                    pind->csignals[nsensor] = 1.0;
          else
                    pind->csignals[nsensor] = 0.0;
         for (f=0,fr = 0.0;f < pipar->signalssf; f++, fr += (1.0 / (float) pipar->signalssf))
           if (pind->csignals[nsensor] >= fr && pind->csignals[nsensor] < (fr + (1.0 / (float) pipar->signalssf)))
                     pind->csignals[nsensor+1+f] = 1.0;
           else
                     pind->csignals[nsensor+1+f] = 0.0;
        }

}


/*
 * update the camera characterized by
 *  a given cameraviewangle (e.g. +/- 18 degrees)
 *  a given number of camerareceptors for each cameracolor (1,2, or 3)
 *   with non-overlapping recepitive field
 *  a sensitivity up to a given cameradistance
 */
void
update_camera(struct individual *pind, struct  iparameters *pipar)

{

    int i;
    int ii;
    int f;
    int ff;
    int c;
    int r;
    int a1,a2;
    float x_cam;
    float y_cam;
    float dir_cam;
    float objxa, objya;
    float objxb, objyb;
    double dist;
    double dista;
    double distb;
    double ang;
    double anga;
    double angb;
    double ang1, ang2;
    double angs;
    double angr;
    double a;
    double da;
    float  av[3];
    struct envobject *obj;
    struct envobject **cenvobjs;
    float  objax, objay;
    float  objbx, objby;
    float  cameradist[360];
    char   message[256];
    int    npixels;
    struct individual *cind;
    int    skip;

    //initialize 1-degree photoreceptors state
    for(c=0; c < pipar->cameracolors; c++)
     for(f=0; f < pipar->cameraviewangle; f++)
     {
      pind->camera[c][f] = 0.0;
      cameradist[f] = 99999.0;
     }

    // camera located on the barycentre of the robot
    x_cam    = pind->pos[0];
    y_cam    = pind->pos[1];
    dir_cam  = pind->direction;

    cenvobjs = envobjs;
    // we only check round and sround objects
    for (i=0; i < NOBJS; i++)       
     {
      if (i == 0 || i == 1 || i == 2 || i == 5)
      for (ii=0, obj = *(cenvobjs + i); ii < envnobjs[i]; ii++, obj++)
       {
        skip = 0;
        // robots objects 
        if (obj->type == ROBOT)
      if (ii < nteam && ii != pind->n_team)
       {
         cind = (ind + ii);
             obj->x = cind->pos[0];
             obj->y = cind->pos[1];
             obj->r = robotradius;
           }
      else
       {
             skip = 1;
           }
         // compute the distance between the camera and the object
         if (obj->type == SWALL)
        dist = (mdist(x_cam, y_cam, obj->x, obj->y) + mdist(x_cam, y_cam, obj->X, obj->Y)) / 2.0;
       else
        dist = mdist(x_cam, y_cam, obj->x, obj->y);
     if (dist < pipar->cameradistance)
      {
          // cylindrical surface
      if (obj->type == SROUND || obj->type == ROUND || obj->type == ROBOT)
       {
            ang = mang((int) x_cam,(int) y_cam, (int) obj->x, (int) obj->y, dist);
        objxa = SetX(obj->x, obj->r, (double) ang + 90.0);
            objya = SetY(obj->y, obj->r, (double) ang + 90.0);
        dista = mdist(x_cam,y_cam,objxa,objya);  
        anga  = mang((int)x_cam,(int)y_cam,(int)objxa,(int)objya,dista);
        angs  = fabs(ang-anga);
        // the next line solve the limit problem
            if (angs > 180)
          angs = 360 - angs;
        angs = angs * 2;
        angr = ang - pind->direction;
        if (angr < 0)
         angr += 360;
        //sprintf(robotwstatus, "t%d abs %.0f abs2 %.0f dir %.0f span %.0f rel %.0f",obj->type, ang, anga, pind->direction, angs, angr);
           }

          // linear surface
      if (obj->type == SWALL)
       {
        dista  = mdist(x_cam, y_cam, obj->x, obj->y);
        distb  = mdist(x_cam, y_cam, obj->X, obj->Y);
            anga   = mang((int) x_cam,(int) y_cam, (int) obj->x, (int) obj->y, dista);
        angb   = mang((int) x_cam,(int) y_cam, (int) obj->X, (int) obj->Y, distb);
        angs  = fabs(angb-anga);
            if (angs > 180)
          angs = 360 - angs;
        if (((angb - anga) == angs) || (((angb + 360) - anga) == angs))
          ang = anga + (angs / 2);
         else
          ang = angb + (angs / 2);
        if (ang < 0) 
          ang += 360;
        angr = ang - pind->direction;
        if (angr < 0)
         angr += 360;
        //sprintf(robotwstatus, "t%d abs %.0f %.0f dir %.0f span %.0f a %.0f rel %.0f",obj->type, anga, angb, pind->direction, angs, ang, angr);
           }

       if (skip == 0)
       for(f=0, a = angr - (angs / 2); f < angs; f++, a += 1.0)
       {
            if (a < 0) a += 360;
        if (a > 360) a -= 360;
        ff = a;
        a1 = (360 - (pipar->cameraviewangle / 2));
            a2 = (pipar->cameraviewangle / 2);
        if (ff >= a1 && ff < 360 && dist < cameradist[ff-a1])
          for(c=0;c < pipar->cameracolors;c++)
           {
                pind->camera[c][ff-a1] = obj->c[c];
                cameradist[ff-a1] = dist;
           }
        if (ff >= 0 && ff < a2 && dist < cameradist[ff+a2])
          for(c=0;c < pipar->cameracolors;c++)
           {
                pind->camera[c][ff+a2] = obj->c[c];
				
                cameradist[ff+a2] = dist;
           }
       }

          } // dist < cameradistance
       }
     }
    // compress and average information for photoreceptors that encode more than one degree of view
    if (pipar->camerareceptors < pipar->cameraviewangle)
     for(f=0; f < pipar->camerareceptors; f++)
     {
       for(c=0; c < pipar->cameracolors; c++)
         av[c] = 0.0;
       npixels = pipar->cameraviewangle / pipar->camerareceptors;
       for(i=0; i < npixels; i++)
       {
        for(c=0; c < pipar->cameracolors; c++)
          av[c] += pind->camera[c][f*npixels+i];
       }
       for(c=0; c < pipar->cameracolors; c++)
         pind->camera[c][f] = av[c] / (float) npixels;
     }
     

}


/* 
 * Update lego nxt camera
 * Check for a Red blog
 */
void  update_legocam(struct individual *pind, struct  iparameters *pipar)
{

    int i;
    int ii;
    int f;
    int ff;
    int c;
    int r;
    int a1,a2;
    float x_cam;
    float y_cam;
    float dir_cam;
    float objxa, objya;
    float objxb, objyb;
    double dist;
    double dista;
    double distb;
    double ang;
    double anga;
    double angb;
    double ang1, ang2;
    double angs;
    double angr;
    double a;
    double da;
    float  av[3];
    struct envobject *obj;
    struct envobject **cenvobjs;
    float  objax, objay;
    float  objbx, objby;
    float  cameradist[360];
    char   message[256];
    int    npixels;
    struct individual *cind;
    int    skip;
    float  trackedBlob[8][4];

    //initialize 1-degree photoreceptors state
    for(c=0; c < pipar->cameracolors; c++)
     for(f=0; f < pipar->cameraviewangle; f++)
     {
      pind->camera[c][f] = 0.0;
      cameradist[f] = 99999.0;
     }

    // camera located on the barycentre of the robot
    x_cam    = pind->pos[0];
    y_cam    = pind->pos[1];
    dir_cam  = pind->direction;

    cenvobjs = envobjs;
    // we only check round and sround objects
    for (i=0; i < NOBJS; i++)       
     {
      if (i == 0 || i == 1 || i == 2 || i == 5)
      for (ii=0, obj = *(cenvobjs + i); ii < envnobjs[i]; ii++, obj++)
       {
        skip = 0;
        // robots objects 
        if (obj->type == ROBOT)
      if (ii < nteam && ii != pind->n_team)
       {
         cind = (ind + ii);
             obj->x = cind->pos[0];
             obj->y = cind->pos[1];
             obj->r = robotradius;
           }
      else
       {
             skip = 1;
           }
         // compute the distance between the camera and the object
         if (obj->type == SWALL)
        dist = (mdist(x_cam, y_cam, obj->x, obj->y) + mdist(x_cam, y_cam, obj->X, obj->Y)) / 2.0;
       else
        dist = mdist(x_cam, y_cam, obj->x, obj->y);
     if (dist < pipar->cameradistance)
      {
          // cylindrical surface
      if (obj->type == SROUND || obj->type == ROUND || obj->type == ROBOT)
       {
            ang   = mang((int) x_cam,(int) y_cam, (int) obj->x, (int) obj->y, dist);
        objxa = SetX(obj->x, obj->r, (double) ang + 90.0);
            objya = SetY(obj->y, obj->r, (double) ang + 90.0);
        dista = mdist(x_cam,y_cam,objxa,objya);  
        anga  = mang((int)x_cam,(int)y_cam,(int)objxa,(int)objya,dista);
        angs  = fabs(ang-anga);
        // the next line solve the limit problem
            if (angs > 180)
          angs = 360 - angs;
        angs = angs * 2;
        angr = ang - pind->direction;
        if (angr < 0)
         angr += 360;
        //sprintf(robotwstatus, "t%d abs %.0f abs2 %.0f dir %.0f span %.0f rel %.0f",obj->type, ang, anga, pind->direction, angs, angr);
           }

          // linear surface
      if (obj->type == SWALL)
       {
        dista  = mdist(x_cam, y_cam, obj->x, obj->y);
        distb  = mdist(x_cam, y_cam, obj->X, obj->Y);
            anga   = mang((int) x_cam,(int) y_cam, (int) obj->x, (int) obj->y, dista);
        angb   = mang((int) x_cam,(int) y_cam, (int) obj->X, (int) obj->Y, distb);
        angs   = fabs(angb-anga);
            if (angs > 180)
          angs = 360 - angs;
        if (((angb - anga) == angs) || (((angb + 360) - anga) == angs))
          ang = anga + (angs / 2);
         else
          ang = angb + (angs / 2);
        if (ang < 0) 
          ang += 360;
        angr = ang - pind->direction;
        if (angr < 0)
         angr += 360;
           }

       if (skip == 0)
       for(f=0, a = angr - (angs / 2); f < angs; f++, a += 1.0)
       {
            if (a < 0) a += 360;
        if (a > 360) a -= 360;
        ff = a;
        a1 = (360 - (pipar->cameraviewangle / 2));
            a2 = (pipar->cameraviewangle / 2);
        if (ff >= a1 && ff < 360 && dist < cameradist[ff-a1])
          for(c=0;c < pipar->cameracolors;c++)
           {
                pind->camera[c][ff-a1] = obj->c[c];
                cameradist[ff-a1] = dist;
           }
        if (ff >= 0 && ff < a2 && dist < cameradist[ff+a2])
          for(c=0;c < pipar->cameracolors;c++)
           {
                pind->camera[c][ff+a2] = obj->c[c];
                cameradist[ff+a2] = dist;
           }
       }

          } // dist < cameradistance
       }
     }
     // we initialize the size, the initial and final angle and the barycentre of red blobs 
     trackedBlob[0][0] = 0.0;  // size
     trackedBlob[0][1] = -1.0; // initial angle
     trackedBlob[0][2] = -1.0; // final angle
     trackedBlob[0][3] = 0.0;  // barycentre
     // red blob 
     // we compute the blob size,initial and final angle, and barycentre
     for(i=0;i<pipar->cameraviewangle;i++)
    {
      if(pind->camera[0][i] == 1.0) //red color
        {
          trackedBlob[0][0] += 1.0;  // blob size (relative to viewangle)
          if (trackedBlob[0][1] < 0.0) 
                trackedBlob[0][1] = i; // initial angle
          if (trackedBlob[0][2] < i)
                trackedBlob[0][2] = i; // final angle
        }
    }
      trackedBlob[0][0] = trackedBlob[0][0] / 42.0;
      trackedBlob[0][3] = trackedBlob[0][1] + ((trackedBlob[0][2] - trackedBlob[0][1]) / 2); // barycentre
      // we normalize 42 degree range over 176 pixels
      trackedBlob[0][3] = trackedBlob[0][3] / 42.0 * 176.0;  
      // we then compute the offset of the blob centre with respect to 58 and 118 pixels (+- 7.15 degrees)
      if (trackedBlob[0][0] > 0.0)
       {
        pind->camera[0][0] = 1.0-(abs(58.0-trackedBlob[0][3]) / 60.0);
    if (pind->camera[0][0] < 0.0)
         pind->camera[0][0] = 0.0;
        pind->camera[0][1] = 1.0-(abs(118.0-trackedBlob[0][3]) / 60.0);
    if (pind->camera[0][1] < 0.0)
         pind->camera[0][1] = 0.0;
        pind->camera[0][2] = trackedBlob[0][0];
       }
      else
       {
        pind->camera[0][0] = 0.0;
        pind->camera[0][1] = 0.0;
        pind->camera[0][2] = 0.0;
       }
      //sprintf(robotwstatus, "camera ang [%.1f %.1f] [%.1f] [%.1f %.1f] size %.1f ",trackedBlob[0][1], trackedBlob[0][2],trackedBlob[0][3], abs(58.0-trackedBlob[0][3])/60.0, abs(118.0-trackedBlob[0][3])/60.0, trackedBlob[0][0]);   
}


/*
 * update the simple camera 
 * the simple camera detect the other robot only
 */
void
update_simplecamera(struct individual *pind, struct  iparameters *pipar)

{

    int    i;
    float  p1;
    float  p2;
    double direction;
    int    t;
    double dist;
    double ang;
    int    relang;
    struct individual *cind;

    p1        = pind->pos[0];
    p2        = pind->pos[1];
    direction = pind->direction;

    for (i=0; i < 5; i++)
      pind->scamera[i] = 0.0;

    if (pipar->simplevision == 1)
    {
     for (t=0,cind=ind; t<nteam; t++, cind++)
     {
      if (pind->n_team != cind->n_team)
      {
       dist = mdist((float)p1,(float)p2,(float) cind->pos[0],(float)cind->pos[1]);
       ang = mang((int)p1,(int)p2,(int)cind->pos[0],(int)cind->pos[1],(double)dist);
       relang = (int) mangrel2((double)ang,(double)direction);
       if (relang >= 342 && relang <= 351)
              pind->scamera[0] = 1.0;
       if (relang > 351 && relang <= 360)
              pind->scamera[1] = 1.0;
       if (relang >= 0 && relang <= 9)
              pind->scamera[2] = 1.0;
       if (relang > 9 && relang <= 18)
              pind->scamera[3] = 1.0;
       if (relang > 18 && relang < 342)
              pind->scamera[4] = 1.0;
      }
     }
    }
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

                pind->scamera[0] = input_left;
                pind->scamera[1] = input_right;
                pind->scamera[2] = input_not_visible;
            }
        }
    }
}


/*
 * update the ground sensors
 * for groundsensor=1 set the analog value
 * for groundsensors>1 activate the neuron of the corresponding interval
 */
void
update_groundsensors(struct individual *pind, struct  iparameters *pipar)

{

    int g;
    int i;
    float c;
    float fi;
    float v;

    c = read_ground_color(pind);
    if (pipar->groundsensor == 1)
     {
       pind->ground[0] = c;
     }
    else
     {
      fi = 1.0 / (float) (pipar->groundsensor + 1);
      for (i=0, v=fi; i < pipar->groundsensor; i++, v += fi)
        if (c > v && c <= (v + fi))
         pind->ground[i] = (float) 1.0;
        else
         pind->ground[i] = (float) 0.0;
     }

}

/*
 * update the ground sensors encoding the last visited area
 */
void
update_lastground(struct individual *pind, struct  iparameters *pipar)

{

    int g;
    int i;
    float c;
    float fi;
    float v;

    c = read_ground_color(pind);
    fi = 1.0 / (float) (pipar->a_groundsensor2 + 1);
    if (c > fi)
     {
      for (i=0, v=fi; i < pipar->a_groundsensor2; i++, v += fi)
       if (c > v && c <= (v + fi))
         pind->lastground[i] = (float) 1.0;
        else
         pind->lastground[i] = (float) 0.0;
     }
    
}

/*
 * update the leaky-integrator ground sensors 
 */
void
update_deltaground(struct individual *pind, struct  iparameters *pipar)

{

    int g;
    int i;
    float c;
    float fi;
    float v;

    c = read_ground_color(pind);
    fi = 1.0 / (float) (pipar->a_groundsensor + 1);
    for (i=0, v=fi; i < pipar->a_groundsensor; i++, v += fi)
      if (c > v && c <= (v + fi))
         pind->ground[i] = pind->ground[i] * 0.99 + 0.01;
      else
         pind->ground[i] = pind->ground[i] * 0.99;

}

/*
* update the retina
/*/
void update_retina(struct individual *pind, struct  iparameters *pipar)
{
	//variable to retrieve small objects
	
	struct envobject *obj;
	int    ta,tt;
	float px,py;
	float obcounter;
	
	int r,c;
	float dr=(float)ipar->cRetinaSize/(float)ipar->retina;

	//resetting camera	
	pind->retinaColorReceptor=0.0;
	int cf=(pipar->retina-1)/2;
	for(c=0;c<pipar->retina;c++)
				for(r=0;r<pipar->retina;r++)
					pind->retinaMatrix[c][r]=0;

			 for(int c=0;c<ipar->retina;c++)
			  for(int r=0;r<ipar->retina;r++)
			  {
				 
				  px=ipar->cRetinaX-ipar->cRetinaSize/2.0+c*dr+dr/2.0;
				  py=ipar->cRetinaY-ipar->cRetinaSize/2.0+r*dr+dr/2.0;
				  obcounter=0;
				  //now reading all objects and computing the distance between receptors
				  for (ta=0,obj = *(envobjs + SROUND); ta<envnobjs[SROUND]; ta++, obj++)
					{
						
						float dist, gdist, prova;
						dist=mdist(px, py, obj->x, obj->y);
						
						gdist=gaussianActivation(dist);
						if(dist<ipar->cRetinaReceptiveField)
						{
						pind->retinaMatrix[c][r]+=obj->c[0];//gdist;
						obcounter+=1.0;
						}
							//if (pind->retinaMatrix[c][r]>1.0) pind->retinaMatrix[c][r]=1.0;

						if(c==cf && r == cf && obj->c[1]==1) // cioè stiamo al centro e l'oggetto è colorato
						{
							if(dist<ipar->cRetinaReceptiveField)
							pind->retinaColorReceptor=obj->c[1];//+=gdist;
						}
			
						
					}
				 if(obcounter>0)
				  pind->retinaMatrix[c][r]=pind->retinaMatrix[c][r]/obcounter;
				  if(pind->retinaColorReceptor>1.0) pind->retinaColorReceptor=1.0;


			  }
		
			  if(sensornoise>0)
			  for(c=0;c<pipar->retina;c++)
				for(r=0;r<pipar->retina;r++)
					pind->retinaMatrix[c][r]+=rans(sensornoise);
	//computing the distance between every object an receptors

}

extern void   update_laserScan(struct individual *pind, struct  iparameters *pipar)
{
	//to do: laser activation
   int    s;
   int    w;
   float  ang;
   float  x2,y2;
   float  xi,yi;
   struct envobject *obj;
   double dist;
   float  val=0;

	pind->laserPos=((cos((float)run/pipar->laserSpeed)+1))/2.0;

	ToPoint(pind->pos[0], pind->pos[1], pind->direction+pind->laserPos*2*ipar->laserScanRange-ipar->laserScanRange, pipar->laserLength + robotradius, &x2, &y2);
     obj = *(envobjs + SWALL);
     for (w=0, obj = *(envobjs + SWALL); w < envnobjs[SWALL]; w++, obj++)
      {
       if (intersect_lines(pind->pos[0], pind->pos[1], x2, y2 , obj->x, obj->y, obj->X, obj->Y, &xi, &yi) == 1)
        {
         dist = PointsDist(pind->pos[0], pind->pos[1], xi, yi);
     if (dist < pipar->laserLength+robotradius)
          { 
       val = 1.0 - (dist-robotradius) / (float)pipar->laserLength; 
          
          }
        }
      }
	  pind->laserSensor=val;
}

float gaussianActivation(float val)
{
	
	double res;
	res=(val*val)/(2*pow( ipar->cRetinaReceptiveField,2));
	return (float)exp(-res);
}
float gaussianActivation(float val, float var)
{
	double res;
	res=(val*val)/(2*pow( var,2));
	return (float)exp(-res);
}
/*
 * move the robot 
 */
void move_robot(struct individual *pind)
 {

     int    v1,v2;
     float  **mo;
     float  *m;
     float  ang, dist;
     extern float mod_speed;
     extern int   selected_ind_mod_speeed;

     double anglVari;             //Variation of angle as a consequence of movement
     double roboPivoDist;         //Distance from robot's perimeter to pivot of movement
     double roboCentPivoDist;     //Distance from robot's centre to pivot of movement
     double rightLegForc;      //right speed
     double leftLegForc;      //left speed
     double PI=3.1415926535;
     double PGRE_HALF=PI/2.0;
     double bodyRadi=57.5;      //distance between the two wheels 
     double bodyDiam=115;         //body diameter
     double maxSpeed=14.4;        //max speed in mm/s
     double angl, oldAngl;

//char buffer[123];
//sprintf(buffer, "robottype %d ", robottype);
//display_stat_message(buffer);

     switch(robottype)
     {
       case KHEPERA:
     pind->oldpos[0] = pind->pos[0];
     pind->oldpos[1] = pind->pos[1];
     pind->oldirection = pind->direction;

     v1 = (int) (pind->wheel_motor[0] * 20.0);
     v2 = (int) (pind->wheel_motor[1] * 20.0);
     // approximate to the closest integer value
     if ( ((pind->wheel_motor[0] * 200) - (v1 * 20)) > 5)
         v1++;
     if ( ((pind->wheel_motor[1] * 200) - (v2 * 20)) > 5)
          v2++;

     v1 -= 10;
     v2 -= 10;

     if (pind->wheel_motor[2] < 1.0f)
     {
      v1 = (int) ((float) v1 * pind->wheel_motor[2]);
      v2 = (int) ((float) v2 * pind->wheel_motor[2]);
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
     break;
       case EPUCK:
		   maxSpeed=0.5;
     /*
	 pind->oldpos[0] = pind->pos[0];
     pind->oldpos[1] = pind->pos[1];
     pind->oldirection = pind->direction;
     v1 = (int) (pind->wheel_motor[0] * 20.0);
     v2 = (int) (pind->wheel_motor[1] * 20.0);
     // approximate to the closest integer value
     if ( ((pind->wheel_motor[0] * 200) - (v1 * 20)) > 5)
         v1++;
     if ( ((pind->wheel_motor[1] * 200) - (v2 * 20)) > 5)
          v2++;

     v1 -= 10;
     v2 -= 10;

     if (pind->wheel_motor[2] < 1.0f)
     {
      v1 = (int) ((float) v1 * pind->wheel_motor[2]);
      v2 = (int) ((float) v2 * pind->wheel_motor[2]);
     }

     pind->speed[0] = (float) v1;
     pind->speed[1] = (float) v2;

     mo = motor;
     m = *(mo + (v1 + 12));
     m = (m + ((v2 + 12) * 2));
     ang = *m;
     m++;
     dist = *m * (float) 10.0;

	 ///inizio modifiche per fermare i motori
     pind->pos[0] =  SetX(pind->pos[0],dist,pind->direction);
     pind->pos[1] =  SetY(pind->pos[1],dist,pind->direction);


     pind->direction+= ang;
	 //fine modifiche
     if (pind->direction >= 360.0)
         pind->direction -= 360;
     if (pind->direction < 0.0)
         pind->direction += 360;
		 */
		   pind->oldpos[0] = pind->pos[0];
    pind->oldpos[1] = pind->pos[1];
    pind->oldirection = pind->direction;
    rightLegForc=pind->wheel_motor[0]*2*maxSpeed-maxSpeed;
    leftLegForc =pind->wheel_motor[1]*2*maxSpeed-maxSpeed;
    pind->speed[0] = rightLegForc;
    pind->speed[1] = leftLegForc;
        angl=DGRtoRAD(pind->direction);
        oldAngl=angl;
        if (abs(rightLegForc - leftLegForc) < 0.00001f)  //Speed of two wheels is very similar
         {
           pind->pos[0]  += cos(angl) * rightLegForc;
           pind->pos[1]  += sin(angl) * rightLegForc;
         }
         else
         {
           if (leftLegForc < rightLegForc)   
            {
              anglVari         = (rightLegForc - leftLegForc) / bodyDiam;
              angl             += anglVari;
              roboPivoDist     = leftLegForc / anglVari;
              roboCentPivoDist = roboPivoDist + bodyRadi;
              pind->pos[0]     += (cos(angl - PGRE_HALF) - cos(oldAngl - PGRE_HALF)) * roboCentPivoDist;
              pind->pos[1]     += (sin(angl - PGRE_HALF) - sin(oldAngl - PGRE_HALF)) * roboCentPivoDist;
            }
            else   
            {
              anglVari         = (leftLegForc - rightLegForc) / bodyDiam;
              angl             -= anglVari;
              roboPivoDist     = rightLegForc / anglVari;
              roboCentPivoDist = roboPivoDist + bodyRadi;
              pind->pos[0]            += (cos(angl + PGRE_HALF) - cos(oldAngl + PGRE_HALF)) * roboCentPivoDist;
              pind->pos[1]            += (sin(angl + PGRE_HALF) - sin(oldAngl + PGRE_HALF)) * roboCentPivoDist;
            }
         }
         if (PI*2 < angl)
           angl -= PI*2;
         if (angl < 0.0f)
           angl += PI*2;
         pind->direction=RADtoDGR(angl);
     break;
       case LEGO:
    pind->oldpos[0] = pind->pos[0];
    pind->oldpos[1] = pind->pos[1];
    pind->oldirection = pind->direction;
    rightLegForc=pind->wheel_motor[0]*2*maxSpeed-maxSpeed;
    leftLegForc =pind->wheel_motor[1]*2*maxSpeed-maxSpeed;
    pind->speed[0] = rightLegForc;
    pind->speed[1] = leftLegForc;
        angl=DGRtoRAD(pind->direction);
        oldAngl=angl;
        if (abs(rightLegForc - leftLegForc) < 0.00001f)  //Speed of two wheels is very similar
         {
           pind->pos[0]  += cos(angl) * rightLegForc;
           pind->pos[1]  += sin(angl) * rightLegForc;
         }
         else
         {
           if (leftLegForc < rightLegForc)   
            {
              anglVari         = (rightLegForc - leftLegForc) / bodyDiam;
              angl             += anglVari;
              roboPivoDist     = leftLegForc / anglVari;
              roboCentPivoDist = roboPivoDist + bodyRadi;
              pind->pos[0]     += (cos(angl - PGRE_HALF) - cos(oldAngl - PGRE_HALF)) * roboCentPivoDist;
              pind->pos[1]     += (sin(angl - PGRE_HALF) - sin(oldAngl - PGRE_HALF)) * roboCentPivoDist;
            }
            else   
            {
              anglVari         = (leftLegForc - rightLegForc) / bodyDiam;
              angl             -= anglVari;
              roboPivoDist     = rightLegForc / anglVari;
              roboCentPivoDist = roboPivoDist + bodyRadi;
              pind->pos[0]            += (cos(angl + PGRE_HALF) - cos(oldAngl + PGRE_HALF)) * roboCentPivoDist;
              pind->pos[1]            += (sin(angl + PGRE_HALF) - sin(oldAngl + PGRE_HALF)) * roboCentPivoDist;
            }
         }
         if (PI*2 < angl)
           angl -= PI*2;
         if (angl < 0.0f)
           angl += PI*2;
         pind->direction=RADtoDGR(angl);
       break;
	   case EYEBOT:
		   pind->direction = -90 + pind->wheel_motor[0]*90-45;
		   break;
	   case RETINABOT:
		   //in this case we have an horizontal and a vertical direction
		    //we asave in memory previous position for drawing purpouses
		    ipar->cRetinaYold=ipar->cRetinaY;
			ipar->cRetinaXold=ipar->cRetinaX;
			ipar->cRetinaSizeold=ipar->cRetinaSize;
		   
		   if(ipar->retinaMotorControlType==0)
		   {
		   ipar->cRetinaX=ipar->retinaX+pind->wheel_motor[0]*ipar->retinaXrange*2.0-ipar->retinaXrange;
		   ipar->cRetinaY=ipar->retinaY+pind->wheel_motor[1]*ipar->retinaYrange*2.0-ipar->retinaYrange;
		   }

		   if(ipar->retinaMotorControlType==1)
		   {
		   ipar->cRetinaX+=pind->wheel_motor[0]*18-9;//18-9
		   ipar->cRetinaY+=pind->wheel_motor[1]*18-9;
		   if(ipar->cRetinaX>ipar->retinaX+ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX+ipar->retinaXrange;
		   if(ipar->cRetinaX<ipar->retinaX-ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX-ipar->retinaXrange;
		   if(ipar->cRetinaY>ipar->retinaY+ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY+ipar->retinaYrange;
		   if(ipar->cRetinaY<ipar->retinaY-ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY-ipar->retinaYrange;
		   }
		   //ipar->cRetinaX=ipar->retinaX+50;
		   //ipar->cRetinaY=ipar->retinaY+50;
		      if(ipar->retinaMotorControlType==2) //3 motors required
			{
				float ax=pind->wheel_motor[0]*10-5;
				float ay=pind->wheel_motor[1]*10-5;
				float ang=atan2(ay,ax);
				ipar->cRetinaX+=45*pind->wheel_motor[2]*cos(ang);
				ipar->cRetinaY+=45*pind->wheel_motor[2]*sin(ang);
				if(ipar->cRetinaX>ipar->retinaX+ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX+ipar->retinaXrange;
		   if(ipar->cRetinaX<ipar->retinaX-ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX-ipar->retinaXrange;
		   if(ipar->cRetinaY>ipar->retinaY+ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY+ipar->retinaYrange;
		   if(ipar->cRetinaY<ipar->retinaY-ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY-ipar->retinaYrange;

			 }
			  		      if(ipar->retinaMotorControlType==3) //3 motors required
			{
				float ax=pind->wheel_motor[0]*10-5;
				float ay=pind->wheel_motor[1]*10-5;
				float ang=atan2(ay,ax);
				ipar->cRetinaX=90*pind->wheel_motor[2]*cos(ang)+ipar->retinaX;
				ipar->cRetinaY=90*pind->wheel_motor[2]*sin(ang)+ipar->retinaX;
				if(ipar->cRetinaX>ipar->retinaX+ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX+ipar->retinaXrange;
		   if(ipar->cRetinaX<ipar->retinaX-ipar->retinaXrange) ipar->cRetinaX=ipar->retinaX-ipar->retinaXrange;
		   if(ipar->cRetinaY>ipar->retinaY+ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY+ipar->retinaYrange;
		   if(ipar->cRetinaY<ipar->retinaY-ipar->retinaYrange) ipar->cRetinaY=ipar->retinaY-ipar->retinaYrange;

			 }
			if(ipar->retinaMotorControlType==4) //4 motors required
			{
				ipar->cRetinaX=ipar->retinaX+pind->wheel_motor[0]*ipar->retinaXrange-ipar->retinaXrange*pind->wheel_motor[1];
			    ipar->cRetinaY=ipar->retinaY+pind->wheel_motor[2]*ipar->retinaYrange-ipar->retinaYrange*pind->wheel_motor[3];
			 }
			if(ipar->retinaMotorControlType==5) //5parametrized per number of motors
			{
				float ax=0;
				float ay=0;
				float dalfa=(2*PI)/(float)ipar->wheelmotors;
				float calfa;
				for(int i=0;i<ipar->wheelmotors;i++)
				{
					calfa=i*dalfa;
					ax+=pind->wheel_motor[i]*cos(calfa)*ipar->retinaXrange;
					ay+=pind->wheel_motor[i]*sin(calfa)*ipar->retinaYrange;

				}
				ipar->cRetinaX=ipar->retinaX+ax;
			    ipar->cRetinaY=ipar->retinaY+ay;
			 }

			if(ipar->retinaMotorControlType==10)
			{
				if(run%100==0)
				{
				ipar->cRetinaX=ipar->retinaX+rans(90);
			    ipar->cRetinaY=ipar->retinaY+90;
				}
			}

			if(ipar->retinaMotorControlType==20) //muscle control requires 4 motors
			{
				if(xMuscle!=NULL && yMuscle!=NULL)
				{
				xMuscle->setActivation(pind->wheel_motor[0],pind->wheel_motor[1]);
				yMuscle->setActivation(pind->wheel_motor[2],pind->wheel_motor[3]);
				ipar->cRetinaX+=xMuscle->apply()*0.2;
				ipar->cRetinaY+=yMuscle->apply()*0.2;
				}

			}
		
		   if(ipar->retinaZoomMotor==1)
		   {
			  ipar->cRetinaSize=(ipar->retinaSize-30)*pind->zoom_motor[0]+30; // ipar->cRetinaSize=(330)*pind->zoom_motor[0]+20;
		   }
		   //if(ipar->retinaZoomMotor>1)
			//   ipar->cRetinaReceptiveField=(ipar->retinaReceptiveField-5)*pind->zoom_motor[1]+5;
		   if(ipar->retinaZoomMotor==2 && ipar->retinaMotorControlType==20)
		   {
			   //zoom muscle control
			   if(zMuscle!=NULL)
			   {
			   zMuscle->setActivation(pind->zoom_motor[0],pind->zoom_motor[1]);
			   ipar->cRetinaSize+=zMuscle->apply()*0.2;
			   }

		   }

		   break;

    }
   // toroidal world
    if (pind->pos[0] < 0.0)
      pind->pos[0] = envdx;
    if (pind->pos[0] > envdx)
      pind->pos[0] = 0.0;
    if (pind->pos[1] < 0.0)
      pind->pos[1] = envdy;
    if (pind->pos[1] > envdy)
      pind->pos[1] = 0.0;

 }

/*
 *  we chech whether the gripper causes a collision 
 *  and we handle grasp and release actions
 */
int
move_gripper(struct individual *pind)

{

   float   headx,heady; 
   struct  envobject *obj;
   float   objdist;
   int     crash;
   int     i;
   float   xx,yy;
   int    grippable;
   int    idgrippable;
   double gbackdir;
   double gleftdir;
   double grightdir;     
   float  gheadx,gheady;
   float  gbackx,gbacky;     
   float  gleftx,glefty;          
   float  grightx,grighty;
   float  gflx,gfly;
   float  gfrx,gfry;                  
   float  gfdeltax,gfdeltay;
   float  gbdeltax,gbdeltay;     
   float  gblx,gbly; 
   float  gbrx,gbry;
   float  gopen;
   float  lgripper[4];
   float  rgripper[4]; 
   float  vgripper[4]; 
   int    whilen;
   float  xi,yi;


     crash = 0;
     grippable = 0;
     idgrippable = 0;

     gbackdir = pind->direction + 180.0;
     if (gbackdir > 359.0) gbackdir -= 360.0;
     gleftdir = pind->direction + 90.0;
     if (gleftdir > 359.0) gleftdir -= 360.0;
     grightdir = pind->direction - 90.0;
     if (grightdir < 0.0) grightdir += 360.0; 

     // DEBUG: originally the robot moved back only if treshould was not
     //        exceeded last time. Moreover instead of setting
     //        the motors to 0,0 we how move it back of 10mm
     if (pind->gripper_motor[0] > 0.75)
     {
      pind->pos[0] = SetX(pind->pos[0],10,gbackdir);
      pind->pos[1] = SetY(pind->pos[1],10,gbackdir);      
     }         

     headx  = SetX(pind->pos[0],65.0f,pind->direction);
     heady  = SetY(pind->pos[1],65.0f,pind->direction);

     gheadx  = SetX(pind->pos[0],42.5f,pind->direction);
     gheady  = SetY(pind->pos[1],42.5f,pind->direction);
     gbackx  = SetX(pind->pos[0],27.5f,gbackdir);
     gbacky  = SetY(pind->pos[1],27.5f,gbackdir);     
     grightx = SetX(pind->pos[0],35.0f,grightdir);
     grighty = SetY(pind->pos[1],35.0f,grightdir);
     gleftx  = SetX(pind->pos[0],35.0f,gleftdir);
     glefty  = SetY(pind->pos[1],35.0f,gleftdir);          
     
     gfdeltax = (gheadx - pind->pos[0]);
     gfdeltay = (gheady - pind->pos[1]);          
     gbdeltax = (pind->pos[0] - gbackx);
     gbdeltay = (pind->pos[1] - gbacky);               
     gflx  =  gleftx + gfdeltax;   
     gfly  =  glefty + gfdeltay;        
     gfrx  =  grightx + gfdeltax;   
     gfry  =  grighty + gfdeltay;                  
     gblx  =  gleftx - gbdeltax;   
     gbly  =  glefty - gbdeltay;        
     gbrx  =  grightx - gbdeltax;   
     gbry  =  grighty - gbdeltay;                  
        
     if (pind->grip_obtaken == 1)
       gopen = 14.0;
     else
       gopen = 25.0;

     lgripper[0] = SetX(gheadx,gopen,gleftdir);
     lgripper[1] = SetY(gheady,gopen,gleftdir);
     lgripper[2] = SetX(lgripper[0],35.0,pind->direction);
     lgripper[3] = SetY(lgripper[1],35.0,pind->direction);
     rgripper[0] = SetX(gheadx,gopen,grightdir);
     rgripper[1] = SetY(gheady,gopen,grightdir);
     rgripper[2] = SetX(rgripper[0],35.0f,pind->direction);
     rgripper[3] = SetY(rgripper[1],35.0f,pind->direction);
     vgripper[0] = gheadx; 
     vgripper[1] = gheady;
     vgripper[2] = SetX(vgripper[0],22.0f,pind->direction);
     vgripper[3] = SetY(vgripper[1],22.0f,pind->direction);
     // we check whether the gripper crashed with an sround obstacles
     if (pind->grip_obtaken == 0 && pind->gripper_motor[0] > 0.75)
      for(i=0, obj = *(envobjs + SROUND);i < envnobjs[SROUND]; i++, obj++)
      {
      if (mdist(headx,heady,obj->x,obj->y) < 65)
       {
        if (intersect_line_circle(lgripper[0],lgripper[1],lgripper[2],lgripper[3], obj->x, obj->y, (obj->r + 2), &xi, &yi) == 1)
          crash = 1;
        if (intersect_line_circle(rgripper[0],rgripper[1],rgripper[2],rgripper[3], obj->x, obj->y, (obj->r + 2), &xi, &yi) == 1)
          crash = 1;
        if (intersect_line_circle(gflx,gfly,gfrx,gfry, obj->x, obj->y, obj->r, &xi, &yi) == 1)
          crash = 1;
        if (intersect_line_circle(vgripper[0],vgripper[1],vgripper[2],vgripper[3], obj->x, obj->y, obj->r, &xi, &yi) == 1)
       {
            grippable = 1;
        idgrippable = i;
           }
        }
     }
     // we chech whether the arm crashes agaist walls (only handle orthogonal walls)
     if (crash == 0 && pind->gripper_motor[0] > 0.75 && pind->grip_obtaken == 0)
      for(i=0, obj = *(envobjs + SWALL);i < envnobjs[SWALL]; i++, obj++)
      {
        if (headx >= obj->x && headx <= obj->X)
            xx = headx;
           else
            xx = obj->x;
          if (heady >= obj->y && heady <= obj->Y)
            yy = heady;
       else
            yy = obj->y;
          if  (mdist(headx,heady,xx,yy) < 40.0)
         crash = 1;
        }

        // we check whether the robot tries and succeeds in grasping the object
        // the object is taken and a new object is placed near the old object
        if (crash == 0 && pind->gripper_motor[0] > 0.75 && pind->grip_obtaken == 0 && grippable == 1)
         { 
          pind->grip_obtaken = 1;
      obj = *(envobjs + SROUND);
      obj = (obj + idgrippable);
      obj->x = headx + mrand(120) - 60;
      obj->y = heady + mrand(120) - 60;
      whilen = 0;
          while (check_crash(pind->pos[0],pind->pos[1])== 1 && whilen < 100)
         {
           obj->x = headx + mrand(120) - 60;
           obj->y = heady + mrand(120) - 60;
           whilen++;
          }
         }
        // we check whether the robot releases the object
        if (pind->gripper_motor[1] > 0.75 && crash == 0 && pind->grip_obtaken == 1) 
      {
        pind->grip_obtaken = 0;
        if ((headx < 0 || headx > envdx) || (heady < 0 || heady > envdy))
          pind->grip_obreleased += 1; 
          }

        return(crash);
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
    if ((fp = fopen("../bin/sample_files/motor.sam","r")) != NULL)

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
 * allocate space for sampled objects
 */
void
create_world_space()

{
    int     **wa;
    int        **ob;
    float    **mo;
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



