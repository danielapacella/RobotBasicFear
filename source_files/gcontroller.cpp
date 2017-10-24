/*
 * Evorobot* - gcontroller.cpp
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
 * robot-env.cpp
 */

#include "public.h"
#include "epuck.h"

/*
 * Move the robot through a behaviour based controller
 */
void
update_bcontroller(struct individual *pind, struct  iparameters *pipar)

{

  double target_a;
  double target_d;
  float groundc;

  // predator
  // turn toward the prey and move forward
  // move backword and turns outside foraging area (ground > 0.25 && < 0.75) or after a collision
  if (pind->controlmode = 1)
  {
   groundc = read_ground_color(pind);
   if (groundc > 0.25 && groundc < 0.75 && pind->jcrashed == 0)
    {
     target_d = mdist(pind->pos[0],pind->pos[1],ind->pos[0],ind->pos[1]);
     target_a = mang((int) pind->pos[0],(int)pind->pos[1],(int)ind->pos[0],(int)ind->pos[1],target_d);
     target_a = mangrel2(target_a,pind->direction);
     if (target_a < 180)
       pind->direction += 10;
     else
       pind->direction -= 10;
     if (pind->direction < 0)
       pind->direction += 360;
     if (pind->direction > 360)
       pind->direction -= 360;
     pind->wheel_motor[0] = 0.65;
     pind->wheel_motor[1] = 0.65;
     pind->wheel_motor[2] = 1.0;
     move_robot(pind);
    }
   else
    {
     pind->wheel_motor[0] = 0.0;
     pind->wheel_motor[1] = 0.0;
     pind->wheel_motor[2] = 1.0;
     move_robot(pind);
     pind->direction += mrand(90) + 5;
     if (pind->direction > 360)
       pind->direction -= 360;
    }

  }

}