// epuckSerComm.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include "mode.h"
#ifdef EVOREALWIN

	#include "windows.h"
	#include "time.h"

#endif


#include "defs.h"







//variabili relative alla connessione seriale e ai robot
struct robot robots[7];

#ifdef EVOREALWIN
	HANDLE		 hSerialPort[7];  //up to 7 connection
	DCB			 dcbSerialParams={0};
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

#ifdef EVOREALWIN

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
	dcbSerialParams.Parity	 = NOPARITY;

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

#ifdef EVOREALWIN

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
#ifdef EVOREALWIN
	CloseHandle(hSerialPort[rbt->serialConnection]);
	rbt->connected=0;
#endif

return 0;
}



int queryRobotsSensors()
{

#ifdef EVOREALWIN

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

#ifdef EVOREALWIN

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
#ifdef EVOREALWIN

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
