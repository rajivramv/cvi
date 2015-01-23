/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2011. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                           1
#define  PANEL_Wright                    2
#define  PANEL_Wleft                     3
#define  PANEL_Wstraight                 4
#define  PANEL_Wred                      5
#define  PANEL_Worange                   6
#define  PANEL_Sright                    7
#define  PANEL_Sleft                     8
#define  PANEL_Sstraight                 9
#define  PANEL_Sred                      10
#define  PANEL_Sorange                   11
#define  PANEL_Eright                    12
#define  PANEL_Eleft                     13
#define  PANEL_Estraight                 14
#define  PANEL_Ered                      15
#define  PANEL_Eorange                   16
#define  PANEL_Nright                    17
#define  PANEL_Nleft                     18
#define  PANEL_Nstraight                 19
#define  PANEL_Nred                      20
#define  PANEL_Norange                   21
#define  PANEL_Meridiem                  22
#define  PANEL_SS                        23
#define  PANEL_MM                        24
#define  PANEL_HH                        25
#define  PANEL_IncSpeed                  26      /* callback function: IncSpeed_code */
#define  PANEL_DecSpeed                  27      /* callback function: DecSpeed_code */
#define  PANEL_NLeft                     28
#define  PANEL_NStraight                 29
#define  PANEL_NRight                    30
#define  PANEL_SRight                    31
#define  PANEL_SStraight                 32
#define  PANEL_SLeft                     33
#define  PANEL_TimeKeeper                34      /* callback function: TimeKeeper_code */
#define  PANEL_WTraffic                  35      /* callback function: WTraffic_code */
#define  PANEL_NTraffic                  36      /* callback function: NTraffic_code */
#define  PANEL_STraffic                  37      /* callback function: STraffic_code */
#define  PANEL_ETraffic                  38      /* callback function: ETraffic_code */
#define  PANEL_VehicleSpeed              39      /* callback function: VehicleSpeed_code */
#define  PANEL_GreenTime                 40      /* callback function: GreenTime_code */
#define  PANEL_ETimer                    41
#define  PANEL_NTimer                    42
#define  PANEL_WTimer                    43
#define  PANEL_STimer                    44
#define  PANEL_SimulateAndPause          45      /* callback function: SimulateAndPause_code */
#define  PANEL_TEXTMSG_6                 46
#define  PANEL_TEXTMSG_7                 47
#define  PANEL_ERight                    48
#define  PANEL_EStraight                 49
#define  PANEL_ELeft                     50
#define  PANEL_WLeft                     51
#define  PANEL_WStraight                 52
#define  PANEL_WRight                    53
#define  PANEL_Mode                      54      /* callback function: Mode_code */
#define  PANEL_Stop                      55      /* callback function: Stop_code */
#define  PANEL_QUITBUTTON                56      /* callback function: QuitCallback */
#define  PANEL_CrossRoad                 57
#define  PANEL_TimeKeeperSpeed           58


     /* Menu Bars, Menus, and Menu Items: */

#define  MainMenu                        1
#define  MainMenu_File                   2
#define  MainMenu_File_Exit              3       /* callback function: Exit_code */
#define  MainMenu_About                  4
#define  MainMenu_About_Help             5       /* callback function: Help_code */
#define  MainMenu_About_Credits          6       /* callback function: Credits_code */


     /* Callback Prototypes: */ 

void CVICALLBACK Credits_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DecSpeed_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ETraffic_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Exit_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK GreenTime_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Help_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK IncSpeed_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Mode_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NTraffic_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SimulateAndPause_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Stop_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK STraffic_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimeKeeper_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK VehicleSpeed_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WTraffic_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
