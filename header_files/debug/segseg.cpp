/*
This code is described in "Computational Geometry in C" (Second Edition),
Chapter 7.  It is not written to be comprehensible without the 
explanation in that book.

Compile:  gcc -o segseg segseg.c (or simply: make)

Written by Joseph O'Rourke.
Last modified: November 1997
Questions to orourke@cs.smith.edu.
--------------------------------------------------------------------
This code is Copyright 1998 by Joseph O'Rourke.  It may be freely 
redistributed in its entirety provided that this copyright notice is 
not removed.
--------------------------------------------------------------------
*/
//#include <stdio.h>
//#include <math.h>
#include "defs.h"
#include "public.h"
#define	EXIT_FAILURE 1

//typedef	enum { FALSE, TRUE } bool;

#define DIM 2


/*---------------------------------------------------------------------
Function prototypes.
---------------------------------------------------------------------*/
int 	SegSegInt( tPointi a, tPointi b, tPointi c, tPointi d, tPointd p );
char	ParallelInt( tPointi a, tPointi b, tPointi c, tPointi d, tPointd p );
bool	Between( tPointi a, tPointi b, tPointi c );
void	Assigndi( tPointd p, tPointi a );
int	Collinear( tPointi a, tPointi b, tPointi c );
int     AreaSign( tPointi a, tPointi b, tPointi c );
/*-------------------------------------------------------------------*/

int X = 0;
int Y = 1;

/*
 * return the relative angle between the robot direction and a line
 */
int intersect(float posx, float posy, double dir, float maxd, float *line2)
{
   double  s, t;       /* The two parameters of the parametric eqns. */
   double  num, denom;  /* Numerator and denoninator of equations. */
   int     code;
   tPointi a; 
   tPointi b; 
   tPointi c; 
   tPointi d; 
   tPointd p;
   double  dist;
   
   a[X] = posx;
   a[Y] = posy;
   b[X] = SetX(posx, maxd, dir);
   b[Y] = SetY(posy, maxd, dir);
   c[X] = *line2;
   c[Y] = *(line2 + 1);
   d[X] = *(line2 + 2);
   d[Y] = *(line2 + 3);

   denom = a[X] * (double)( d[Y] - c[Y] ) +
           b[X] * (double)( c[Y] - d[Y] ) +
           d[X] * (double)( b[Y] - a[Y] ) +
           c[X] * (double)( a[Y] - b[Y] );

   /* If denom is zero, then segments are parallel: handle separately. */
   if (denom == 0.0)
      return  ParallelInt(a, b, c, d, p);

   num =    a[X] * (double)( d[Y] - c[Y] ) +
            c[X] * (double)( a[Y] - d[Y] ) +
            d[X] * (double)( c[Y] - a[Y] );
   if ( (num == 0.0) || (num == denom) ) code = 2;
   s = num / denom;

   num = -( a[X] * (double)( c[Y] - b[Y] ) +
            b[X] * (double)( a[Y] - c[Y] ) +
            c[X] * (double)( b[Y] - a[Y] ) );
   if ( (num == 0.0) || (num == denom) ) code = 2;
   t = num / denom;

   if      ( (0.0 < s) && (s < 1.0) &&
             (0.0 < t) && (t < 1.0) )
     code = 1;
   else if ( (0.0 > s) || (s > 1.0) ||
             (0.0 > t) || (t > 1.0) )
     code = 0;

   p[X] = a[X] + s * ( b[X] - a[X] );
   p[Y] = a[Y] + s * ( b[Y] - a[Y] );

   sprintf(drawdata_st,"intersection between wall1 and wall2: %f %f, dist %.2f code %d ",p[X], p[Y], dist, code);
   display_stat_message(drawdata_st);

   return code;
}



/*---------------------------------------------------------------------
SegSegInt: Finds the point of intersection p between two closed
segments ab and cd.  Returns p and a char with the following meaning:
   'e': The segments collinearly overlap, sharing a point.
   'v': An endpoint (vertex) of one segment is on the other segment,
        but 'e' doesn't hold.
   '1': The segments intersect properly (i.e., they share a point and
        neither 'v' nor 'e' holds).
   '0': The segments do not intersect (i.e., they share no points).
Note that two collinear segments that share just one point, an endpoint
of each, returns 'e' rather than 'v' as one might expect.
---------------------------------------------------------------------*/

int SegSegInt(tPointi a, tPointi b, tPointi c, tPointi d, tPointd p )
{
   double  s, t;       /* The two parameters of the parametric eqns. */
   double  num, denom;  /* Numerator and denoninator of equations. */
   int     code;

   denom = a[X] * (double)( d[Y] - c[Y] ) +
           b[X] * (double)( c[Y] - d[Y] ) +
           d[X] * (double)( b[Y] - a[Y] ) +
           c[X] * (double)( a[Y] - b[Y] );

   /* If denom is zero, then segments are parallel: handle separately. */
   if (denom == 0.0)
      return  ParallelInt(a, b, c, d, p);

   num =    a[X] * (double)( d[Y] - c[Y] ) +
            c[X] * (double)( a[Y] - d[Y] ) +
            d[X] * (double)( c[Y] - a[Y] );
   if ( (num == 0.0) || (num == denom) ) code = 2;
   s = num / denom;

   num = -( a[X] * (double)( c[Y] - b[Y] ) +
            b[X] * (double)( a[Y] - c[Y] ) +
            c[X] * (double)( b[Y] - a[Y] ) );
   if ( (num == 0.0) || (num == denom) ) code = 2;
   t = num / denom;

   if      ( (0.0 < s) && (s < 1.0) &&
             (0.0 < t) && (t < 1.0) )
     code = 1;
   else if ( (0.0 > s) || (s > 1.0) ||
             (0.0 > t) || (t > 1.0) )
     code = 0;

   p[X] = a[X] + s * ( b[X] - a[X] );
   p[Y] = a[Y] + s * ( b[Y] - a[Y] );

   return code;
}


char	ParallelInt( tPointi a, tPointi b, tPointi c, tPointi d, tPointd p )
{
   if ( !Collinear( a, b, c) )
      return '0';

   if ( Between( a, b, c ) ) {
      Assigndi( p, c );
      return 'e';
   }
   if ( Between( a, b, d ) ) {
      Assigndi( p, d );
      return 'e';
   }
   if ( Between( c, d, a ) ) {
      Assigndi( p, a );
      return 'e';
   }
   if ( Between( c, d, b ) ) {
      Assigndi( p, b );
      return 'e';
   }
   return '0';
}

void	Assigndi( tPointd p, tPointi a )
{
   int i;
   for ( i = 0; i < DIM; i++ )
      p[i] = a[i];
}

/*---------------------------------------------------------------------
Returns TRUE iff point c lies on the closed segement ab.
Assumes it is already known that abc are collinear.
---------------------------------------------------------------------*/
bool    Between( tPointi a, tPointi b, tPointi c )
{

   /* If ab not vertical, check betweenness on x; else on y. */
   if ( a[X] != b[X] )
      return ((a[X] <= c[X]) && (c[X] <= b[X])) ||
             ((a[X] >= c[X]) && (c[X] >= b[X]));
   else
      return ((a[Y] <= c[Y]) && (c[Y] <= b[Y])) ||
             ((a[Y] >= c[Y]) && (c[Y] >= b[Y]));
}

int Collinear( tPointi a, tPointi b, tPointi c )
{
   return AreaSign( a, b, c ) == 0;
}


int     AreaSign( tPointi a, tPointi b, tPointi c )
{
    double area2;

    area2 = ( b[0] - a[0] ) * (double)( c[1] - a[1] ) -
            ( c[0] - a[0] ) * (double)( b[1] - a[1] );

    /* The area should be an integer. */
    if      ( area2 >  0.5 ) return  1;
    else if ( area2 < -0.5 ) return -1;
    else                     return  0;
}
