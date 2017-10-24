/* 
 * Evorobot* - epuck.h
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
#include "mode.h"

//epuck.h contains all the stuff for serial communication
#ifdef EVOWINDOWS //Windows mode
#include <stdio.h>
#include "windows.h"
#include "time.h"
//funzioni;
char queryCommand[10];
char sensorsBuffer[4000];	//conterrà tutti i sensori richiesti all'epuck
int openSerialPort(int noPort); 

int connectRobot(struct robot rbt, int comPort);
int disconnectRobot(struct robot rbt);
int queryRobotSensors(struct robot rbt);
int setRobotSpeed(struct robot rbt, int speedL, int speedR);

int closeSerialPort(int noPort);
int querySensors();			//richiesta e lettura dal buffer dei valori dell'epuck
int setRobotSpeed(int rbIndex, int speedL, int speedR);
int intFrom2Bytes(char a, char b);

struct robot
{
	int connected;
	//Comm.
	HANDLE hSerial;
	DCB	dcbSerialParams;
	COMMTIMEOUTS comt;
	//Sensors
	int IR[8]; //infrared sensors 'N'
	int LI[8]; //light sensors    'O'
	int FL[3]; //floor sensors    'M'
	int AC[3]; //acceleration s.  'A'
	char videoBuffer[1600];//image 'I'
	char audioBuffer[800];
	char sensorsBuffer[50];		   // do not include sound and camera readings; 	
	int camW; //cam width         'X cam_mode,cam_widht, cam_heigth,cam_zoom :expressed in byte
	int camH; //cam height			
	int camZ; //cam zoom

} robots[7]; //since 7 is the maximum bluetooth serial port allowed.
#endif