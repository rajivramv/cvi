#include <analysis.h>
#include <formatio.h>
#include "toolbox.h"
#include <cvirte.h>
#include <userint.h>
#include "ECG_Monitor.h"
#include <NIDAQmx.h>

#define SplitterLeft 597

#define GraphRefreshRate 0.1
#define TimeBase 4

#define STCharFreq 3.8
#define VTCharFreq 2.6
#define BGCharFreq 1.8

int AllocAndCheck (double **, int);
void RemoveDCAndAvg (double *);

static int	MainInterface,	FileModePanel,	DAQModePanel,	DrugLevelPanel,	SettingsPanel,	PSDPanel,	VCGPanel, MenuBar;
int			ActivePanel, 	NextPanel, 		NOE;
int 		FileModeON = 0, DAQModeON = 0;
TaskHandle 	DAQTask;

double	SamplingFreq,	*Lead1Data,	*Lead2Data,	*Lead3Data, *Lead1DataBUS, *Lead2DataBUS, *DAQData;
double	NoiseUC = 52.0, NoiseLC = 48.0;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((MainInterface = LoadPanel (0, "ECG_Monitor.uir", MainInt)) < 0)
		return -1;
	MenuBar			= LoadMenuBar (MainInterface, "ECG_Monitor.uir", MainMenu);
	FileModePanel	= LoadPanel (MainInterface, "ECG_Monitor.uir", FileMode);
	DAQModePanel 	= LoadPanel (MainInterface, "ECG_Monitor.uir", DAQMode);
	SettingsPanel 	= LoadPanel (MainInterface, "ECG_Monitor.uir", Settings);
	PSDPanel 		= LoadPanel (0, "ECG_Monitor.uir", PSD);
	VCGPanel	 	= LoadPanel (0, "ECG_Monitor.uir", VCG);
	DrugLevelPanel 	= LoadPanel (0, "ECG_Monitor.uir", DrugLevel);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, FileModePanel, 1, 0);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, DAQModePanel, 1, 0);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, SettingsPanel, 1, 0);
	SetPanelPos	(FileModePanel,	220, 600);
	SetPanelPos (DAQModePanel, 	220, 600);
	SetPanelPos (SettingsPanel, 220, 600);
	SetGraphCursor	(PSDPanel, PSD_PSDGraph, 1, 48, 0);
	SetGraphCursor	(PSDPanel, PSD_PSDGraph, 2, 52, 0);
	DisplayPanel	(FileModePanel);
	DisplayPanel 	(MainInterface);
	ActivePanel = FileModePanel;
	RunUserInterface ();
	DiscardPanel	(MainInterface);
	DiscardPanel	(PSDPanel);
	DiscardPanel	(VCGPanel);
	DiscardPanel	(DrugLevelPanel);
	return 0;
}
/****************************************************************************************************************
********************************* MainInterface Call Back Functions *********************************************
*****************************************************************************************************************/

int CVICALLBACK MainInt_code (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:
			if(ConfirmPopup("Quit","Are you sure you want to quit ?")) QuitUserInterface (0);
			break;
		}
	return 0;
}

int CVICALLBACK SplitterMove_code (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	static int splitter_fully_receed = 0;
	int splitter_left,panel_width,splitter_pos;
	switch (event)
		{
		case EVENT_TIMER_TICK:
			GetCtrlAttribute	(MainInterface, MainInt_PanelSplitter, ATTR_LEFT, &splitter_left);
			GetPanelAttribute	(MainInterface, ATTR_WIDTH, &panel_width);
			if (!splitter_fully_receed)
			{
				if ((panel_width - splitter_left) > 5)							   //move splitter to the end of screen
					OperateSplitter (MainInterface, MainInt_PanelSplitter, 5);
				else
				{
					splitter_fully_receed = 1;
					HidePanel(FileModePanel);HidePanel(DAQModePanel);HidePanel(SettingsPanel);
					DisplayPanel (NextPanel);
					ActivePanel = NextPanel;
				}
			}
			else if (splitter_fully_receed)
			{
				if (splitter_left > SplitterLeft)								   //move splitter back to original pos
					OperateSplitter (MainInterface, MainInt_PanelSplitter, -5);
				else
				{
					splitter_fully_receed = 0;
					SetCtrlAttribute (MainInterface, MainInt_SplitterMove, ATTR_ENABLED, 0); //timer "self" switch-off
				}
			}
			break;
		}
	return 0;
}

int CVICALLBACK ECGProcessor_code (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	double 	RR_length = 0.0, *pl, *pa, *psd, hr, df, dump, *vcg_x, *vcg_y, *spectrum, *lead1data, *lead2data;
	int		file_handle, i, count = 0;
	char	hr_s[10]= "---" , condition[30] = "---", treatment[30] = "---";
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if (!AllocAndCheck (&lead1data, NOE)) return 0;
			if (!AllocAndCheck (&lead2data, NOE)) return 0;
			Copy1D (Lead1Data, NOE, lead1data);
			Copy1D (Lead2Data, NOE, lead2data);
			if (DAQModeON)
			{
				SetCtrlAttribute	(MainInterface, MainInt_Status, ATTR_CTRL_VAL, "DAQ: Processing...");
				Bw_BSF 				(lead1data, NOE, SamplingFreq, NoiseLC, NoiseUC, 5, lead1data);
				Bw_BSF 				(lead2data, NOE, SamplingFreq, NoiseLC, NoiseUC, 5, lead2data);
				RemoveDCAndAvg 		(lead1data);
				RemoveDCAndAvg 		(lead2data);
			}

			AllocAndCheck	(&spectrum, NOE);
			Copy1D			(Lead2Data, NOE, spectrum);
			Spectrum		(spectrum, NOE);
			DeleteGraphPlot (PSDPanel, PSD_PSDGraph, -1, VAL_IMMEDIATE_DRAW);
			df 				= SamplingFreq/NOE;
			PlotWaveform 	(PSDPanel, PSD_PSDGraph, spectrum, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);


			PeakDetector (lead2data, NOE, 0.65, 3, 0, 1, 1, &count, &pl, &pa, &psd);	//<-- consider decreasing array size here
			if (count < 2) PeakDetector (lead2data, NOE, -0.4, 3, 1, 1, 1, &count, &pl, &pa, &psd);
			if (count < 2) return 0;
			for (i = 0; i<count-1; i++) RR_length += (pl[i+1] - pl[i]);
			RR_length 	/= (count-1);
			hr			= 60/(RR_length*(1/SamplingFreq)) + 0.5;
			Fmt 		(hr_s, "%d", (int)hr);

			vcg_x = calloc	(RR_length,sizeof(double));
			vcg_y = calloc	(RR_length,sizeof(double));
			Subset1D		(lead1data, NOE, (int) pl[0], RR_length, vcg_x);
			Subset1D		(lead2data, NOE, (int) pl[0], RR_length, vcg_y);
			LinEv1D			(vcg_y, RR_length, 2.0, 0.0, vcg_y);
			Sub1D			(vcg_y, vcg_x, RR_length, vcg_y);
			LinEv1D 		(vcg_y, RR_length, (1/1.732), 0.0, vcg_y);
			QScale1D		(vcg_x, RR_length, vcg_x, &dump);
			QScale1D		(vcg_y, RR_length, vcg_y, &dump);
			DeleteGraphPlot (VCGPanel, VCG_VCG, -1, VAL_IMMEDIATE_DRAW);
			PlotXY (VCGPanel, VCG_VCG, vcg_x, vcg_y, RR_length, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_SIMPLE_DOT, VAL_SOLID, 1, VAL_BLACK);


			if ((hr < 30) || (hr > 300)) return 0;
			else if (hr < 60)
			{
				strcpy (condition, "Bradycardia");
				strcpy (treatment, "Administer Bradycardia Drug");
			}
			else if (spectrum[(int)(BGCharFreq/df)] > 0.001) //&& spectrum[(int)(VTandBGCharFreq/df + 0.5)] < 0.01)
			{
				strcpy (condition, "Bigeminus");
				strcpy (treatment, "Intensive Care");
			}
			else if (spectrum[(int)(VTCharFreq/df + 0.5 )] > 0.01)
			{
				strcpy (condition, "Ventricular Tachycardia");
				strcpy (treatment, "Intensive Care");
			}
			else if (hr > 120)
			{
				strcpy (condition, "Tachycardia");
				strcpy (treatment, "Administer Tachycardia Drug");
			}
			else if (spectrum[(int)(STCharFreq/df + 0.5)] > 0.002)
			{
				strcpy (condition, "ST Declined");
				strcpy (treatment, "Intensive Care");
			}
			else if	(spectrum[(int)(STCharFreq /df + 0.5)] > 0.0002 && spectrum[(int)(STCharFreq/df + 0.5)] < 0.001)
			{
				strcpy (condition, "ST Elevated");
				strcpy (treatment, "Intensive Care");
			}
			break;
		default:
			return 0;
	}
	if (strcmp(treatment,"---") == 0 && strcmp(hr_s,"---") != 0) strcpy(condition,"Normal");
	SetCtrlAttribute(MainInterface, MainInt_HeartRate, ATTR_CTRL_VAL, hr_s);
	SetCtrlAttribute(MainInterface, MainInt_Condition, ATTR_CTRL_VAL, condition);
	SetCtrlAttribute(MainInterface, MainInt_Treatment, ATTR_CTRL_VAL, treatment);
	if (FileModeON) SetCtrlAttribute(MainInterface, MainInt_ECGProcessor, ATTR_ENABLED, 0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////MainInterface Panel Related Functions/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CVICALLBACK PanelDisp_code (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
int dump,dump1;														//motherhood function to control which panel to hide
	switch (event)													//when the splitter receeds and which panel to display
		{															//before the splitter starts retreating
		case EVENT_COMMIT:
			GetCtrlAttribute (MainInterface, MainInt_SplitterMove, ATTR_ENABLED, &dump);
			GetCtrlAttribute (MainInterface, control, ATTR_CTRL_VAL, &dump1);
			if (dump)
			{
				SetCtrlAttribute (MainInterface, control, ATTR_CTRL_VAL, !dump1);
				return 0;
			}
			switch (control)
			{
				case MainInt_DAQMode:
					NextPanel = DAQModePanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_FileMode, MainInt_Settings);
					break;
				case MainInt_FileMode:
					NextPanel = FileModePanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_DAQMode, MainInt_Settings);
					break;
				case MainInt_Settings:
					NextPanel = SettingsPanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_FileMode, MainInt_DAQMode);
					break;
			}
				if (NextPanel == ActivePanel)
					SetCtrlAttribute (MainInterface, control, ATTR_CTRL_VAL, 1);
				else
					SetCtrlAttribute (MainInterface, MainInt_SplitterMove, ATTR_ENABLED, 1);
			break;
		}
	return 0;
}

/****************************************************************************************************************
********************************* FileModePanel Call Back Functions *********************************************
*****************************************************************************************************************/

int CVICALLBACK LoadFile_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int choice, file_handle, i;
	char file_path[500];
	switch (event)
		{
		case EVENT_COMMIT:
			choice = FileSelectPopup 	("ECG_DATA", "*.txt", "Data file (*.dat);Text file (*.txt)", "Select File", VAL_LOAD_BUTTON, 0, 0, 1, 1, file_path);
			if 							(choice == VAL_NO_FILE_SELECTED)	return 0;
			else if 					(choice == VAL_NEW_FILE_SELECTED)	return MessagePopup ("Error!", "Select a valid file!");
			file_handle = OpenFile 		(file_path, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
			ScanFile 					(file_handle,"%d", &NOE);
			ScanFile					(file_handle,"%f", &SamplingFreq);
			if (!AllocAndCheck (&Lead1Data, NOE)) return 0;
			if (!AllocAndCheck (&Lead2Data, NOE)) return 0;
			if (!AllocAndCheck (&Lead3Data, NOE)) return 0;
			for (i=0;i<NOE;i++) ScanFile (file_handle, "%s[t10]>%f", &Lead1Data[i]);
			for (i=0;i<NOE;i++) ScanFile (file_handle, "%s[t10]>%f", &Lead2Data[i]);
			Bw_BSF 						(Lead1Data, NOE, SamplingFreq, NoiseLC, NoiseUC, 5, Lead1Data);
			Bw_BSF						(Lead2Data, NOE, SamplingFreq, NoiseLC, NoiseUC, 5, Lead2Data);
			RemoveDCAndAvg 				(Lead1Data);
			RemoveDCAndAvg 				(Lead2Data);
			Sub1D						(Lead2Data, Lead1Data, NOE, Lead3Data);
			DeleteGraphPlot				(MainInterface, MainInt_Lead1, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot				(MainInterface, MainInt_Lead2, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot				(MainInterface, MainInt_Lead3, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform 				(MainInterface, MainInt_Lead1, Lead1Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			PlotWaveform 				(MainInterface, MainInt_Lead2, Lead2Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			PlotWaveform 				(MainInterface, MainInt_Lead3, Lead3Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			SetCtrlAttribute			(MainInterface, MainInt_ECGProcessor, ATTR_ENABLED, 1);
			FileModeON = 1;
			break;
		}
	return 0;
}

int CVICALLBACK SaveToFile_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int choice, file_handle;
	char file_path[500];
	switch (event)
		{
		case EVENT_COMMIT:
			SuspendTimerCallbacks ();
			choice = FileSelectPopup ("ECG_DATA", ".txt", "Data file (*.dat);Text file (*.txt)", "Select/Create File", VAL_SAVE_BUTTON, 0, 0, 1, 1, file_path);
			switch (choice)
			{
				case VAL_EXISTING_FILE_SELECTED:
					if(ConfirmPopup ("Warning!", "Are you sure you want to overwrite the file?")) break;
				case VAL_NO_FILE_SELECTED:
					ResumeTimerCallbacks ();
					return 0;
			}
			file_handle = OpenFile	(file_path, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
			FmtFile 				(file_handle, "%d\n%f\n", NOE, SamplingFreq);
			ArrayToFile 			(file_path, Lead1Data, VAL_DOUBLE, 0, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_APPEND);
			ArrayToFile				(file_path, Lead2Data, VAL_DOUBLE, 0, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_APPEND);
			CloseFile 				(file_handle);
			ResumeTimerCallbacks ();
			break;
		}
	return 0;
}

/****************************************************************************************************************
********************************* DAQModePanel Call Back Functions **********************************************
*****************************************************************************************************************/

int CVICALLBACK DAQOutput_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute		(DAQModePanel, DAQMode_DAQOutput, ATTR_CTRL_VAL, &dump);
			if (dump == 0)
			{
				SetCtrlAttribute	(DAQModePanel, DAQMode_DAQTimer, ATTR_ENABLED, 0);
				DAQmxClearTask		(DAQTask);
				SetCtrlAttribute	(DAQModePanel, DAQMode_SamplingFreq, ATTR_DIMMED, 0);
				SetCtrlAttribute	(FileModePanel, FileMode_LoadFile, ATTR_DIMMED, 0);
				SetCtrlAttribute	(MainInterface, MainInt_ECGProcessor, ATTR_INTERVAL, 0.1);
				SetCtrlAttribute 	(MainInterface, MainInt_ECGProcessor, ATTR_ENABLED, 0);
				SetCtrlAttribute 	(MainInterface, MainInt_Status, ATTR_CTRL_VAL, "");
				DAQModeON = 0;
				return 	0;
			}
			DeleteGraphPlot 			(VCGPanel, VCG_VCG, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot 			(PSDPanel, PSD_PSDGraph, -1, VAL_IMMEDIATE_DRAW);
			SetCtrlAttribute			(MainInterface, MainInt_ECGProcessor, ATTR_INTERVAL, 6.0);
			SetCtrlAttribute 			(DAQModePanel, DAQMode_SamplingFreq, ATTR_DIMMED, 1);
			SetCtrlAttribute 			(FileModePanel, FileMode_LoadFile, ATTR_DIMMED, 1);
			GetCtrlAttribute 			(DAQModePanel, DAQMode_SamplingFreq, ATTR_CTRL_VAL, &SamplingFreq);
			DAQmxCreateTask 			("Get Data", &DAQTask); 											//create and start the daq task
			DAQmxCreateAIVoltageChan 	(DAQTask, "Dev3/ai0", "Lead1", DAQmx_Val_Cfg_Default, -5.0, 5.0, DAQmx_Val_Volts, "");
			DAQmxCreateAIVoltageChan	(DAQTask, "Dev3/ai4", "Lead2", DAQmx_Val_Cfg_Default, -5.0, 5.0,DAQmx_Val_Volts, "");
			if							(!AllocAndCheck(&Lead1Data, 5*SamplingFreq)) return 0;
			if 							(!AllocAndCheck(&Lead2Data, 5*SamplingFreq)) return 0;
			if 							(!AllocAndCheck(&Lead3Data, 5*SamplingFreq)) return 0;
			DAQData = calloc 			(2*(0.1*SamplingFreq), sizeof(double));
			Lead1DataBUS = calloc		(0.1*SamplingFreq, sizeof(double));
			Lead2DataBUS = calloc		(0.1*SamplingFreq, sizeof(double));
			DAQmxCfgSampClkTiming 		(DAQTask, "OnboardClock", SamplingFreq, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 0);
			DAQmxStartTask 				(DAQTask);
			SetCtrlAttribute 			(DAQModePanel, DAQMode_DAQTimer, ATTR_ENABLED, 1);
			SetCtrlAttribute 			(MainInterface, MainInt_ECGProcessor, ATTR_ENABLED, 1);
			DAQModeON = 1;
			break;
		}
	return 0;
}

int CVICALLBACK DAQTimer_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int count;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			SetCtrlAttribute 	(MainInterface, MainInt_Status, ATTR_CTRL_VAL, "DAQ: Acquiring...");
			DAQmxReadAnalogF64 (DAQTask, (0.1*SamplingFreq), -1, DAQmx_Val_GroupByChannel, DAQData, 2*(0.1*SamplingFreq), &count,
								0);
			if (count == 0) 	break;
			Subset1D			(DAQData, 2*(0.1*SamplingFreq), 0, count, Lead1DataBUS);
			Subset1D			(DAQData, 2*(0.1*SamplingFreq), count, count, Lead2DataBUS);
			Shift				(Lead1Data, NOE, count, Lead1Data);
			Shift				(Lead2Data, NOE, count, Lead2Data);
			Reverse				(Lead1DataBUS, 0.1*SamplingFreq, Lead1DataBUS);
			Reverse				(Lead2DataBUS, 0.1*SamplingFreq, Lead2DataBUS);
			Copy1D				(Lead1DataBUS, count, Lead1Data);
			Copy1D				(Lead2DataBUS, count, Lead2Data);
			Sub1D				(Lead2Data, Lead1Data, NOE, Lead3Data);
			DeleteGraphPlot		(MainInterface, MainInt_Lead1, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot		(MainInterface, MainInt_Lead2, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot		(MainInterface, MainInt_Lead3, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform 		(MainInterface, MainInt_Lead1, Lead1Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			PlotWaveform 		(MainInterface, MainInt_Lead2, Lead2Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			PlotWaveform 		(MainInterface, MainInt_Lead3, Lead3Data, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, (1/SamplingFreq), VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);
			break;
	}
	return 0;
}

/****************************************************************************************************************
********************************* DrugLevelPanel Call Back Functions ********************************************
*****************************************************************************************************************/

int CVICALLBACK DrugLevelPanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:
			HidePanel (DrugLevelPanel);
			break;
		}
	return 0;
}
/****************************************************************************************************************
********************************* SettingsPanel Call Back Functions *********************************************
*****************************************************************************************************************/


/****************************************************************************************************************
********************************* PSDPanel Call Back Functions **************************************************
*****************************************************************************************************************/

int CVICALLBACK PSDPanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	char noise[50];
	switch (event)
		{
		case EVENT_CLOSE:
			if (FileModeON)
			{
				DeleteGraphPlot		(MainInterface, MainInt_Lead1, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot		(MainInterface, MainInt_Lead2, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot		(MainInterface, MainInt_Lead3, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot		(PSDPanel, PSD_PSDGraph, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot 	(VCGPanel, VCG_VCG, -1, VAL_IMMEDIATE_DRAW);
				SetCtrlAttribute	(MainInterface, MainInt_HeartRate, ATTR_CTRL_VAL, "---");
				SetCtrlAttribute	(MainInterface, MainInt_Condition, ATTR_CTRL_VAL, "---");
				SetCtrlAttribute	(MainInterface, MainInt_Treatment, ATTR_CTRL_VAL, "---");
				FileModeON = 0;
			}
			sprintf				(noise, "%.1f ± %.1f", (NoiseLC + NoiseUC)/2, NoiseUC - (NoiseLC + NoiseUC)/2);
			SetCtrlAttribute	(SettingsPanel, Settings_Noise, ATTR_CTRL_VAL, noise);
			HidePanel 			(PSDPanel);
			break;
		}
	return 0;
}

int CVICALLBACK PSDGraph_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double x1,x2,dump,temp;
	char noise[50];
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetGraphCursor (PSDPanel, PSD_PSDGraph, 1, &x1, &dump);
			GetGraphCursor (PSDPanel, PSD_PSDGraph, 2, &x2, &dump);
			if (x1 > x2)
			{
				temp = x1;
				x1 = x2;												 //always make x1<x2 and y1<y2
				x2 = temp;
			}
			NoiseLC = x1;
			NoiseUC = x2;
			sprintf (noise, "Noise Bandwidth: %.1f ± %.1f", (NoiseLC + NoiseUC)/2, NoiseUC - (NoiseLC + NoiseUC)/2);
			SetCtrlAttribute (PSDPanel, PSD_Noise_Label, ATTR_CTRL_VAL, noise);
			break;
		}
	return 0;
}

/****************************************************************************************************************
********************************* VCGPanel Call Back Functions **************************************************
*****************************************************************************************************************/

int CVICALLBACK VCGPanel_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:
			HidePanel (VCGPanel);
			break;
		}
	return 0;
}

/****************************************************************************************************************
********************************* MenuBar Call Back Functions ***************************************************
*****************************************************************************************************************/

void CVICALLBACK Quit_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	if (ConfirmPopup ("Quit", "Are you sure you want to quit?"))
		QuitUserInterface (0);
}

void CVICALLBACK MainMenu_PanelDisp_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int control, dump;
	GetCtrlAttribute (MainInterface, MainInt_SplitterMove, ATTR_ENABLED, &dump);
	if (!dump)
	{
		switch (menuItem)
		{
			case MainMenu_Menu_FileMode:
				control		= MainInt_FileMode;
				NextPanel	= FileModePanel;
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_DAQMode, MainInt_Settings);
				break;
			case MainMenu_Menu_DAQMode:
				control		= MainInt_DAQMode;
				NextPanel	= DAQModePanel;
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_FileMode, MainInt_Settings);
				break;
			case MainMenu_Menu_Settings:
				control		= MainInt_Settings;
				NextPanel	= SettingsPanel;
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 2, MainInt_FileMode, MainInt_DAQMode);
				break;


		}
		SetCtrlAttribute (MainInterface, control, ATTR_CTRL_VAL, 1);
		if (NextPanel == ActivePanel)
			SetCtrlAttribute (MainInterface, control, ATTR_CTRL_VAL, 1);
		else
			SetCtrlAttribute (MainInterface, MainInt_SplitterMove, ATTR_ENABLED, 1);
	}
}

void CVICALLBACK Credits_code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n Developed by Rajiv Ram V\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n Developed by Rajiv Ram V\n  rajivram.v@gmail.com\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n  rajivram.v@gmail.com\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
}


/****************************************************************************************************************
********************************* COMMON Call Back Functions ****************************************************
*****************************************************************************************************************/

int CVICALLBACK ShowPanel_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			switch (control)
			{
				case Settings_NoiseChange:
					if (FileModeON && !ConfirmPopup ("Change Noise Bandwidth", "This will unload the current file. Do you wish to continue?")) break;
					DisplayPanel (PSDPanel);
					break;
				case MainInt_VCG:
					DisplayPanel (VCGPanel);
					break;
				case MainInt_DrugLevel:
					DisplayPanel (DrugLevelPanel);
					break;
			}
			break;
		}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Common Functions ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AllocAndCheck (double **array, int elements)
{
	*array = calloc (elements,sizeof(double));
	if (!*array) return (MessagePopup ("Error!", "Either the file is corrupted or there is no sufficient memory!\nCheck the file or try closing some background applications!"));
	NOE = elements;
	return 1;
}

void RemoveDCAndAvg (double *lead_data)
{
	double *dump, mean;
	int i,j;
	AllocAndCheck(&dump, NOE);
	ReFFT (lead_data, dump, NOE);
 	lead_data[0] *= 0; lead_data[NOE-1] *= 0;
	InvFFT (lead_data, dump, NOE);
	for (j = 0; j<50; j++)
	for (i = 1; i<NOE-1; i++) lead_data[i] = (lead_data[i-1] + lead_data[i] + lead_data[i+1])/3;

}
