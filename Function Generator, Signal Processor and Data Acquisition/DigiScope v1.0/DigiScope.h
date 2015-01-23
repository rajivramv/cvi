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

#define  FGenPanel                       1
#define  FGenPanel_StatusPanel           2
#define  FGenPanel_toggleOutput          3       /* callback function: toggleOutput_Code */
#define  FGenPanel_toggleArbitrary       4       /* callback function: toggleArbitrary_Code */
#define  FGenPanel_toggleRamp            5       /* callback function: toggleRamp_Code */
#define  FGenPanel_toggleSawtooth        6       /* callback function: toggleSawtooth_Code */
#define  FGenPanel_fGenExit              7       /* callback function: fGenExit_Code */
#define  FGenPanel_toggleTriangular      8       /* callback function: toggleTriangular_Code */
#define  FGenPanel_toggleSquare          9       /* callback function: toggleSquare_Code */
#define  FGenPanel_toggleSine            10      /* callback function: toggleSine_Code */
#define  FGenPanel_FGenHeader            11
#define  FGenPanel_dialPhase             12      /* callback function: dialPhase_Code */
#define  FGenPanel_dialDutyCycle         13      /* callback function: dialDutyCycle_Code */
#define  FGenPanel_dialDCOffset          14      /* callback function: dialDCOffset_Code */
#define  FGenPanel_dialAmpl              15      /* callback function: dialAmpl_Code */
#define  FGenPanel_dialFreq              16      /* callback function: dialFreq_Code */
#define  FGenPanel_FGenLogo              17
#define  FGenPanel_txtFreq               18
#define  FGenPanel_txtAmpl               19
#define  FGenPanel_txtDCOffset           20
#define  FGenPanel_txtDutyCycle          21
#define  FGenPanel_txtPhase              22
#define  FGenPanel_Timer                 23      /* callback function: Timer_Code */

#define  MainPanel                       2
#define  MainPanel_mainPanelBackGrnd     2
#define  MainPanel_FGen_Button           3       /* callback function: FGen_Button_Code */
#define  MainPanel_SigPro_Button         4       /* callback function: SigPro_Button_Code */
#define  MainPanel_DAQ_Button            5       /* callback function: DAQ_Button_Code */
#define  MainPanel_Options_Button        6       /* callback function: Options_Button_Code */
#define  MainPanel_Exit_Button           7       /* callback function: Exit_Button_Code */
#define  MainPanel_SPLITTER              8

#define  PlotPanel                       3
#define  PlotPanel_dialTimeBase          2       /* callback function: dialTimeBase_Code */
#define  PlotPanel_dialGain              3       /* callback function: dialGain_Code */
#define  PlotPanel_GraphSave             4       /* callback function: GraphSave_Code */
#define  PlotPanel_GraphPrint            5       /* callback function: GraphPrint_Code */
#define  PlotPanel_GraphClose            6       /* callback function: GraphClose_Code */
#define  PlotPanel_GRAPH                 7
#define  PlotPanel_switchZoom            8       /* callback function: switchZoom_Code */
#define  PlotPanel_sigProBut             9       /* callback function: sigProBut_Code */
#define  PlotPanel_toggleRunStop         10      /* callback function: toggleRunStop_Code */

#define  splashScrn                      4
#define  splashScrn_ProgressBar          2
#define  splashScrn_splashScrnLOGO       3
#define  splashScrn_splashScrnTimer      4       /* callback function: splashScrnTimer_Code */

#define  SProPanel                       5
#define  SProPanel_StatusPanel           2
#define  SProPanel_toggleSPOutput        3       /* callback function: toggleSPOutput_Code */
#define  SProPanel_SProHeader            4
#define  SProPanel_FGenLogo              5
#define  SProPanel_sigProExit            6       /* callback function: sigProExit_Code */
#define  SProPanel_selectFilterType      7       /* callback function: selectFilterType_Code */
#define  SProPanel_selectFilter          8       /* callback function: selectFilter_Code */
#define  SProPanel_selectPeak            9       /* callback function: selectPeak_Code */
#define  SProPanel_selectSignal          10      /* callback function: selectSignal_Code */
#define  SProPanel_selectionRing         11      /* callback function: selectionRing_Code */
#define  SProPanel_ringUT                12      /* callback function: ringUT_Code */
#define  SProPanel_ringSampling          13      /* callback function: ringSampling_Code */
#define  SProPanel_ringUpCut             14      /* callback function: ringUpCut_Code */
#define  SProPanel_ringLowCut            15      /* callback function: ringLowCut_Code */
#define  SProPanel_ringOrder             16      /* callback function: ringOrder_Code */
#define  SProPanel_ringLT                17      /* callback function: ringLT_Code */
#define  SProPanel_txtHz3                18
#define  SProPanel_txtHz2                19
#define  SProPanel_txtHz1                20
#define  SProPanel_txtUnit2              21
#define  SProPanel_txtUnit1              22
#define  SProPanel_Timer_FileLoad        23      /* callback function: Timer_FileLoad_Code */
#define  SProPanel_TimerOutPutControl    24      /* callback function: TimerOutPutControl_Code */
#define  SProPanel_TimerFilter           25      /* callback function: TimerFilter_Code */
#define  SProPanel_TimerSigPro           26      /* callback function: TimerSigPro */


     /* Menu Bars, Menus, and Menu Items: */

#define  MainMenu                        1
#define  MainMenu_File                   2
#define  MainMenu_File_FunctionGenerator 3       /* callback function: FunctionGenerator_Code */
#define  MainMenu_File_SignalProcessor   4       /* callback function: SignalProcessor_Code */
#define  MainMenu_File_DataAcquisitor    5       /* callback function: DataAcquisitor_Code */
#define  MainMenu_File_Exit              6       /* callback function: Exit_Code */
#define  MainMenu_Help                   7
#define  MainMenu_Help_Contents          8       /* callback function: Contents_Code */
#define  MainMenu_Help_Instructions      9       /* callback function: Instructions_Code */
#define  MainMenu_Help_Credits           10      /* callback function: Credits_Code */


     /* Callback Prototypes: */ 

void CVICALLBACK Contents_Code(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK Credits_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DAQ_Button_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK DataAcquisitor_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK dialAmpl_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialDCOffset_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialDutyCycle_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialFreq_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialGain_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialPhase_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK dialTimeBase_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Exit_Button_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Exit_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK FGen_Button_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK fGenExit_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK FunctionGenerator_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK GraphClose_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphPrint_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphSave_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Instructions_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK Options_Button_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringLowCut_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringLT_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringOrder_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringSampling_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringUpCut_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ringUT_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectFilter_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectFilterType_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectionRing_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectPeak_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectSignal_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK SignalProcessor_Code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SigPro_Button_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK sigProBut_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK sigProExit_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK splashScrnTimer_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK switchZoom_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Timer_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Timer_FileLoad_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerFilter_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerOutPutControl_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerSigPro(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleArbitrary_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleOutput_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleRamp_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleRunStop_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleSawtooth_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleSine_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleSPOutput_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleSquare_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK toggleTriangular_Code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
