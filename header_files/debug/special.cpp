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

#include "public.h"
#include "defs.h"

#ifdef EVOWINDOWS 
#include <windows.h>     // for Sleep function
#else
#include <unistd.h>      // for Sleep function
#endif


/*
 * return a random float between -n and + n
 */ 
float
rans(double n)
	
{


#ifdef EVOWINDOWS 
   static	double max = 32767;
   return ((float) (n - (2.0 * n *((double) rand() / max))));
#else
   return 2.0*n*((double)rand()/(double)RAND_MAX) - n;
#endif

}


/*
 * return an integer between 0 and (i-1)
 */
int
mrand(int i)


{
//#ifdef EVOWINDOWS 
//	extern int rand();
//#endif
	int r;

	r = rand();
	r = r % (int) i;
	return r;
}


/*
 * set the seed for the random number generator
 */
void set_seed(int s)

{
   srand(s);
}

/*
 * pause for n ms
 */
void pause(int n)

{
  #ifdef EVOREALWIN
  Sleep(n);
  #endif
}




/*
 * play sound (mode1 play, mode0 close the midi device)
 */
void playsound(struct individual *pind, int mode)

{ 


} 


