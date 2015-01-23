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

#define  EndPanel                        1
#define  EndPanel_Heading                2
#define  EndPanel_Body1                  3
#define  EndPanel_Body2                  4
#define  EndPanel_Body3                  5
#define  EndPanel_Body4                  6
#define  EndPanel_ContinueGame           7       /* callback function: ContinueGame_code */
#define  EndPanel_Back2MainMenu          8       /* callback function: Back2MainMenu_code */
#define  EndPanel_Proceed                9       /* callback function: ContinueGame_code */
#define  EndPanel_Body6                  10
#define  EndPanel_Body5                  11
#define  EndPanel_Body7                  12
#define  EndPanel_bkgrnd                 13

#define  GamePanel                       2       /* callback function: GamePanel_code */
#define  GamePanel_Z                     2       /* callback function: KeyPress */
#define  GamePanel_Y                     3       /* callback function: KeyPress */
#define  GamePanel_X                     4       /* callback function: KeyPress */
#define  GamePanel_W                     5       /* callback function: KeyPress */
#define  GamePanel_V                     6       /* callback function: KeyPress */
#define  GamePanel_U                     7       /* callback function: KeyPress */
#define  GamePanel_T                     8       /* callback function: KeyPress */
#define  GamePanel_S                     9       /* callback function: KeyPress */
#define  GamePanel_R                     10      /* callback function: KeyPress */
#define  GamePanel_Q                     11      /* callback function: KeyPress */
#define  GamePanel_P                     12      /* callback function: KeyPress */
#define  GamePanel_O                     13      /* callback function: KeyPress */
#define  GamePanel_N                     14      /* callback function: KeyPress */
#define  GamePanel_M                     15      /* callback function: KeyPress */
#define  GamePanel_L                     16      /* callback function: KeyPress */
#define  GamePanel_K                     17      /* callback function: KeyPress */
#define  GamePanel_J                     18      /* callback function: KeyPress */
#define  GamePanel_I                     19      /* callback function: KeyPress */
#define  GamePanel_H                     20      /* callback function: KeyPress */
#define  GamePanel_G                     21      /* callback function: KeyPress */
#define  GamePanel_F                     22      /* callback function: KeyPress */
#define  GamePanel_E                     23      /* callback function: KeyPress */
#define  GamePanel_D                     24      /* callback function: KeyPress */
#define  GamePanel_C                     25      /* callback function: KeyPress */
#define  GamePanel_B                     26      /* callback function: KeyPress */
#define  GamePanel_A                     27      /* callback function: KeyPress */
#define  GamePanel_AbandonGame           28      /* callback function: AbandonGame_code */
#define  GamePanel_ShowHint              29      /* callback function: ShowHint_code */
#define  GamePanel_StartGame             30      /* callback function: StartGame_code */
#define  GamePanel_WordDisplay           31
#define  GamePanel_TimeKeeper            32      /* callback function: TimeKeeper_code */
#define  GamePanel_HangMan               33
#define  GamePanel_Time                  34
#define  GamePanel_Score                 35
#define  GamePanel_bkgrnd                36

#define  MainPanel                       3       /* callback function: MainPanel_code */
#define  MainPanel_Quit                  2       /* callback function: Quit_code */
#define  MainPanel_ViewScore             3       /* callback function: ViewScore_code */
#define  MainPanel_ContinueGame          4       /* callback function: ContinueGame_code */
#define  MainPanel_AddWord               5       /* callback function: AddWord_code */
#define  MainPanel_NewGame               6       /* callback function: NewGame_code */
#define  MainPanel_AddorSelectPlayer     7       /* callback function: AddorSelectPlayer_code */
#define  MainPanel_SelectedPlayer        8
#define  MainPanel_hangman               9
#define  MainPanel_bkgrnd                10

#define  PlayrPanel                      4       /* callback function: PlayrPanel_code */
#define  PlayrPanel_PlayerList           2       /* callback function: PlayerList_code */
#define  PlayrPanel_DelPlayer            3       /* callback function: DelPlayer_code */
#define  PlayrPanel_AddPlayer            4       /* callback function: AddPlayer_code */
#define  PlayrPanel_LoadPlayerProfile    5       /* callback function: LoadPlayerProfile_code */
#define  PlayrPanel_Cancel               6       /* callback function: Cancel_code */
#define  PlayrPanel_NameDisplay          7
#define  PlayrPanel_WordsCompletedDisplay 8
#define  PlayrPanel_ScoreDisplay         9
#define  PlayrPanel_bkgrnd               10
#define  PlayrPanel_LifeDisplay          11


     /* Menu Bars, Menus, and Menu Items: */

#define  MainMenu                        1
#define  MainMenu_File                   2
#define  MainMenu_File_AddOrSelectPlayer 3       /* callback function: AddorSelectPlayer_menu_code */
#define  MainMenu_File_NewGame           4       /* callback function: NewGame_menu_code */
#define  MainMenu_File_ContinueGame      5       /* callback function: ContinueGame_menu_code */
#define  MainMenu_File_AddWord           6       /* callback function: AddWord_menu_code */
#define  MainMenu_File_ViewScore         7       /* callback function: ViewScore_menu_code */
#define  MainMenu_File_Quit              8       /* callback function: Quit_menu_code */
#define  MainMenu_Credits                9       /* callback function: Credits_menu_code */


     /* Callback Prototypes: */ 

int  CVICALLBACK AbandonGame_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AddorSelectPlayer_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK AddorSelectPlayer_menu_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK AddPlayer_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AddWord_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK AddWord_menu_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK Back2MainMenu_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Cancel_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ContinueGame_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ContinueGame_menu_code(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK Credits_menu_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DelPlayer_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GamePanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK KeyPress(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadPlayerProfile_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NewGame_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK NewGame_menu_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PlayerList_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlayrPanel_code(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Quit_menu_code(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ShowHint_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartGame_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimeKeeper_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ViewScore_code(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ViewScore_menu_code(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
