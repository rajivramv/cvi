#include <utility.h>
#include <stdlib.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "traffic_signal.h"

#define NORTH 1
#define EAST 2
#define WEST 3
#define SOUTH 4

#define LEFT 1
#define RIGHT 2
#define STRAIGHT 3

#define NORTH_SOUTH_LEFT 0
#define NORTH_RIGHT 1
#define SOUTH_RIGHT 2
#define NORTH_SOUTH_STRAIGHT 3
#define EAST_WEST_LEFT 4
#define EAST_RIGHT 5
#define WEST_RIGHT 6
#define EAST_WEST_STRAIGHT 7

static int panelHandle; 

static double TimeKeeperSpeed = 1;
static int HH = 6;
static int MM = 0;
static int SS = 0;
static char Meridiem[3] = "AM";

static float NLeft = 0;
static float NRight = 0;
static float NStraight = 0;

static float ELeft = 0;
static float ERight = 0;
static float EStraight = 0;

static float WLeft = 0;
static float WRight = 0;
static float WStraight = 0;

static float SLeft = 0;
static float SRight = 0;
static float SStraight = 0;

unsigned int GreenSignalTime = 0;
unsigned int OrangeSignalTime = 0;

static int OldStatus = 0;
static int NewStatus = 0;
static int OrangeStatus = 0;
static int GreenStatus = 0;

void Vehicles (void);
void GreenTimer (void);
void GreenOn (void);
int OrangeTimer (void);
void OrangeOn (void);
void Default (void);
void ChangePic(void);

													/**************
													*Main function*
													**************/
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "traffic_signal.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	LoadMenuBar (panelHandle, "traffic_signal.uir", MainMenu);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

													/************
													*Time Keeper*
													************/

int CVICALLBACK TimeKeeper_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{			
	int i,j;
	char hh_s[3];
	char mm_s[3];
	char ss_s[3];
	//char time[15];
	switch (event)
		{
		case EVENT_TIMER_TICK:
			//Calculating time 
			SS++;										
			if (SS == 60)
			{
				MM++;
				SS = 0;
				if (HH == 6 && MM <20)				   //Changing background every minute between 06:00-06:20 AM/PM  
					ChangePic();
				if (MM == 60)
				{
					HH++;
					MM = 0;
					if (HH == 12 && strcmp(Meridiem,"AM") == 0)
						strcpy(Meridiem,"PM");
					else if (HH == 12 && strcmp(Meridiem,"PM") == 0)
						strcpy(Meridiem,"AM");
				}
			}
			if (HH == 13)
				HH = 1;
			sprintf (hh_s,"%02d",HH);	 //Conversion to string
			sprintf (mm_s,"%02d",MM);
			sprintf (ss_s,"%02d",SS);
			SetCtrlAttribute (panelHandle, PANEL_HH, ATTR_CTRL_VAL, hh_s);
			SetCtrlAttribute (panelHandle, PANEL_MM, ATTR_CTRL_VAL, mm_s);
			SetCtrlAttribute (panelHandle, PANEL_SS, ATTR_CTRL_VAL, ss_s);
			SetCtrlAttribute (panelHandle, PANEL_Meridiem, ATTR_CTRL_VAL, Meridiem);
						
			
			Vehicles(); 	   //Calculating vehicles 
		
								
								//Orange ON -> Orange delay complete -> Green ON -> Green delay to begin
										
			if(OrangeStatus == 2 && GreenStatus == 1)
			{	
				GreenTimer ();
				return 0;
			}
										//Orange ON -> Orange delay complete -> Green should ON 
											
			if(OrangeStatus == 2 && GreenStatus == 0)
			{
				GreenOn ();
			}
			
												//Orange ON -> Orange delay to begin
			
			if(OrangeStatus == 1 && GreenStatus == 0)
			{	
				OrangeTimer ();	
			}
			
														//Orange should ON
				
			if (OrangeStatus == 0 && GreenStatus == 0)
			{
				OrangeOn ();
			}
			break;
		}
	return 0;
}												/************
												*Vehicles ()*
												************/

void Vehicles (void)
{
	int dump;
	float N_left_traffic,N_right_traffic,N_straight_traffic;
	float E_left_traffic,E_right_traffic,E_straight_traffic;
	float W_left_traffic,W_right_traffic,W_straight_traffic;
	float S_left_traffic,S_right_traffic,S_straight_traffic;
	float vehicle_speed;
		
	GetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_CTRL_VAL, &dump);
	N_left_traffic = (float)1/dump;
	N_right_traffic = (float)1/dump;
	N_straight_traffic = (float)1/dump;
	
	GetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_CTRL_VAL, &dump);
	E_left_traffic = (float)1/dump;														//Getting the incoming traffic  and 
	E_right_traffic = (float)1/dump;													//converting it to vehicles/second.
	E_straight_traffic = (float)1/dump;
																														  
	GetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_CTRL_VAL, &dump);
	W_left_traffic = (float)1/dump;
	W_right_traffic = (float)1/dump;
	W_straight_traffic = (float)1/dump;
	
	GetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_CTRL_VAL, &dump);
	S_left_traffic = (float)1/dump;
	S_right_traffic = (float)1/dump;
	S_straight_traffic = (float)1/dump;
	
	GetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_CTRL_VAL, &dump);
	vehicle_speed = (float)1/dump;
	
	if(OrangeStatus == 2 && GreenStatus == 1)									 //Based on the green light that is ON,
	{																			 //subtracting the outgoing traffic (also 
		switch(NewStatus)														 //converted to vehicles/second.
		{
			case NORTH_SOUTH_LEFT:
				N_left_traffic -= vehicle_speed;
				S_left_traffic -= vehicle_speed;
				break;
			case NORTH_RIGHT:
				N_right_traffic -= vehicle_speed;
				break;
			case SOUTH_RIGHT:
				S_right_traffic -= vehicle_speed;
				break;
			case NORTH_SOUTH_STRAIGHT:
				N_straight_traffic -= vehicle_speed;
				S_straight_traffic -= vehicle_speed;
				break;
			case EAST_WEST_LEFT:
				E_left_traffic -= vehicle_speed;
				W_left_traffic -= vehicle_speed;
				break;
			case EAST_RIGHT:
				E_right_traffic -= vehicle_speed;
				break;
			case WEST_RIGHT:
				W_right_traffic -= vehicle_speed;
				break;
			case EAST_WEST_STRAIGHT:
				E_straight_traffic -= vehicle_speed;
				W_straight_traffic -= vehicle_speed;
				break;
		}
	}													
	
	//Calculating the vehicles and updating the numerics.
	NLeft += N_left_traffic;
	if(NLeft<0)
		NLeft = 0;
	SetCtrlAttribute (panelHandle, PANEL_NLeft, ATTR_CTRL_VAL,(int)NLeft);
	
	NRight += N_right_traffic;
	if(NRight<0)
		NRight = 0;
	SetCtrlAttribute (panelHandle, PANEL_NRight, ATTR_CTRL_VAL,(int)NRight);
	
	NStraight += N_straight_traffic;
	if(NStraight<0)
		NStraight = 0;
	SetCtrlAttribute (panelHandle, PANEL_NStraight, ATTR_CTRL_VAL,(int)NStraight);

	ELeft += E_left_traffic;
	if(ELeft<0)
		ELeft = 0;
	SetCtrlAttribute (panelHandle, PANEL_ELeft, ATTR_CTRL_VAL,(int)ELeft);

	ERight += E_right_traffic;
	if(ERight<0)
		ERight = 0;
	SetCtrlAttribute (panelHandle, PANEL_ERight, ATTR_CTRL_VAL,(int)ERight);

	EStraight += E_straight_traffic;
	if(EStraight<0)
		EStraight = 0;
	SetCtrlAttribute (panelHandle, PANEL_EStraight, ATTR_CTRL_VAL,(int)EStraight);
	
	WLeft += W_left_traffic;
	if(WLeft<0)
		WLeft = 0;
	SetCtrlAttribute (panelHandle, PANEL_WLeft, ATTR_CTRL_VAL,(int)WLeft);
	
	WRight += W_right_traffic;
	if(WRight<0)
		WRight = 0;
	SetCtrlAttribute (panelHandle, PANEL_WRight, ATTR_CTRL_VAL,(int)WRight);
	
	WStraight += W_straight_traffic;
	if(WStraight<0)
		WStraight = 0;
	SetCtrlAttribute (panelHandle, PANEL_WStraight, ATTR_CTRL_VAL,(int)WStraight);
	
	SLeft += S_left_traffic;
	if(SLeft<0)
		SLeft = 0;
	SetCtrlAttribute (panelHandle, PANEL_SLeft, ATTR_CTRL_VAL,(int)SLeft);
	  
	SRight += S_right_traffic;
	if(SRight<0)
		SRight = 0;
	SetCtrlAttribute (panelHandle, PANEL_SRight, ATTR_CTRL_VAL,(int)SRight);
	
	SStraight += S_straight_traffic;
	if(SStraight<0)
		SStraight = 0;
	SetCtrlAttribute (panelHandle, PANEL_SStraight, ATTR_CTRL_VAL,(int)SStraight);
}						


				
												/**************
												*GreenTimer ()*
												**************/
													
void GreenTimer (void)							  //Keeps decrementing GreenSignalTime everytime function 
{												  //is called and updates it in the corresponding numeric
	GreenSignalTime--;
	switch(NewStatus)
	{
			case NORTH_SOUTH_LEFT:
				SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
				SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case NORTH_RIGHT:
				SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
		      	break;
			case SOUTH_RIGHT:
				SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case NORTH_SOUTH_STRAIGHT:
				SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
				SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case EAST_WEST_LEFT:
				SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
				SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case EAST_RIGHT:
				SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case WEST_RIGHT:
				SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
			case EAST_WEST_STRAIGHT:
				SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
				SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
				break;
	}		
	if(GreenSignalTime == 0)
	{
		OrangeStatus = 0;
		GreenStatus = 0;
		OldStatus = NewStatus;
	}
}											   

												/********
												*GreenOn*
												********/

void GreenOn (void)
{
	switch(OldStatus)
	{		
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 1);
			break;
		case NORTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 1);
			break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 1);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 1);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 1);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 1);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 1);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 1);
			break;
	}
	switch(NewStatus)
	{
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nleft, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sleft, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;																 
		case NORTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nright, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sright, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Nstraight, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sstraight, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, GreenSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eleft, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Wleft, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eright, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wright, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Estraight, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Wstraight, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, GreenSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, GreenSignalTime);
			break;
	}
		GreenStatus = 1;
}

						
												/***************
												*OrangeTimer ()*
												***************/

int OrangeTimer (void)
{													 //Keeps decrementing OrangeSignalTime everytime function 
	if(OrangeSignalTime == 0)						 //is called and updates it in the corresponding numeric 
	{
		OrangeStatus = 2;
		return 0;
	}
	OrangeSignalTime--;
	switch(OldStatus)
	{		
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_RIGHT:
			 SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
	}
	switch(NewStatus)
	{
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
	      	break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
	}		
	if(OrangeSignalTime == 0)
	{
		OrangeStatus = 2;
	}
	return 0;
}   

									
													/***********
													*OrangeOn()*
													***********/

void OrangeOn (void)										 //Finds the directions with the maximum vehicles
{															//and decides which green light should turn ON next. 
	float vehicles[8];
	float max_vehicles;
	int i;
	char debug1[10],debug2[10];					
	
	vehicles[NORTH_SOUTH_LEFT] = (NLeft + SLeft)/2;
	vehicles[NORTH_RIGHT] = NRight;
	vehicles[SOUTH_RIGHT] = SRight;
	vehicles[NORTH_SOUTH_STRAIGHT] = (NStraight + SStraight)/2;
	vehicles[EAST_WEST_LEFT] = (ELeft + WLeft)/2;
	vehicles[EAST_RIGHT] = ERight;
	vehicles[WEST_RIGHT] = WRight;
	vehicles[EAST_WEST_STRAIGHT] = (EStraight + WStraight)/2;	
	
	max_vehicles = vehicles[NORTH_SOUTH_LEFT];
	
	for(i=NORTH_RIGHT;i<=EAST_WEST_STRAIGHT;i++)
	{
		if(max_vehicles < vehicles[i])
		{
			max_vehicles = vehicles[i];
		}
	}
	for(i=NORTH_SOUTH_LEFT;i<=EAST_WEST_STRAIGHT;i++)
	{	
		if (vehicles[i] == max_vehicles)
		{
			NewStatus =  i;
			break;
		}
	}
														
	GetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, &GreenSignalTime);		 //After deciding the direction
	OrangeSignalTime = 0.3 * GreenSignalTime;												 //corresponding orange lights are 
																							 //turned ON/OFF and corresponding
	switch(OldStatus)																		 //timers are updated.
	{		
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Nleft, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sleft, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
		case NORTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nright, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Sright, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nstraight, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sstraight, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Eleft, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wleft, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Eright, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Wright, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Estraight, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wstraight, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
	}
	switch(NewStatus)
	{
		case NORTH_SOUTH_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
		    break;
		case SOUTH_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case NORTH_SOUTH_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Nred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Sred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Norange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Sorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_LEFT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case WEST_RIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
		case EAST_WEST_STRAIGHT:
			SetCtrlAttribute (panelHandle, PANEL_Ered, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Wred, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (panelHandle, PANEL_Eorange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_Worange, ATTR_CTRL_VAL, 1);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_CTRL_VAL, OrangeSignalTime);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_CTRL_VAL, OrangeSignalTime);
			break;
	}
	OrangeStatus = 1; 
}
	
											/*************		
											*ChangePic ()*
											*************/
											
void ChangePic (void)												//Updates the background pic based on AM/PM
{
	int pic,visible;
	if (strcmp(Meridiem,"AM") == 0)
	{	
		pic = MM;
		visible = 0;
	}
	else if (strcmp(Meridiem,"PM") == 0)
	{
		pic = 20 - MM; 	
		visible = 1;
	}
	switch(pic)
	{
		case 1:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr1.jpg");
			break;
		case 2:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr2.jpg");
			break;
		case 3:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr3.jpg");
			break;
		case 4:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr4.jpg");
			break;
		case 5:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr5.jpg");
			break;	
		case 6:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr6.jpg");
			break;	
		case 7:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr7.jpg");
			break;
		case 8:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr8.jpg");
			break;
		case 9:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr9.jpg");
			break;
		case 10:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr10.jpg");
			break;
		case 11:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr11.jpg");
			break;
		case 12:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr12.jpg");
			break;
		case 13:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr13.jpg");
			break;
		case 14:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr14.jpg");
			break;
		case 15:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr15.jpg");
			break;
		case 16:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr16.jpg");
			break;
		case 17:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr17.jpg");
			break;
		case 18:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr18.jpg");
			break;
		case 19:
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr19.jpg");
			break;	
		default:
			break;
	}
}
											
						/*********************************************************************************
						*Control Buttons - Simulate and Pause, Inc.speed, Dec. speed, Stop, Mode, Default*
						*********************************************************************************/

int CVICALLBACK SimulateAndPause_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int state = 0;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_SimulateAndPause, ATTR_CTRL_VAL, &state);
			if(state == 1)
			{
				SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_INTERVAL, TimeKeeperSpeed);
				SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_ENABLED, 1);
			}
			else
				SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_ENABLED, 0);
			SetCtrlAttribute (panelHandle, PANEL_Stop, ATTR_DIMMED, 0);
			break;
		}
	return 0;
}

int CVICALLBACK IncSpeed_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char speed[7];
	switch (event)
		{
		case EVENT_COMMIT:
			TimeKeeperSpeed/=2;
			SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_INTERVAL, TimeKeeperSpeed);
			
			if (TimeKeeperSpeed == 16)
				strcpy(speed,"1/16 X");
			else if (TimeKeeperSpeed == 8)
			{
				strcpy(speed,"1/8 X");
				SetCtrlAttribute (panelHandle, PANEL_DecSpeed, ATTR_DIMMED, 0);
			}
			else if (TimeKeeperSpeed == 4)
				strcpy(speed,"1/4 X");
			else if (TimeKeeperSpeed == 2)
				strcpy(speed,"1/2 X");
			else if (TimeKeeperSpeed == 1)
				strcpy(speed,"1 X");
			else if (TimeKeeperSpeed == 0.5)
				strcpy(speed,"2 X");
			else if (TimeKeeperSpeed == 0.25)
				strcpy(speed,"4 X");
			else if (TimeKeeperSpeed == 0.125)
				strcpy(speed,"8 X");
			else if (TimeKeeperSpeed == 0.0625)
			{
				strcpy(speed,"16 X");	
				SetCtrlAttribute (panelHandle, PANEL_IncSpeed, ATTR_DIMMED, 1);
			}
			SetCtrlVal (panelHandle, PANEL_TimeKeeperSpeed, speed);
			
			break;
		}
	return 0;
}

int CVICALLBACK DecSpeed_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char speed[7];
	switch (event)
		{
		case EVENT_COMMIT:
			TimeKeeperSpeed*=2;
			SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_INTERVAL, TimeKeeperSpeed);
			
			if (TimeKeeperSpeed == 16)
			{	
				strcpy(speed,"1/16 X");
				SetCtrlAttribute (panelHandle, PANEL_DecSpeed, ATTR_DIMMED, 1);
			}
			else if (TimeKeeperSpeed == 8)
				strcpy(speed,"1/8 X");
			else if (TimeKeeperSpeed == 4)
				strcpy(speed,"1/4 X");
			else if (TimeKeeperSpeed == 2)
				strcpy(speed,"1/2 X");
			else if (TimeKeeperSpeed == 1)
				strcpy(speed,"1 X");
			else if (TimeKeeperSpeed == 0.5)
				strcpy(speed,"2 X");
			else if (TimeKeeperSpeed == 0.25)
				strcpy(speed,"4 X");
			else if (TimeKeeperSpeed == 0.125)
			{
				strcpy(speed,"8 X");
				SetCtrlAttribute (panelHandle, PANEL_IncSpeed, ATTR_DIMMED, 0);
			}
			else if (TimeKeeperSpeed == 0.0625)
				strcpy(speed,"16 X");	
				
			SetCtrlVal (panelHandle, PANEL_TimeKeeperSpeed, speed);
			break; 
		}
	return 0;
}

int CVICALLBACK Stop_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int mode;
	switch (event)
		{
		case EVENT_COMMIT:
			Default ();
			break;
		}
	return 0;
}

int CVICALLBACK Mode_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int mode;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_Mode, ATTR_CTRL_VAL, &mode);
			switch (mode)
			{
				case 0:
					Default ();
					SetCtrlAttribute (panelHandle, PANEL_NLeft, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_NStraight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_NRight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_ELeft, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_EStraight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_ERight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_WLeft, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_WStraight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_WRight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_SLeft, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_SStraight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_SRight, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_MAX_VALUE, 10);
					break;
				case 1:
					Default ();
					SetCtrlAttribute (panelHandle, PANEL_NLeft, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_NStraight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_NRight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_ELeft, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_EStraight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_ERight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_WLeft, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_WStraight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_WRight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_SLeft, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_SStraight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_SRight, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_MAX_VALUE, 120);
					break;
			}
			
			break;
		}
	return 0;
}
void Default (void)
{
			int mode,green_time;
			SetCtrlAttribute (panelHandle, PANEL_TimeKeeper, ATTR_ENABLED, 0);
			SetCtrlAttribute (panelHandle, PANEL_Stop, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_IncSpeed, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_DecSpeed, ATTR_DIMMED, 0);
			TimeKeeperSpeed = 1;
			HH = 6;
			MM = 0;
			SS = 0;
			strcpy(Meridiem,"AM");

			NLeft = 0;
			NRight = 0;
			NStraight = 0;

			ELeft = 0;
			ERight = 0;
			EStraight = 0;

			WLeft = 0;
			WRight = 0;
			WStraight = 0;

			SLeft = 0;
			SRight = 0;
			SStraight = 0;

			GreenSignalTime = 0;
			OrangeSignalTime = 0;

			OldStatus = 0;
			NewStatus = 0;
			OrangeStatus = 0;
			GreenStatus = 0;
			
			DisplayImageFile (panelHandle, PANEL_CrossRoad, "cr1.jpg"); 
			
			GetCtrlAttribute (panelHandle, PANEL_Mode, ATTR_CTRL_VAL, &mode);
			DefaultPanel (panelHandle);
			SetCtrlAttribute (panelHandle, PANEL_Mode, ATTR_CTRL_VAL, mode);
			
			GetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, &green_time);
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_MAX_VALUE, green_time);
}	
						
											  		/**************
											   		*User Numerics*
											   		**************/
int CVICALLBACK GreenTime_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int green_time,mode;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, &green_time);
			GetCtrlAttribute (panelHandle, PANEL_Mode, ATTR_CTRL_VAL, &mode);
			
			if (mode == 0)
			{
				if (green_time > 10)
				{	
					SuspendTimerCallbacks ();
					MessagePopup("Oops..","You can only set a maximum green signal delay of 10 seconds in Simple mode.");
					green_time = 10;
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, green_time);
					ResumeTimerCallbacks ();
				}
				else if (green_time < 1)
				{	
					SuspendTimerCallbacks ();
					MessagePopup("Oops..","You can only set a minimum green signal delay of 1 second in Simple mode.");
					green_time = 1;
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, green_time);
					ResumeTimerCallbacks ();
				}
			}
			else if (mode == 1)
			{	
				if (green_time > 120)
				{	
					SuspendTimerCallbacks ();
					MessagePopup("Oops..","You can only set a maximum green signal delay of 120 seconds in Advanced mode.");
					green_time = 120;
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, green_time);
					ResumeTimerCallbacks ();
				}
				else if (green_time < 1)
				{	
					SuspendTimerCallbacks ();
					MessagePopup("Oops..","You can only set a minimum green signal delay of 1 second in Advanced mode.");
					green_time = 1;
					SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, green_time);
					ResumeTimerCallbacks ();
				}
			}
			
			if (green_time < GreenSignalTime)
			{
				green_time = GreenSignalTime;
				SetCtrlAttribute (panelHandle, PANEL_GreenTime, ATTR_CTRL_VAL, green_time);
			}
			
			SetCtrlAttribute (panelHandle, PANEL_NTimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_ETimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_WTimer, ATTR_MAX_VALUE, green_time);
			SetCtrlAttribute (panelHandle, PANEL_STimer, ATTR_MAX_VALUE, green_time);
			break;
		}
	return 0;
}
int CVICALLBACK NTraffic_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_CTRL_VAL, &dump);
			if (dump > 120)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_CTRL_VAL, 120);
				ResumeTimerCallbacks ();
			}
			else if (dump < 4)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_NTraffic, ATTR_CTRL_VAL, 4);
				ResumeTimerCallbacks ();
			}
			break;
		}
	return 0;
}

int CVICALLBACK ETraffic_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_CTRL_VAL, &dump);
			if (dump > 120)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_CTRL_VAL, 120);
				ResumeTimerCallbacks ();
			}
			else if (dump < 4)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_ETraffic, ATTR_CTRL_VAL, 4);
				ResumeTimerCallbacks ();
			}
			break;
		}
	return 0;
}

int CVICALLBACK WTraffic_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_CTRL_VAL, &dump);
			if (dump > 120)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_CTRL_VAL, 120);
				ResumeTimerCallbacks ();
			}
			else if (dump < 4)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_WTraffic, ATTR_CTRL_VAL, 4);
				ResumeTimerCallbacks ();
			}
			break;
		}
	return 0;
}

int CVICALLBACK STraffic_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_CTRL_VAL, &dump);
			if (dump > 120)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_CTRL_VAL, 120);
				ResumeTimerCallbacks ();
			}
			else if (dump < 4)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set an incoming traffic of 4-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_STraffic, ATTR_CTRL_VAL, 4);
				ResumeTimerCallbacks ();
			}
			break;
		}
	return 0;
}

int CVICALLBACK VehicleSpeed_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_CTRL_VAL, &dump);
			if (dump > 120)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set a junction traffic of 2-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_CTRL_VAL, 120);
				ResumeTimerCallbacks ();
			}
			else if (dump < 2)
			{
				SuspendTimerCallbacks ();
				MessagePopup("Oops..","You can only set a junction traffic of 2-120 seconds/vehicle.");
				SetCtrlAttribute (panelHandle, PANEL_VehicleSpeed, ATTR_CTRL_VAL, 2);
				ResumeTimerCallbacks ();
			}
			break;
		}
	return 0;
}
												
												/********************
												*Menu bar call backs*
												********************/

void CVICALLBACK Exit_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int response;
	SuspendTimerCallbacks (); 
	response = ConfirmPopup ("Quit", "Are you sure you want to quit?");
	if (response == 1)
		QuitUserInterface (0);
	ResumeTimerCallbacks ();
}


void CVICALLBACK Credits_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	SuspendTimerCallbacks (); 
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n Developed by Rajiv Ram V\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n Developed by Rajiv Ram V\n    rajivram.v@gmail.com\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n    rajivram.v@gmail.com\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	ResumeTimerCallbacks ();
}
void CVICALLBACK Help_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	SuspendTimerCallbacks ();
	MessagePopup("Help","In Simple mode, you can only control the green signal delay (1-10 second(s)).\n\nIn Advanced mode, you can set the incoming traffic (4-120 seconds/vehicle),\nthe time taken by the vehicles to cross the junction (2-120 seconds/vehicle)once\nthe green lights are on and the green signal delay (1-120 second(s)).\n\nFor EQUAL traffic from all directions, use this equilibium formula:\n         G > 16 / ( (V/S)-10.4 )\n         G: Green signal delay\n         S: Time taken to cross junction\n         V: Vehicle traffic"); 
	ResumeTimerCallbacks ();
}


												/*******************
												*Hidden Quit Button*
												********************/
int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int response;
	switch (event)
		{
		case EVENT_COMMIT:
			SuspendTimerCallbacks ();
			response = ConfirmPopup ("Quit", "Are you sure you want to quit?");
			if (response == 1)
				QuitUserInterface (0);
			ResumeTimerCallbacks ();	
			break;
		}
	return 0;
}


