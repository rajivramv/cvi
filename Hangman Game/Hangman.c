#include <utility.h>
#include "toolbox.h"
#include <ansi_c.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include <time.h>
#include "Hangman.h"

#define WordList 100
#define MaxPlayers	10
#define MaxPlayerNameLength 20
#define MaxWordLength 20
#define MaxHintLength 50

#define MaxHangManStages 6
#define WordFound 1
#define TimeOver 2
#define ManHanged 3
 
static int mainpanel,playerpanel,gamepanel,endpanel,menubar;			//panelHandels and menuHandel
																		//declaring variables for handling the various player details
static int SelectedPlayer = -1,TotalPlayers = 0,PlayerIndex[MaxPlayers],PlayerWordsCompleted[MaxPlayers],PlayerScore[MaxPlayers],PlayerLife[MaxPlayers],PlayerGame[MaxPlayers][WordList];
char PlayerName[MaxPlayers][MaxPlayerNameLength + 1];

char Word[WordList][MaxWordLength + 1], Hint[WordList][MaxHintLength + 1];   //declaring variables to store the words from memory
static int TotalWords, SelectedWordIndex;
char MaskedWord[MaxWordLength + 1];

int Time;
double TimeKeeperSpeed;

int NewGame (void);
int AddWords (void);		 		 //Main Panel related functions
void ReadWords (void);
int Check (char *);

void ReadPlayerData (void);
void WritePlayerData (void);		 //Player Select Panel related functions
void PlayerDataDisp (void);

void RefreshAndDisp (int);  		 //Function common to Main Panel and Player Select Panel

void SelectWord (void);
void CheckChar (char *);			 //Game Panel related functions
int ConfirmAndSaveScore (char *);
void EndGame (int);

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainpanel = LoadPanel (0, "Hangman.uir", MainPanel)) < 0)
		return -1;
	playerpanel = LoadPanel (0, "Hangman.uir", PlayrPanel);
	gamepanel = LoadPanel (0, "Hangman.uir", GamePanel);
	endpanel = LoadPanel (gamepanel, "Hangman.uir", EndPanel);
	menubar = LoadMenuBar (mainpanel, "Hangman.uir", MainMenu);
	DisplayPanel (mainpanel);
	RunUserInterface ();
	DiscardPanel (mainpanel);
	return 0;
}
/************************************************************************************************************************************
***********************************************Main panel call backs*****************************************************************
*************************************************************************************************************************************/
int CVICALLBACK MainPanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			if (ConfirmPopup("Quit","Are you sure you want to quit?"))
				QuitUserInterface (0);
			break;
		}
	return 0;
}
int CVICALLBACK Quit_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (ConfirmPopup("Quit","Are you sure you want to quit?"))
				QuitUserInterface (0);
			break;
		}
	return 0;
}

int CVICALLBACK ViewScore_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			OpenDocumentInDefaultViewer ("Scores.txt", VAL_NO_ZOOM);
			break;
		}
	return 0;
}

int CVICALLBACK ContinueGame_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (panel)
			{
				case 1:
					HidePanel (mainpanel);
					break;
				case 5:
					HidePanel(endpanel);
					break;
			}	
			RefreshAndDisp (GamePanel);
			break;
		}
	return 0;
}

int CVICALLBACK AddWord_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
		switch (event)
		{
		case EVENT_COMMIT:
			AddWords();
			break;
		}
	return 0;
}

int CVICALLBACK NewGame_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			NewGame ();
			break;
		}
	return 0;
}

int CVICALLBACK AddorSelectPlayer_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			ReadPlayerData ();
			PlayerDataDisp ();
			InstallPopup (playerpanel);
			break;
		}
	return 0;
}
	
						 ///////////////////////////////////////////////////////////////////////////////////
						//	MAIN PANEL RELATED FUNCTIONS: NewGame (), AddWords(), ReadWords (), Check () //
					   /////////////////////////////////////////////////////////////////////////////////// 
int NewGame (void)
{
	int i = 0;																						   
	if (!(PlayerLife[SelectedPlayer] == 2 && PlayerWordsCompleted[SelectedPlayer] == 0))			   //DOESN'T ask for confirmation to start a new 
	{																								   //when a new user is created  
		if (ConfirmPopup("New Game", "Are you sure you want to start a new game?") == 0)
			return 0;
		if (!(PlayerWordsCompleted[SelectedPlayer] == TotalWords || PlayerLife[SelectedPlayer] == 0))  //If the player has already played
			ConfirmAndSaveScore("Do you wish to save your score before you start a new game?");		   //then prompted to save score
	}
	
	PlayerScore[SelectedPlayer] = 0;				 //Initialize all player details for starting the new game
	PlayerWordsCompleted[SelectedPlayer] = 0;  
	PlayerLife[SelectedPlayer] = 2;
	while (PlayerGame[SelectedPlayer][i] != 0)
	{
		PlayerGame[SelectedPlayer][i] = 0;
		i++;
	}
	WritePlayerData ();								//Write the data to file, close main panel and open the game panel
	HidePanel(mainpanel); 
	RefreshAndDisp (GamePanel);
	return 1;
}
int AddWords (void)
{
	int i,j,words_file;
	char word [21], hint[51], dump1[21], dump2[21];
	ReadWords ();								   //Read all the words to memory
	do
	{
		PromptPopup ("New Word", "Enter the word.\n\n\a Only alphabets and <space> are allowed.\n\a Word must begin with an alphabet.\n\a Maximum 20 characters.\n\a Press <esc> exit", word, 20);
		if (!strcmp(word,""))
		{
			if(!ConfirmPopup("New Word", "No word enterd. Do you wish to enter a new word?"))	 //Check and prompt when no word is entered
				return 0;
		}	
	}
	while (!Check(word));					  //Keep asking for a word till the criterias are satisfied or the user wishes to cancel entry
	strcpy (dump1,word);
	for (i=0;i<strlen(dump1);i++)
		dump1[i] = toupper(dump1[i]);
	for (i=0;i<TotalWords;i++)
	{
		strcpy (dump2,Word[i]);
		for (j=0;j<strlen(dump2);j++)		 //Check for the word if it exists. Words with same spelling but differnt capital/small 
			dump2[j] = toupper(dump2[j]);	//letter are also treated as duplicates.
		if(!strcmp(dump1,dump2))
		{
			MessagePopup ("Word Exists!", "The word entered already exists in the word list!");
			return 0;
		}
	}
	do
	{
		PromptPopup ("New Word", "Enter the hint for the word.\n\n\a Only alphabets and <space> are allowed.\n\a Hint must begin with an alphabet.\n\a Maximum 50 characters.\n\a Press <esc> to exit", hint, 50);
		if (!strcmp(hint,""))
		{
			if(!ConfirmPopup("New Word", "No hint enterd. Do you want to continue with the new word entry?"))   //Check and prompt when no hint is entered
				return 0;
		}	
	}
	while (!Check(hint));				   //Keep asking for the hint till the criterias are satisfied or the user wishes to cancel entry  
	words_file = OpenFile ("Words.txt", VAL_WRITE_ONLY, VAL_APPEND, VAL_ASCII);
	FmtFile (words_file, "%s#%s\n", word,hint);		 					 //Word is stored
	CloseFile (words_file);
	MessagePopup ("Word Added!","Word added successfully to the list!"); //Confirmation to user
	if (SelectedPlayer != -1)
		RefreshAndDisp (MainPanel);		  //IF A PLAYER HAS ALREADY COMPLETED THE WORD LIST AND IS SELECTED BEFORE THE NEW WORD ENTRY, THE USER IS ALLOWED TO PLAY THE NEW WORD THAT IS ENTERED 
	return 1;
}

void ReadWords (void)
{
	int words_file,bytes_read,i;														//Read and store the words and hints from memory
	words_file = OpenFile("Words.txt",VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
	TotalWords = 0;
	for (i=0;i<WordList;i++)
	{
		bytes_read = ScanFile (words_file,"%s[t10]>%s[t35]#%s[t10]", Word[i], Hint[i]);
		if (bytes_read == 0)
			break;
		TotalWords++;
	}
}			  
int Check (char *string)
{																		//function to check if the entered word
	int i;																//and hint is NOT 
	if (strlen(string) == 0) return 0;									//(1)empty
	if (string[0] == ' ')												//(2)doesnot begin with space
	{
		MessagePopup ("Check!", "Word must begin with an alphabet");
		return 0;
	}
	for (i=0;i<strlen(string);i++)										
	{
		if (!(isalpha (string[i]) || string[i] == ' '))					//(3)contains only alpha and <space>
		{	
			MessagePopup ("Check!", "Only alphabets and <space> are allowed");
			return 0;
		}
	}
	return 1;
}

/************************************************************************************************************************************
********************************************Player select panel call backs***********************************************************
*************************************************************************************************************************************/
int CVICALLBACK PlayrPanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel(playerpanel);
			break;
		}
	return 0;
}

int CVICALLBACK LoadPlayerProfile_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (playerpanel, PlayrPanel_PlayerList, ATTR_CTRL_VAL, &SelectedPlayer);
			HidePanel(playerpanel);
			RefreshAndDisp (MainPanel);
			break;
		}
	return 0;
}

int CVICALLBACK AddPlayer_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{	
	char new_player[MaxPlayerNameLength+1];
	int dump = -1,i;
	switch (event)
		{
		case EVENT_COMMIT:
			do
			{
				if (TotalPlayers == MaxPlayers)									   //Allow only MaxPlayer number of player addition 
				{
					MessagePopup("Sorry!","Maximum number of players reached. Please delete one or more players.");
					return 0;
				}
				PromptPopup ("Add Player", "Player name?", new_player, MaxPlayerNameLength); 
				if (strcmp(new_player,"") == 0)		   										 //Check if no player name is entered and take necessary steps
				{
					dump = ConfirmPopup ("Enter a player name!", "No player name entered. Do you wish to enter a new player name?");
					continue;
				}
				for (i=0;i<TotalPlayers;i++)
				{
					if (strcmp(PlayerName[i],new_player) == 0)			   //Check if player name already exists
					{
						dump = ConfirmPopup ("Player exists!", "Player name already exists! Do you wish to enter a new player name?");         	
						break;
					}
				}
			}
			while(dump == 1);
			if (dump == 0)
				break;												 
			strcpy(PlayerName[TotalPlayers],new_player);			 //Initialize the new players details
			PlayerIndex[TotalPlayers] = TotalPlayers;
			PlayerWordsCompleted[TotalPlayers] = 0;
			PlayerScore[TotalPlayers] = 0;
			PlayerLife[TotalPlayers] = 2;
			TotalPlayers++;
			WritePlayerData ();										 //Wtrite to file
			ReadPlayerData ();
			PlayerDataDisp ();
			break;
		}
	return 0;
}

int CVICALLBACK DelPlayer_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i,j,selected_player;
	char del_msg[60];
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (playerpanel, PlayrPanel_PlayerList, ATTR_CTRL_VAL, &selected_player);
			sprintf(del_msg,"Are you sure you want to delete the player '%s' ?",PlayerName[selected_player]);
			if (ConfirmPopup ("Delete Player",del_msg) == 0)		    		   //Confirm player deletion
				break;
			for (i=selected_player;i<TotalPlayers-1;i++)						  //Starting from the player name to be deleted
			{																	  //replace the current player details with the
				strcpy(PlayerName[i],PlayerName[i+1]);							  //next player details
				PlayerWordsCompleted[i] = PlayerWordsCompleted[i+1];
				PlayerScore[i] = PlayerScore[i+1];
				PlayerLife[i] = PlayerLife[i+1];
				for (j=0;j<WordList;j++)
					PlayerGame[i][j] = PlayerGame[i+1][j];
			}
			if (selected_player == SelectedPlayer)
			{																	 			 //If the player deleted has already been selected 
				SetCtrlAttribute (mainpanel, MainPanel_SelectedPlayer, ATTR_VISIBLE, 0);	 //then make necessary changes
				SetAttributeForCtrls (mainpanel, ATTR_DIMMED, 1, 2, MainPanel_NewGame, MainPanel_ContinueGame);
			}
			else if (selected_player < SelectedPlayer)
			{																 //If the deleted player is above the player selected
				SelectedPlayer--;											 //in the list, then change the selected player index
			}
			TotalPlayers--;
			WritePlayerData ();
			ReadPlayerData ();
			PlayerDataDisp ();
		}
	return 0;
}

int CVICALLBACK PlayerList_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			PlayerDataDisp ();
			break;
		}
	return 0;
}

int CVICALLBACK Cancel_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel(playerpanel);
			break;
		}
	return 0;
}
				 /////////////////////////////////////////////////////////////////////////////////////////////////////
				// PLAYER SELECT PANEL RELATED FUNCTIONS: ReadPlayerData (), WritePlayerData (), PlayerDataDisp () //
		   	   /////////////////////////////////////////////////////////////////////////////////////////////////////
						

void ReadPlayerData (void)
{																									  //code to read player data
	int bytes_read, i, player_file, player_game_file;
	player_file = OpenFile ("players.dat", VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
	player_game_file = OpenFile ("players_game.dat", VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
	TotalPlayers = 0;
	for (i=0;i<MaxPlayers;i++)
	{
		bytes_read = ScanFile (player_file,"%s[t10]>%d#%s[t35]#%d#%d#%d", &PlayerIndex[i],PlayerName[i],&PlayerWordsCompleted[i],&PlayerScore[i],&PlayerLife[i]);
		if (bytes_read == 0)
			break;
		TotalPlayers++;
		ScanFile (player_game_file,"%s>%*i[x]",WordList,&PlayerGame[i]);
	}
	CloseFile(player_file);
	CloseFile(player_game_file);
	ClearListCtrl (playerpanel, PlayrPanel_PlayerList);
	for (i=0;i<TotalPlayers;i++)
	{
		InsertListItem (playerpanel, PlayrPanel_PlayerList, PlayerIndex[i], PlayerName[i], PlayerIndex[i]);
	}
}

void WritePlayerData (void)
{																								   //code to write player data
	int i, player_file, player_game_file;
	player_file = OpenFile ("players.dat", VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	player_game_file = OpenFile ("players_game.dat", VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	for (i=0;i<TotalPlayers;i++)
	{
		FmtFile (player_file, "%d#%s#%d#%d#%d\n", PlayerIndex[i],PlayerName[i],PlayerWordsCompleted[i],PlayerScore[i],PlayerLife[i]);
		FmtFile (player_game_file, "%s<%*i[j1],\n", WordList, PlayerGame[i]);
	}
	CloseFile(player_file);
	CloseFile(player_game_file);
}	
																								   //code to disp player data
void PlayerDataDisp (void)
{
	int selected_player,dump;
	char cont_game[20] = "Continue Game";
	char score_disp[15] = "Score: " ,words_completed_disp[25] = "Words completed: ",life_disp[12] = "Life: ", name_disp[30] = "Player: ";
	GetNumListItems (playerpanel, PlayrPanel_PlayerList, &dump);
	if (dump == 0)
	{
		sprintf(score_disp,"Score: -----");
		sprintf(words_completed_disp,"Words completed: -----");
		sprintf(life_disp,"Life: -----");
		sprintf(name_disp,"Player: -----");
		SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_LABEL_TEXT, cont_game);
		PositionCtrlRelativeToCtrl (mainpanel, MainPanel_NewGame, 0, MainPanel_ContinueGame, 0, 27, kPositionCtrlBottomCenter,
									kPositionCtrlCenterJustification);
		SetAttributeForCtrls (playerpanel, ATTR_DIMMED, 1, 2, PlayrPanel_LoadPlayerProfile, PlayrPanel_DelPlayer);
	}
	else
	{
		SetAttributeForCtrls (playerpanel, ATTR_DIMMED, 0, 2, PlayrPanel_LoadPlayerProfile, PlayrPanel_DelPlayer);
		GetCtrlAttribute (playerpanel, PlayrPanel_PlayerList, ATTR_CTRL_VAL, &selected_player);
		sprintf(score_disp,"Score: %03d", PlayerScore[selected_player]);
		sprintf(words_completed_disp,"Words completed: %02d", PlayerWordsCompleted[selected_player]);
		sprintf(life_disp,"Life: %02d", PlayerLife[selected_player]);
		sprintf(name_disp,"Player: %s", PlayerName[selected_player]);
	}
	SetCtrlAttribute (playerpanel, PlayrPanel_NameDisplay, ATTR_CTRL_VAL, name_disp);
	SetCtrlAttribute (playerpanel, PlayrPanel_WordsCompletedDisplay, ATTR_CTRL_VAL, words_completed_disp);
	SetCtrlAttribute (playerpanel, PlayrPanel_ScoreDisplay, ATTR_CTRL_VAL, score_disp);
	SetCtrlAttribute (playerpanel, PlayrPanel_LifeDisplay, ATTR_CTRL_VAL, life_disp);
}  

 				/////////////////////////////////////////////////////////////////////////////////////
				// FUNCTION COMMON TO PLAYER SELECT PANEL AND MAIN PANEL: RefreshAndDisp (panel)  //
		   	   ////////////////////////////////////////////////////////////////////////////////////
void RefreshAndDisp (int panel) 																   //based on the panel parameted
{																								   //the panels are updated
	int i;
	char greetings[28],cont_game[40],score[12]; 
	switch(panel)
	{
		case MainPanel:
			sprintf(greetings,"Hi %s !",PlayerName[SelectedPlayer]);
			SetCtrlAttribute (mainpanel, MainPanel_SelectedPlayer, ATTR_CTRL_VAL,greetings);
			SetCtrlAttribute (mainpanel, MainPanel_SelectedPlayer, ATTR_VISIBLE, 1);
			ReadWords ();
			
			SetCtrlAttribute (mainpanel, MainPanel_ContinueGame, ATTR_DIMMED, 1);
			SetMenuBarAttribute (menubar, MainMenu_File_ContinueGame, ATTR_DIMMED, 1);
			SetCtrlAttribute (mainpanel, MainPanel_NewGame, ATTR_DIMMED, 0);
			SetMenuBarAttribute (menubar, MainMenu_File_NewGame, ATTR_DIMMED, 0);

			if (PlayerWordsCompleted[SelectedPlayer] == TotalWords)
				sprintf(cont_game,"Continue Game (Word List completed!)");
			else if (PlayerLife[SelectedPlayer] == 0)
				sprintf(cont_game,"Continue Game (Life left: %02d)",PlayerLife[SelectedPlayer]);
			else if (PlayerLife[SelectedPlayer] == 2 && PlayerWordsCompleted[SelectedPlayer] == 0)
				sprintf(cont_game,"Continue Game (Start a new game!)");
			else
			{
				SetCtrlAttribute (mainpanel, MainPanel_ContinueGame, ATTR_DIMMED, 0);
				SetMenuBarAttribute (menubar, MainMenu_File_ContinueGame, ATTR_DIMMED, 0);
				sprintf(cont_game,"Continue Game (Life left: %02d)",PlayerLife[SelectedPlayer]);
			}
			SetCtrlAttribute (mainpanel, MainPanel_ContinueGame, ATTR_LABEL_TEXT, cont_game);
			SetMenuBarAttribute (menubar, MainMenu_File_ContinueGame, ATTR_ITEM_NAME, cont_game);
			PositionCtrlRelativeToCtrl (mainpanel, MainPanel_NewGame, 0, MainPanel_ContinueGame, 0, 19, kPositionCtrlBottomCenter,
										kPositionCtrlCenterJustification);
			DisplayPanel (mainpanel);
			break;
		case GamePanel:
			HidePanel(gamepanel);
			sprintf(score,"Score: %3d", PlayerScore[SelectedPlayer]);
			SetCtrlAttribute (gamepanel, GamePanel_Score, ATTR_CTRL_VAL, score);
			SetCtrlAttribute (gamepanel, GamePanel_ShowHint, ATTR_DIMMED, 1);
			SetCtrlAttribute (gamepanel, GamePanel_StartGame, ATTR_DIMMED, 0);
			for (i=GamePanel_Z;i<=GamePanel_A;i++)
				SetCtrlAttribute (gamepanel, i, ATTR_DIMMED, 1);
			DisplayImageFile (gamepanel, GamePanel_HangMan, "stage00.jpg");
			SetCtrlAttribute (gamepanel, GamePanel_WordDisplay, ATTR_CTRL_VAL, "");
			SetCtrlAttribute (gamepanel, GamePanel_Time, ATTR_CTRL_VAL, 0);
			SetRandomSeed (0);
			DisplayPanel(gamepanel);
			break;
	}
}
/************************************************************************************************************************************
***************************************************Game panel call backs*************************************************************
*************************************************************************************************************************************/
int CVICALLBACK GamePanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	int dump;
	char char2check;
	switch (event)
		{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
		case EVENT_KEYPRESS:
			char2check = toupper(GetKeyPressEventCharacter (eventData2));					//allow only alphabet keypresses
			if (char2check >= 'A' && char2check <='Z')
			{
				GetCtrlAttribute (gamepanel, 'A' + GamePanel_A - char2check, ATTR_DIMMED, &dump);
				if (dump == 1)
					break;
				SetCtrlAttribute (gamepanel,'A' + GamePanel_A - char2check, ATTR_DIMMED, 1);
				CheckChar (&char2check);
			}
			break;

		}
	return 0;
}

int CVICALLBACK TimeKeeper_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:
			Time--;
			SetCtrlAttribute (gamepanel, GamePanel_Time, ATTR_CTRL_VAL, Time);
			if (Time == 0)
				EndGame (TimeOver);
			break;
		}
	return 0;
}

int CVICALLBACK AbandonGame_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump = -1,dump1 = -1;
	char str[80];
	switch (event)
		{
		case EVENT_COMMIT:
			sprintf(str,"Are you sure you want to return to the Main Menu? You will have %2d Life left!",PlayerLife[SelectedPlayer]-1);
			GetCtrlAttribute (gamepanel, GamePanel_TimeKeeper, ATTR_ENABLED, &dump);
			(dump == 0)?(dump = ConfirmPopup("Abandon Game", "Are you sure you want to return to the Main Menu?")):(dump1 = ConfirmPopup("Abandon Game", str));
			if (dump1 == 0)													 //if the user has started a game, but doesnt wishes to quit
				break;
			else if (dump1 == 1)											 //if the user has started a game and still wishes to quit
			{
				PlayerLife[SelectedPlayer]--;
				if (PlayerLife[SelectedPlayer] == 0)
					ConfirmAndSaveScore ("Do you wish to save your score?");
				WritePlayerData ();
				HidePanel (gamepanel);
				RefreshAndDisp (MainPanel);
				break;
			}
			else if(dump == 0)											    //if the user has not started a game and doesnt wish to quit
				break;
			else if (dump = 1)												//if the user has not started a game and wishes to quit
			{	
				HidePanel (gamepanel);
				RefreshAndDisp (MainPanel);
				break;
			}	
		}
	return 0;
}

int CVICALLBACK StartGame_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i;
	switch (event)
		{
		case EVENT_COMMIT:
			SelectWord();
			SetCtrlAttribute (gamepanel, GamePanel_WordDisplay, ATTR_CTRL_VAL, MaskedWord);
			TimeKeeperSpeed = 1;
			Time = 60;
			SetCtrlAttribute (gamepanel, GamePanel_StartGame, ATTR_DIMMED, 1);
			SetCtrlAttribute (gamepanel, GamePanel_ShowHint, ATTR_DIMMED, 0);
			SetCtrlAttribute (gamepanel, GamePanel_Time, ATTR_CTRL_VAL, Time);
			SetCtrlAttribute (gamepanel, GamePanel_TimeKeeper, ATTR_INTERVAL, TimeKeeperSpeed);
			SetCtrlAttribute (gamepanel, GamePanel_TimeKeeper, ATTR_ENABLED, 1);
			for (i=GamePanel_Z;i<=GamePanel_A;i++)
				SetCtrlAttribute (gamepanel, i, ATTR_DIMMED, 0);
			break;
		}
	return 0;
}

int CVICALLBACK ShowHint_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char score[12];
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlAttribute (gamepanel, GamePanel_ShowHint, ATTR_DIMMED, 1);
			MessagePopup ("Hint!", Hint[SelectedWordIndex]);
			sprintf(score,"Score: %3d", (PlayerScore[SelectedPlayer] -= 5));
			SetCtrlAttribute (gamepanel, GamePanel_Score, ATTR_CTRL_VAL, score);
			break;
		}
	return 0;
}
					

int CVICALLBACK KeyPress (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{																			   //function to get the button pressed and pass it on 
	int dump;																   //to CheckChar() function 
	char char2check;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlAttribute (gamepanel, GamePanel_TimeKeeper, ATTR_ENABLED, &dump);
			if (dump == 0)
				break;
			char2check = 'A' + GamePanel_A - control;
			SetCtrlAttribute (gamepanel, control, ATTR_DIMMED, 1);
			CheckChar (&char2check);
			break;
	}
	return 0;
}
				 ////////////////////////////////////////////////////////////////////////////////////////////////////
				// Game panel related functions: SelectWord (), CheckChar (), ConfirmAndSaveScore (), EndGame ()  //
			   ////////////////////////////////////////////////////////////////////////////////////////////////////
void SelectWord (void)
{
	int random_no,i,flag;
	do
	{
		random_no = Random (1, TotalWords+1);
		flag = 0;i=0;													//generate a unique random number which the player 
		while (PlayerGame[SelectedPlayer][i] != 0)						//hasnt played before
		{
			if(PlayerGame[SelectedPlayer][i] == random_no)
			{
				flag = 1;
				break;
			}	
			i++;
		}
	}
	while (flag == 1);
	SelectedWordIndex = random_no - 1;								    
	i = 0;
	for (i=0;i<strlen(Word[SelectedWordIndex]);i++)
	if (isalpha(Word[SelectedWordIndex][i]) > 0)					   //create the mask word based on the alphabets/space
		MaskedWord[i] = '-';
	else
		MaskedWord[i] = ' ';
	MaskedWord[i] = '\0';
}
void CheckChar (char *char2check)
{
	static int pic_index = 0;
	int i = 0,flag = 0;
	char hangman_stage[12];														  //check if character entered is in the word
	flag = 0; i = 0;															  //and update
	while (Word[SelectedWordIndex][i] != '\0')
	{
		if (toupper(Word[SelectedWordIndex][i]) == *char2check)
		{
			flag = 1;
			MaskedWord[i] = Word[SelectedWordIndex][i];
		}
		i++;	
	}
	SetCtrlAttribute (gamepanel, GamePanel_WordDisplay, ATTR_CTRL_VAL, MaskedWord);
	if (flag == 0)
	{																			//if the character is not in the word,then update the pic
		pic_index++;
		sprintf (hangman_stage, "stage%02d.jpg", pic_index);
		DisplayImageFile (gamepanel, GamePanel_HangMan, hangman_stage);
	}	
	if (pic_index == MaxHangManStages)											//end the game if the no. of chances are over
	{
		pic_index = 0;
		EndGame (ManHanged);
	}																			// end the game if the word is found out
	else if (strcmp(Word[SelectedWordIndex],MaskedWord) == 0)
	{
		sprintf (hangman_stage, "stage%02d.jpg", MaxHangManStages+1);
		DisplayImageFile (gamepanel, GamePanel_HangMan, hangman_stage);
		pic_index = 0;
		EndGame (WordFound);
	}
}
int ConfirmAndSaveScore (char *confirm_msg)										//function to simply confirm and save score
{
	int score_file;
	if (ConfirmPopup("Save Score",confirm_msg) == 0)
		return 0;
	score_file = OpenFile ("Scores.txt", VAL_WRITE_ONLY, VAL_APPEND, VAL_ASCII);
	FmtFile (score_file, "%s - %s - %d - %d\n",DateStr(), PlayerName[SelectedPlayer], PlayerWordsCompleted[SelectedPlayer], PlayerScore[SelectedPlayer]);
	CloseFile(score_file);
	return 1;
}
void EndGame (int status)
{
	char dump[55],i=0,cont_game[30], bonus[100] = "";								 //the finale function which will handle the
	SetCtrlAttribute (gamepanel, GamePanel_TimeKeeper, ATTR_ENABLED, 0);			 //the 3 types of game ending
	switch(status)
	{
		case WordFound:																 //when the word is found
			while(PlayerGame[SelectedPlayer][i] != 0)
				i++;
			PlayerGame[SelectedPlayer][i] = SelectedWordIndex + 1;
			PlayerWordsCompleted[SelectedPlayer]++;
			if ((PlayerWordsCompleted[SelectedPlayer] % 8) == 0)
			{
				PlayerLife[SelectedPlayer]++;
				sprintf(bonus,"%32s","Bonus!   Score + 10, Life + 1!\n");
				sprintf(bonus,"%s(complete %d more words for next life bonus!)",bonus,PlayerWordsCompleted[SelectedPlayer]+8);
			}
			else
			{
				sprintf(bonus,"%32s","Bonus!   Score + 10\n");
				sprintf(bonus,"%s(complete %d more words for next life bonus!)",bonus,(((int)(PlayerWordsCompleted[SelectedPlayer]/8))+1)*8-PlayerWordsCompleted[SelectedPlayer]);
			}
				
			SetCtrlAttribute (endpanel, EndPanel_Heading, ATTR_CTRL_VAL, "Hurray!");
			SetCtrlAttribute (endpanel, EndPanel_Body1, ATTR_CTRL_VAL, "You've found the word correctly!");
			sprintf(dump,"%s%8.2d","Words completed: ",PlayerWordsCompleted[SelectedPlayer]);
			SetCtrlAttribute (endpanel, EndPanel_Body2, ATTR_CTRL_VAL, dump);
			sprintf(dump,"%s%7.3d","Your score so far: ",PlayerScore[SelectedPlayer]);
			SetCtrlAttribute (endpanel, EndPanel_Body3, ATTR_CTRL_VAL, dump);
			sprintf(dump,"%s%8.2d","Time Remaining: ",Time);
			SetCtrlAttribute (endpanel, EndPanel_Body4, ATTR_CTRL_VAL, dump);			   //till here, does the necessary score 
			SetCtrlAttribute (endpanel, EndPanel_Body5, ATTR_CTRL_VAL, bonus);			   //and bonuses calc. 
			PlayerScore[SelectedPlayer] += Time + 10;
			sprintf(dump,"%s%8.2d","Current Score: ",PlayerScore[SelectedPlayer]);
			SetCtrlAttribute (endpanel, EndPanel_Body6, ATTR_CTRL_VAL, dump);

			SetAttributeForCtrls (endpanel, ATTR_VISIBLE, 1, 5, EndPanel_Body2, EndPanel_Body3, EndPanel_Body4, EndPanel_Body5, EndPanel_Body6);
			SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_VISIBLE, 0);
			SetCtrlAttribute (endpanel, EndPanel_Proceed, ATTR_DIMMED, 0);
			SetCtrlAttribute (endpanel, EndPanel_Body7, ATTR_VISIBLE, 0);

			if (PlayerWordsCompleted[SelectedPlayer] == TotalWords)
			{
				sprintf(dump,"%50s","Congratulations! You've completed the word list!");
				SetCtrlAttribute (endpanel, EndPanel_Body7, ATTR_CTRL_VAL, dump);			  //to see if the user finished the word list
				SetCtrlAttribute (endpanel, EndPanel_Proceed, ATTR_DIMMED, 1);
				SetCtrlAttribute (endpanel, EndPanel_Body7, ATTR_VISIBLE, 1);	
			}
		
			SetPanelAttribute (endpanel, ATTR_HEIGHT, 320);
			SetCtrlAttribute (endpanel, EndPanel_Back2MainMenu, ATTR_TOP, 277);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Heading, 0, EndPanel_Body1, 0, 6, kPositionCtrlBottomCenter,
										kPositionCtrlCenterJustification);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Heading, 0, EndPanel_Body2, 0, 35, kPositionCtrlBottomCenter,
										kPositionCtrlCenterJustification);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Body2, 0, EndPanel_Body3, 0, 6, kPositionCtrlBottomCenter,
										kPositionCtrlBottomOrRightJustication);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Body3, 0, EndPanel_Body4, 0, 6, kPositionCtrlBottomCenter,		  //customize panel
										kPositionCtrlBottomOrRightJustication);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Body4, 0, EndPanel_Body5, 0, 6, kPositionCtrlBottomCenter,
										kPositionCtrlCenterJustification);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Body4, 0, EndPanel_Body6, 0, 52, kPositionCtrlBottomCenter,
										kPositionCtrlBottomOrRightJustication);
			PositionCtrlRelativeToCtrl (endpanel, EndPanel_Heading, 0, EndPanel_Body7, 0, 195, kPositionCtrlBottomCenter,
										kPositionCtrlCenterJustification);
			break;																	 
		case TimeOver:																 //handle game ending due to end of time
			SetCtrlAttribute (endpanel, EndPanel_Heading, ATTR_CTRL_VAL, "Ooops...");
			SetCtrlAttribute (endpanel, EndPanel_Body1, ATTR_CTRL_VAL, "Time's up!");
			break;
		case ManHanged:																//handle game ending due to running out of chances
			SetCtrlAttribute (endpanel, EndPanel_Heading, ATTR_CTRL_VAL, "Ooops...");
			SetCtrlAttribute (endpanel, EndPanel_Body1, ATTR_CTRL_VAL, "You've ran out chances!");
			break;
	
	}
	if (status == TimeOver || status == ManHanged)								  //common code to handle game ending due to end of time or running out of chances
	{
		PlayerLife[SelectedPlayer]--;
	
		sprintf(cont_game,"Continue Game (Life left: %02d)",PlayerLife[SelectedPlayer]);
		SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_LABEL_TEXT, cont_game);
			
		SetPanelAttribute (endpanel, ATTR_HEIGHT, 134);
		SetAttributeForCtrls (endpanel, ATTR_VISIBLE, 0, 6, EndPanel_Body2, EndPanel_Body3, EndPanel_Body4, EndPanel_Body5,EndPanel_Body6,EndPanel_Body7);
		SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_VISIBLE, 1);
		PlayerLife[SelectedPlayer] == 0?SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_DIMMED, 1):SetCtrlAttribute (endpanel, EndPanel_ContinueGame, ATTR_DIMMED, 0);
		SetCtrlAttribute (endpanel, EndPanel_Back2MainMenu, ATTR_TOP, 97);
		PositionCtrlRelativeToCtrl (endpanel, EndPanel_Heading, 0, EndPanel_Body1, 0, 4, kPositionCtrlBottomCenter,
									kPositionCtrlCenterJustification);
	}
	WritePlayerData();
	InstallPopup(endpanel);
}

int CVICALLBACK Back2MainMenu_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (PlayerLife[SelectedPlayer] == 0 || PlayerWordsCompleted[SelectedPlayer] == TotalWords)
			{
				ConfirmAndSaveScore ("Do you wish to save your score?");
				HidePanel (gamepanel);
				HidePanel (endpanel);
				RefreshAndDisp (MainPanel);
			}
			else if (ConfirmPopup ("Back to Main Menu", "Are you sure you want to return to the Main Menu ?"))
			{
				HidePanel (gamepanel);
				HidePanel (endpanel);
				RefreshAndDisp (MainPanel);
			}
			break;
		}
	return 0;
}
/************************************************************************************************************************************
***************************************************Menu Bar call backs*************************************************************
*************************************************************************************************************************************/

void CVICALLBACK AddorSelectPlayer_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{																											   
	ReadPlayerData ();
	PlayerDataDisp ();
	InstallPopup (playerpanel);
			
}

void CVICALLBACK NewGame_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	NewGame ();
}

void CVICALLBACK ContinueGame_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	HidePanel(mainpanel);
	RefreshAndDisp(GamePanel);
}

void CVICALLBACK AddWord_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	AddWords ();
}

void CVICALLBACK ViewScore_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	OpenDocumentInDefaultViewer ("Scores.txt", VAL_NO_ZOOM);
}

void CVICALLBACK Quit_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	if (ConfirmPopup("Quit","Are you sure you want to quit?"))
		QuitUserInterface (0);
			
}

void CVICALLBACK Credits_menu_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n Developed by Rajiv Ram V\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n Developed by Rajiv Ram V\n    rajivram.v@gmail.com\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n    rajivram.v@gmail.com\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
}
