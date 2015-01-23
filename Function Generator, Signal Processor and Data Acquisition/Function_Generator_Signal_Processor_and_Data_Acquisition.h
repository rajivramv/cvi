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

#define  CopySignal                      1
#define  CopySignal_CopySignal           2       /* callback function: CopySignal_code */
#define  CopySignal_ConvSignalMsg        3

#define  DAQMode                         2
#define  DAQMode_Output                  2       /* callback function: DAQOutput_code */
#define  DAQMode_SamplingFreq            3
#define  DAQMode_RefreshGraph            4       /* callback function: RefreshGraph_code */

#define  FileMode                        3
#define  FileMode_LoadFromFile           2       /* callback function: LoadFromFile_code */
#define  FileMode_SignalSourceToSave     3
#define  FileMode_SaveToFile             4       /* callback function: SaveToFile_code */
#define  FileMode_SPLITTER               5
#define  FileMode_TEXTMSG                6

#define  MainInt                         4       /* callback function: MainInt_code */
#define  MainInt_PanelSplitter           2
#define  MainInt_FileMode                3       /* callback function: PanelDisp_code */
#define  MainInt_DAQMode                 4       /* callback function: PanelDisp_code */
#define  MainInt_SignalProcessor         5       /* callback function: PanelDisp_code */
#define  MainInt_SignalGenerator         6       /* callback function: PanelDisp_code */
#define  MainInt_SplitterMove            7       /* callback function: SplitterMove_code */
#define  MainInt_SignalProcGraph         8
#define  MainInt_SignalGenGraph          9
#define  MainInt_ZoomType                10      /* callback function: ZoomType_code */
#define  MainInt_AmpBase                 11      /* callback function: AmpBase_code */
#define  MainInt_TimeBase                12      /* callback function: TimeBase_code */
#define  MainInt_AmpShift                13      /* callback function: AmpBase_code */
#define  MainInt_ProcSignalZoomReset     14      /* callback function: ZoomReset_code */
#define  MainInt_ProcSignalZoomOut       15      /* callback function: ZoomOut_code */
#define  MainInt_MainSignalZoomReset     16      /* callback function: ZoomReset_code */
#define  MainInt_ProcSignalZoomIn        17      /* callback function: ZoomIn_code */
#define  MainInt_MainSignalZoomOut       18      /* callback function: ZoomOut_code */
#define  MainInt_HUD                     19
#define  MainInt_MainSignalZoomIn        20      /* callback function: ZoomIn_code */
#define  MainInt_MainSignalZoomDiv       21
#define  MainInt_ProcSignalZoomDiv       22

#define  SignalGen                       5
#define  SignalGen_SignalWaveForm        2       /* callback function: SignalWaveForm_code */
#define  SignalGen_Slope                 3       /* callback function: SignalGenOFF_code */
#define  SignalGen_Amplitude             4       /* callback function: SignalGenOFF_code */
#define  SignalGen_DutyCycle             5       /* callback function: SignalGenOFF_code */
#define  SignalGen_DCShift               6       /* callback function: SignalGenOFF_code */
#define  SignalGen_Phase                 7       /* callback function: SignalGenOFF_code */
#define  SignalGen_Frequency             8       /* callback function: SignalGenOFF_code */
#define  SignalGen_Output                9       /* callback function: SignalGenOutput_code */

#define  SignalProc                      6
#define  SignalProc_PeakDetector         2       /* callback function: PeakDetector_code */
#define  SignalProc_UpperThreshold       3       /* callback function: UpperThreshold_code */
#define  SignalProc_LowerThreshold       4       /* callback function: LowerThreshold_code */
#define  SignalProc_LowerCutOff          5
#define  SignalProc_HigherCutOff         6
#define  SignalProc_CutOff               7
#define  SignalProc_FilterOrder          8
#define  SignalProc_Output               9       /* callback function: SignalProcOutput_code */
#define  SignalProc_FilterMethod         10      /* callback function: FilterMethod_code */
#define  SignalProc_FilterType           11      /* callback function: FilterType_code */
#define  SignalProc_Splitter_2           12
#define  SignalProc_Splitter             13
#define  SignalProc_SignalProcTimer      14      /* callback function: SignalProcTimer_code */
#define  SignalProc_TEXTMSG              15
#define  SignalProc_TEXTMSG_2            16
#define  SignalProc_TEXTMSG_3            17


     /* Menu Bars, Menus, and Menu Items: */

#define  MainMenu                        1
#define  MainMenu_File                   2
#define  MainMenu_File_SignalGenerator   3       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_File_DAQMode           4       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_File_FileMode          5       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_File_SignalProcessor   6       /* callback function: MainMenu_PanelDisp_code */
#define  MainMenu_File_Quit              7       /* callback function: Quit_code */
#define  MainMenu_Credits                8       /* callback function: Credits_code */


     /* Callback Prototypes: */ 

int  CVICALLBACK AmpBase_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CopySignal_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Credits_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DAQOutput_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FilterMethod_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FilterType_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadFromFile_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LowerThreshold_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainInt_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK MainMenu_PanelDisp_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PanelDisp_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PeakDetector_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Quit_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK RefreshGraph_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveToFile_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SignalGenOFF_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SignalGenOutput_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SignalProcOutput_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SignalProcTimer_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SignalWaveForm_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SplitterMove_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimeBase_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UpperThreshold_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomIn_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomOut_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomReset_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomType_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
