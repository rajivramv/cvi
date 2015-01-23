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

#define  DAQMode                         1
#define  DAQMode_SamplingFreq            2
#define  DAQMode_DAQOutput               3       /* callback function: DAQOutput_code */
#define  DAQMode_DAQTimer                4       /* callback function: DAQTimer_code */

#define  DrugLevel                       2       /* callback function: DrugLevelPanel_code */
#define  DrugLevel_TachyDrug             2
#define  DrugLevel_BradyDrug             3
#define  DrugLevel_TEXTMSG               4

#define  FileMode                        3
#define  FileMode_SaveToFile             2       /* callback function: SaveToFile_code */
#define  FileMode_LoadFile               3       /* callback function: LoadFile_code */

#define  MainInt                         4       /* callback function: MainInt_code */
#define  MainInt_PanelSplitter           2
#define  MainInt_SplitterMove            3       /* callback function: SplitterMove_code */
#define  MainInt_Settings                4       /* callback function: PanelDisp_code */
#define  MainInt_DAQMode                 5       /* callback function: PanelDisp_code */
#define  MainInt_FileMode                6       /* callback function: PanelDisp_code */
#define  MainInt_Lead3                   7
#define  MainInt_Lead1                   8
#define  MainInt_SPLITTER_2              9
#define  MainInt_SPLITTER                10
#define  MainInt_Lead2                   11
#define  MainInt_SPLITTER_3              12
#define  MainInt_ECGProcessor            13      /* callback function: ECGProcessor_code */
#define  MainInt_Treatment_Label         14
#define  MainInt_DrugLevel               15      /* callback function: ShowPanel_code */
#define  MainInt_VCG                     16      /* callback function: ShowPanel_code */
#define  MainInt_Treatment               17
#define  MainInt_Condition               18
#define  MainInt_HeartRate_Label         19
#define  MainInt_Condition_Label         20
#define  MainInt_HeartRate               21
#define  MainInt_Status                  22
#define  MainInt_HArrow                  23
#define  MainInt_HArrow_2                24
#define  MainInt_VScale                  25
#define  MainInt_HScale                  26

#define  PSD                             5       /* callback function: PSDPanel_code */
#define  PSD_PSDGraph                    2       /* callback function: PSDGraph_code */
#define  PSD_Noise_Label                 3

#define  Settings                        6
#define  Settings_NoiseChange            2       /* callback function: ShowPanel_code */
#define  Settings_Noise                  3
#define  Settings_Noise_Label            4

#define  VCG                             7       /* callback function: VCGPanel_code */
#define  VCG_VCG                         2
#define  VCG_Heart                       3


     /* Menu Bars, Menus, and Menu Items: */

#define  MainMenu                        1
#define  MainMenu_Menu                   2
#define  MainMenu_Menu_FileMode          3       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_Menu_DAQMode           4       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_Menu_Settings          5       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_Menu_Quit              6       /* callback function: Quit_code */
#define  MainMenu_Credits                7       /* callback function: Credits_code */


     /* Callback Prototypes: */ 

void CVICALLBACK Credits_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DAQOutput_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DAQTimer_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DrugLevelPanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ECGProcessor_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadFile_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainInt_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK MainMenu_PanelDisp_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PanelDisp_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PSDGraph_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PSDPanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Quit_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SaveToFile_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ShowPanel_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SplitterMove_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK VCGPanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
