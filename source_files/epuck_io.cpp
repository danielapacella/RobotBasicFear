/* 
 * Evorobot* - epuck_io.h
 * Copyright (C) 2009, Onofrio Gigliotta 
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

#include <stdio.h>
#include "public.h"
#include "epuck.h"

#ifdef EVOWINDOWS
    #include "windows.h"
    #include "time.h"
#endif


struct robot
{
    int connected;
    int serialConnection; //refers to HANDLE vector hSerialPort
    int port;

    //Sensors
    int IR[8]; //infrared sensors 'N'
    int LI[8]; //light sensors    'O'
    int FL[3]; //floor sensors    'M'
    int AC[3]; //acceleration s.  'A'
    int MC[3]; //microphone       'U'
    int MR[6]; //microretina //cam is dived in 6 sector an then for each sector the number of pixel of a target color is computed
    char videoBuffer[1600]; //img 'I'
    int imageRGB[1600];     //rgb image built upon videobuffer
    char sensorsBuffer[60];
    int camW; //cam width         'X cam_mode,cam_widht, cam_heigth,cam_zoom :expressed in byte
    int camH; //cam height
    int camZ; //cam zoom
    //used by  Joachim
    int old_floor_input[2]; //floor input from previous timestep
    int    floor_mean[3];        //mean floor value from previous timesteps
    int target_prototype[3];//prototypes of floorsensor values for [0]=no target(white), [1]=target1(grey), [2]=target2(black)
    int    epuck_angle_old;    //angle of other epuck at previous timestep
    float old_input[3];        //camera input from previous timestep


};

//variabili relative alla connessione seriale e ai robot
struct robot robots[7];

#ifdef EVOWINDOWS
    HANDLE         hSerialPort[7];  //up to 7 connection
    DCB             dcbSerialParams={0};
    COMMTIMEOUTS comt;

// fine



//stat
clock_t startTime;
clock_t endTime;
#endif
int dtime;



int intFrom2Bytes(char a, char b)
{
    int res;
    res=(a & 0xff) | (b & 0xff) << 8;
    return res;
}
// function to be used in evorobot
int initRobots()
{
    int id;
    for(id=0;id<7;id++)
    {
        robots[id].serialConnection=id;
        robots[id].port=-1;
        robots[id].connected=0;
    }
return 0;
}
int connectRobot(struct robot *rbt, int comPort)
{

#ifdef EVOWINDOWS

    char portname[20];
    //sprintf(portname,"COM%d",comPort);
    sprintf(portname,"\\\\.\\COM%d",comPort);
    hSerialPort[rbt->serialConnection] = CreateFileA(portname,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0);
    if (hSerialPort[rbt->serialConnection] == INVALID_HANDLE_VALUE) return -1;



    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
    if (!GetCommState(hSerialPort[rbt->serialConnection], &dcbSerialParams)) return -2;

    dcbSerialParams.BaudRate = 115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity     = NOPARITY;

    comt.ReadIntervalTimeout = 500;
    SetCommTimeouts( hSerialPort[rbt->serialConnection], &comt);
    if (!SetCommState(hSerialPort[rbt->serialConnection], &dcbSerialParams)) return -3;

    rbt->port=comPort;
    rbt->connected=1;

#endif

    return 0;
}
int setRobotSpeed(struct robot *rbt, int speedL, int speedR)
{

#ifdef EVOWINDOWS

    DWORD dwBytesReadWrite = 0;
    char command[5];

    command[0]='D';
    command[1]=speedL & 0xff;
    command[2]=speedL >> 8;
    command[3]=speedR & 0xff;
    command[4]=speedR >>8;

    if (!WriteFile(hSerialPort[rbt->serialConnection],command,5,&dwBytesReadWrite,NULL)) return -1;

#endif

    return 0;


}
int disconnectRobot(struct robot *rbt)
{
#ifdef EVOWINDOWS
    CloseHandle(hSerialPort[rbt->serialConnection]);
    rbt->connected=0;
#endif

return 0;
}



int queryRobotsSensors()
{

#ifdef EVOWINDOWS

    DWORD dwBytesReadWrite = 0;
    char queryCommand[10];
    int i=0;
    int noRobot;
    //N: Infrared sensors O: Light sensors M: floorsensors I:image
    //quering sensors value
    for (noRobot=0;noRobot<7;noRobot++)
    {
        if(robots[noRobot].connected == 1)
        {

            if (!WriteFile(hSerialPort[noRobot],"NOM",4,&dwBytesReadWrite,NULL)) return -1;

        }
    }

    for (noRobot=0;noRobot<7;noRobot++)
    {
        if(robots[noRobot].connected ==1 )
        {

            if (!ReadFile(hSerialPort[noRobot],robots[noRobot].sensorsBuffer,44,&dwBytesReadWrite,NULL)) return -2;
                //in ordere to read cam wehave tu use a different query
            //decoding value
            for(i=0;i<8;i++)
            {
                //IR[i]=(sensorsBuffer[i*2] & 0xff) | (sensorsBuffer[i*2+1] & 0xff) << 8;
                robots[noRobot].IR[i]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);
            }
            for(i=8;i<16;i++)
            {
                robots[noRobot].LI[i-8]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);
            }
            for (i=16;i<19;i++)
            {
                robots[noRobot].FL[i-16]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2+1],robots[noRobot].sensorsBuffer[i*2]);

            }
            for (i=19;i<22;i++)
            {
                robots[noRobot].MC[i-19]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);

            }
        }
    }

#endif
    return 0;

}

int queryRobotsSensorsV()
{

#ifdef EVOWINDOWS

    DWORD dwBytesReadWrite = 0;
    char queryCommand[10];
    int i=0;
    int noRobot;
    int camType=1; //0: Linear camera mode.We use a 60x15 resolution on the epuck camera diveded by 6 sectors of 10x15, then for each sensor we compute the presence of a red blob in the x axes
                   //1: Joachim mode
    int noByteToRead=56;
    if (camType==1)noByteToRead=46;
    //N: Infrared sensors O: Light sensors M: floorsensors and Microphones
    //quering sensors value
    for (noRobot=0;noRobot<7;noRobot++)
    {
        if(robots[noRobot].connected == 1)
        {

            if (!WriteFile(hSerialPort[noRobot],"Z",2,&dwBytesReadWrite,NULL)) return -1;

        }
    }

    for (noRobot=0;noRobot<7;noRobot++)
    {
        if(robots[noRobot].connected ==1 )
        {

            if (!ReadFile(hSerialPort[noRobot],robots[noRobot].sensorsBuffer,noByteToRead,&dwBytesReadWrite,NULL)) return -2;
                //in ordere to read cam wehave tu use a different query
            //decoding value
            for(i=0;i<8;i++)
            {
                //IR[i]=(sensorsBuffer[i*2] & 0xff) | (sensorsBuffer[i*2+1] & 0xff) << 8;
                robots[noRobot].IR[i]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);
            }
            for(i=8;i<16;i++)
            {
                robots[noRobot].LI[i-8]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);
            }
            for (i=16;i<19;i++)
            {
                robots[noRobot].FL[i-16]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2+1],robots[noRobot].sensorsBuffer[i*2]);
            //for some reason that I do not known michel made troubles in big and little endian
            }
            for (i=19;i<22;i++)
            {
                robots[noRobot].MC[i-19]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);

            }
            for (i=22;i<28;i++)
            {
               robots[noRobot].MR[i-22]=intFrom2Bytes(robots[noRobot].sensorsBuffer[i*2],robots[noRobot].sensorsBuffer[i*2+1]);

            }
        }
    }

#endif
    return 0;

}

int printRobotSensors(struct robot *rbt)
{
    printf("Robot no:%d\r\n",rbt->serialConnection);

    printf("IR: %d %d %d %d %d %d %d %d\r\n",
        rbt->IR[0],rbt->IR[1],rbt->IR[2],rbt->IR[3],
        rbt->IR[4],rbt->IR[5],rbt->IR[6],rbt->IR[7]);

    printf("LI: %d %d %d %d %d %d %d %d\r\n",
        rbt->LI[0],rbt->LI[1],rbt->LI[2],rbt->LI[3],
        rbt->LI[4],rbt->LI[5],rbt->LI[6],rbt->LI[7]);

    printf("FL: %d %d %d\r\n",
        rbt->FL[0],rbt->FL[1],rbt->FL[2]);

    printf("MC: %d %d %d\r\n",
        rbt->MC[0],rbt->MC[1],rbt->MC[2]);

return 0;

}
int setRobotLed(struct robot *rbt,int noLed, int value)
{
#ifdef EVOWINDOWS

    DWORD dwBytesReadWrite = 0;
    char command[3];
    if (value<0)value=0; //on
    if (value>1)value=1; //off

    command[0]='L';
    command[1]= noLed;   //number of led to switch
    command[2]= value;  //0: off, 1: on

    if (!WriteFile(hSerialPort[rbt->serialConnection],command,3,&dwBytesReadWrite,NULL))
        return -1;

#endif
    return 0;


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
     for (i=0; i < pipar->wheelmotors; i++, nsensor++)
       pind->input[nsensor]= pind->wheel_motor[i];
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
                               pind->detected_signal[s][0] = cind->signal_motor[s]; 
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


//update the sensors value of all robots
void 
update_realsensors()

{
   
    if (ipar->simplevision > 0)
      queryRobotsSensorsV(); 
     else
      queryRobotsSensors(); 
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
 * connect or disconnect the robots through the bluetooth
 */
void connect_disconnect_robots()
{

    int id;
    if (real==0)
    {
     real=1;
     initRobots();
    }
    else
    {
     real=0;
     for (id=0;id<7;id++)
      {
         if (robots[id].connected==1)
        {
         setRobotSpeed(&robots[id], 0,0);//stop robots before disconnection
         disconnectRobot(&robots[id]);
        }

    }
    }


}
