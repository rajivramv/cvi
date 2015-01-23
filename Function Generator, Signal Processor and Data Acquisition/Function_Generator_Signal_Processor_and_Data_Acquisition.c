#include <formatio.h>
#include <analysis.h>
#include <utility.h>
#include "toolbox.h"
#include <cvirte.h>		
#include <userint.h>
#include <NIDAQmx.h>
#include "Function_Generator_Signal_Processor_and_Data_Acquisition.h"

#define RampWave 1
#define Sawtooth 2
#define Sine 3
#define Square 4
#define Triangular 5
#define RandomNoise 6

#define PeakDetect 1
#define Filter 2

#define AllPeaks 1
#define PositivePeaks 2
#define NegativePeaks 3
#define PeaksInRange 4

#define Butterworth 1
#define Chebyshev 2
#define InvChebyshev 3
#define Elliptical 4

#define LowPass 1
#define HighPass 2
#define BandPass 3
#define BandStop 4

#define SplitterLeft 797
#define CPS 0.05	
 
int CheckSignalGenInputs (double,double);
int CheckFilterInputs (int,double,double);
int AllocAndCheck (double **, int);
void DetectPeaks (int, double, double);
void CheckStateOfOutputs (void);

static int MainInterface, SignalGenPanel, SignalProcPanel, DAQModePanel, FileModePanel, CopySignalPanel, MenuBar, MainSignal = 0;
TaskHandle DAQTask;
int ActivePanel, NextPanel;
int SignalGenSET = 0, SignalProcSET = 0, DAQModeSET = 1, FileModeSET = 1; 
int SignalGenON = 0, SignalProcON = 0, DAQModeON = 0, FileModeON = 0; 
				  
double *Points, *FilteredPoints;
int NOE;

double Amplitude, DCShift, SamplingFreq; 

double TimeBaseScale = 0.1, AmpBaseScale = 0.5, AmpBaseShift = 0.0;

int main (int argc, char *argv[])
{
	int p[3] = {1,2,3}; 
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((MainInterface = LoadPanel (0, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", MainInt)) < 0)
		return -1;
	MenuBar = LoadMenuBar (MainMenu, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", MainMenu);
	SignalGenPanel = LoadPanel (MainInterface, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", SignalGen);
	SignalProcPanel = LoadPanel (MainInterface, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", SignalProc);
	DAQModePanel = LoadPanel (MainInterface, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", DAQMode);
	FileModePanel = LoadPanel (MainInterface, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", FileMode);
	CopySignalPanel = LoadPanel (SignalProcPanel, "Function_Generator_Signal_Processor_and_Data_Acquisition.uir", CopySignal);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, SignalGenPanel, 1, 0);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, SignalProcPanel, 1, 0);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, DAQModePanel, 1, 0);
	AddPanelToSplitter (MainInterface, MainInt_PanelSplitter, VAL_RIGHT_ANCHOR, FileModePanel, 1, 0);
	SetPanelPos (SignalGenPanel, 0, 800);
	SetPanelPos (SignalProcPanel, 0, 800);
	SetPanelPos (DAQModePanel, 0, 800);
	SetPanelPos (FileModePanel, 0, 800);
	SetPanelPos (CopySignalPanel, 131, 0);
	Points = calloc (2, sizeof(double));
	FilteredPoints = calloc (2, sizeof(double));
	DisplayPanel(SignalGenPanel);
	ActivePanel = SignalGenPanel;
	DisplayPanel (MainInterface);
	RunUserInterface ();
	DiscardPanel (MainInterface);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Function to modify controls based on the state of differnet panels////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckStateOfOutputs (void)
{
	
	int dump;
	SetCtrlAttribute (MainInterface, MainInt_SignalGenerator, 	ATTR_TEXT_BOLD, SignalGenON);
	SetCtrlAttribute (MainInterface, MainInt_DAQMode,			ATTR_TEXT_BOLD, DAQModeON);
	SetCtrlAttribute (MainInterface, MainInt_FileMode, 			ATTR_TEXT_BOLD, FileModeON);
	SetCtrlAttribute (MainInterface, MainInt_SignalProcessor,	ATTR_TEXT_BOLD, (SignalProcON && MainSignal));

	SetCtrlAttribute (SignalGenPanel,	SignalGen_Output,		ATTR_DIMMED, !((SignalGenSET && !DAQModeON && !FileModeON) || SignalGenON));
	SetCtrlAttribute (DAQModePanel,		DAQMode_Output,			ATTR_DIMMED, !((DAQModeSET && !SignalGenON && !FileModeON) || DAQModeON));
	SetCtrlAttribute (FileModePanel, 	FileMode_LoadFromFile,	ATTR_DIMMED, !((FileModeSET && !SignalGenON && !DAQModeON) || FileModeON));
	SetCtrlAttribute (SignalProcPanel,	SignalProc_Output,		ATTR_DIMMED, !(SignalProcSET && MainSignal)); 

	ClearListCtrl (FileModePanel, FileMode_SignalSourceToSave);
	InsertListItem (FileModePanel, FileMode_SignalSourceToSave, 0, "<none selected>", 0);
	if (MainSignal)
		InsertListItem (FileModePanel, FileMode_SignalSourceToSave, 1, "Main Signal", 1);
	GetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_CTRL_VAL, &dump);
	if (SignalProcON && !dump)
		InsertListItem (FileModePanel, FileMode_SignalSourceToSave, 2, "Filtered Signal", 2);
	SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, DAQModeON, 3,
						  SignalProc_TEXTMSG, SignalProc_TEXTMSG_2, SignalProc_TEXTMSG_3);
		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////Memory allocation and checking for NULL return if memory is not sufficient//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AllocAndCheck (double **array, int elements)
{
	*array = calloc (elements,sizeof(double));
	if (!Points)
	{
		MessagePopup ("Memory error!", "No sufficient memory! Try closing some background applications and retry!");
		QuitUserInterface (0);
		return 1;
	}
	NOE = elements;
	return 0;
}

/**************************************************************************************************************************************
***************************************************MainInterface callbacks*************************************************************
**************************************************************************************************************************************/

int CVICALLBACK MainInt_code (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:
			if(ConfirmPopup("Quit","Are you sure you want to quit?"))
				QuitUserInterface (0);
			break;
		}
	return 0;
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{											 
	switch (event)
		{
		case EVENT_COMMIT:
			if(ConfirmPopup("Quit","Are you sure you want to quit?"))
				QuitUserInterface (0);
			break;
		}
	return 0;
}

int CVICALLBACK SplitterMove_code (int panel, int control, int event,	  //timer code which moves splitter
		void *callbackData, int eventData1, int eventData2)
{
	static int splitter_fully_receed = 0;
	int splitter_left,panel_width,splitter_pos;
	switch (event)
		{
		case EVENT_TIMER_TICK:
			GetCtrlAttribute (MainInterface, MainInt_PanelSplitter, ATTR_LEFT, &splitter_left);
			GetPanelAttribute (MainInterface, ATTR_WIDTH, &panel_width);
			if (splitter_fully_receed == 0)
			{
				if ((panel_width - splitter_left) > 3)							   //move splitter to the end of screen
					OperateSplitter (MainInterface, MainInt_PanelSplitter, 5);
				else
				{
					splitter_fully_receed = 1;
					HidePanel(SignalGenPanel); HidePanel(SignalProcPanel); HidePanel(DAQModePanel);HidePanel(FileModePanel);	
					DisplayPanel (NextPanel);
					ActivePanel = NextPanel;
				}
			}
			else if (splitter_fully_receed == 1)
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

int CVICALLBACK PanelDisp_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump,dump1;													//motherhood function to control which panel to hide 
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
				case MainInt_SignalGenerator:						  
					NextPanel = SignalGenPanel; 
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalProcessor, MainInt_DAQMode, MainInt_FileMode);
					break;
				case MainInt_SignalProcessor:
					NextPanel = SignalProcPanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_DAQMode, MainInt_FileMode);
					break;
				case MainInt_DAQMode:
					NextPanel = DAQModePanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_SignalProcessor, MainInt_FileMode);
					break;
				case MainInt_FileMode:
					NextPanel = FileModePanel;
					SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_SignalProcessor, MainInt_DAQMode);
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

int CVICALLBACK TimeBase_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{																		//Time base expansion
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (MainInterface, MainInt_TimeBase, ATTR_CTRL_VAL, &TimeBaseScale);
			SetAxisScalingMode (MainInterface, MainInt_SignalGenGraph, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, TimeBaseScale*10);
			SetAxisScalingMode (MainInterface, MainInt_SignalProcGraph, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, TimeBaseScale*10);
			break;
		}
	return 0;
} 
int CVICALLBACK AmpBase_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)				//Controls the Y-axis
{																		//Same code for shifting and expansion in Y-axis
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (MainInterface, MainInt_AmpBase, ATTR_CTRL_VAL, &AmpBaseScale);
			GetCtrlAttribute (MainInterface, MainInt_AmpShift, ATTR_CTRL_VAL, &AmpBaseShift);
			SetAxisScalingMode (MainInterface, MainInt_SignalGenGraph, VAL_LEFT_YAXIS, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
			SetAxisScalingMode (MainInterface, MainInt_SignalProcGraph, VAL_LEFT_YAXIS, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
			break;
		}
	return 0;
}
int CVICALLBACK ZoomType_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int state;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (MainInterface, MainInt_ZoomType, ATTR_CTRL_VAL, &state);
			SetAttributeForCtrls (MainInterface, ATTR_DIMMED, !state, 4, MainInt_HUD, MainInt_TimeBase, MainInt_AmpBase, MainInt_AmpShift);
			SetAttributeForCtrls (MainInterface,  ATTR_ENABLE_ZOOM_AND_PAN, !state, 2, MainInt_SignalGenGraph, MainInt_SignalProcGraph);
			SetAttributeForCtrls (MainInterface, ATTR_VISIBLE, !state, 4, MainInt_MainSignalZoomDiv, MainInt_MainSignalZoomIn, MainInt_MainSignalZoomOut, MainInt_MainSignalZoomReset);
			SetAttributeForCtrls (MainInterface, ATTR_VISIBLE, !state, 4, MainInt_ProcSignalZoomDiv, MainInt_ProcSignalZoomIn, MainInt_ProcSignalZoomOut, MainInt_ProcSignalZoomReset);
			if (state == 1)
			{
				SetAxisRange (MainInterface, MainInt_SignalGenGraph, VAL_MANUAL, 0, TimeBaseScale*10, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
				SetAxisRange (MainInterface, MainInt_SignalProcGraph, VAL_MANUAL, 0, TimeBaseScale*10, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
			}
			break;
		}
	return 0;
}

int CVICALLBACK ZoomIn_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double x1,x2,y1,y2,temp;
	int graph;
	switch (event)
		{
		case EVENT_COMMIT:
			if (control == MainInt_MainSignalZoomIn) graph = MainInt_SignalGenGraph;
			else graph = MainInt_SignalProcGraph;
			GetGraphCursor (MainInterface, graph, 1, &x1, &y1);
			GetGraphCursor (MainInterface, graph, 2, &x2, &y2);
			if (x1 > x2)
			{
				temp = x1;
				x1 = x2;												 //always make x1<x2 and y1<y2
				x2 = temp;
			}
			if (y1 > y2)
			{
				temp = y1;
				y1 = y2;
				y2 = temp;
			}
			SetAxisRange (MainInterface, graph, VAL_MANUAL, x1, x2, VAL_MANUAL, y1, y2);
			break;
		}
	return 0;
}

int CVICALLBACK ZoomOut_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double x1,x2,y1,y2,temp;
	int graph;
	switch (event)
		{
		case EVENT_COMMIT:
			if (control == MainInt_MainSignalZoomOut) graph = MainInt_SignalGenGraph;
			else graph = MainInt_SignalProcGraph;
			GetAxisRange (MainInterface, graph, NULL, &x1, &x2, NULL, &y1, &y2);
			x1 = 2*x1 - x2;
			x2 = 2*x2 - x1;
			if (x1 < 0) x1 = 0;
			if (x2 > 10) x2 = 10;										// zoom out so that the current scale range 
			y1 = 2*y1 - y2;												//becomes one third of the new range
			y2 = 2*y2 - y1;
			if (y1 < -20) y1 = -20;
			if (y2 > 20) y2 = 20;
			SetAxisRange (MainInterface, graph, VAL_MANUAL, x1, x2, VAL_MANUAL, y1, y2);
			break;
		}
	return 0;
}

int CVICALLBACK ZoomReset_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int graph;
	switch (event)
		{
		case EVENT_COMMIT:
			if (control == MainInt_MainSignalZoomReset) graph = MainInt_SignalGenGraph;
			else graph = MainInt_SignalProcGraph;
			SetAxisRange (MainInterface, graph, VAL_MANUAL, 0, TimeBaseScale*10, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
			SetAxisRange (MainInterface, graph, VAL_MANUAL, 0, TimeBaseScale*10, VAL_MANUAL, (AmpBaseShift-1)*AmpBaseScale*2,(AmpBaseShift+1)*AmpBaseScale*2);
			break;
		}
	return 0;
}

/**************************************************************************************************************************************
***************************************************SignalGenPanel callbacks************************************************************
**************************************************************************************************************************************/

int CVICALLBACK SignalWaveForm_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int wave, pos = 120;
	switch (event)													
		{													  //Make visible only necessary function attributes
		case EVENT_VAL_CHANGED:
			SignalGenSET = 1;
			SignalGenON = 0;
			MainSignal = 0;
			SetCtrlAttribute (SignalGenPanel, SignalGen_Output, ATTR_CTRL_VAL, 0); 
			GetCtrlVal (SignalGenPanel, SignalGen_SignalWaveForm, &wave);
			switch (wave)
			{
				case 0:
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 0, 6, SignalGen_Slope, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift, SignalGen_DutyCycle);
					DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
					SignalGenSET = 0;
					break;
				case RampWave:
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 0, 4, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DutyCycle);
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 1, 2, SignalGen_Slope, SignalGen_DCShift);
					DistributeCtrls (SignalGenPanel, VAL_TB_VERTICAL_DISTRIBUTION, VAL_TB_FIXED_GAP_SPACING, &pos, 20, 2,SignalGen_Slope, SignalGen_DCShift);
					break;
				case Sawtooth:
				case Sine:
				case Triangular:
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 0, 2, SignalGen_Slope, SignalGen_DutyCycle); 
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 1, 4, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift);
					DistributeCtrls (SignalGenPanel, VAL_TB_VERTICAL_DISTRIBUTION, VAL_TB_FIXED_GAP_SPACING, &pos, 20, 4, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift);
					break;
				case Square:
					SetCtrlAttribute (SignalGenPanel, SignalGen_Slope, ATTR_VISIBLE, 0);
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 1, 5, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift, SignalGen_DutyCycle);
					DistributeCtrls (SignalGenPanel, VAL_TB_VERTICAL_DISTRIBUTION, VAL_TB_FIXED_GAP_SPACING, &pos, 20, 5, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift, SignalGen_DutyCycle);
					break;	
				case RandomNoise:
					SetAttributeForCtrls (SignalGenPanel, ATTR_VISIBLE, 0, 6, SignalGen_Slope, SignalGen_Amplitude, SignalGen_Frequency, SignalGen_Phase, SignalGen_DCShift, SignalGen_DutyCycle);
					break;
			}
			CheckStateOfOutputs (); 
		break;  
		}
	return 0;
}

int CVICALLBACK SignalGenOutput_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double slope = 1.0, frequency, phase, duty_cycle;
	int dump, waveform, i;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (SignalGenPanel, SignalGen_Output, ATTR_CTRL_VAL, &dump);
			if (dump == 0)
			{
				DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
				SignalGenON = 0;
				CheckStateOfOutputs ();
				MainSignal = 0;
				return 0;
			}
			GetCtrlAttribute (SignalGenPanel, SignalGen_SignalWaveForm, ATTR_CTRL_VAL, &waveform);
			GetCtrlAttribute (SignalGenPanel, SignalGen_Amplitude, ATTR_CTRL_VAL, &Amplitude);
			GetCtrlAttribute (SignalGenPanel, SignalGen_Frequency, ATTR_CTRL_VAL, &frequency);
			GetCtrlAttribute (SignalGenPanel, SignalGen_Phase, ATTR_CTRL_VAL, &phase);
			GetCtrlAttribute (SignalGenPanel, SignalGen_DCShift, ATTR_CTRL_VAL, &DCShift);
			GetCtrlAttribute (SignalGenPanel, SignalGen_DutyCycle, ATTR_CTRL_VAL, &duty_cycle);
			SetCtrlAttribute (MainInterface, MainInt_SignalGenerator, ATTR_TEXT_BOLD, 1);
			switch (waveform)
			{
				case RampWave:
					GetCtrlAttribute (SignalGenPanel, SignalGen_Slope, ATTR_CTRL_VAL, &slope);
					if (slope == 0)
					{
						MessagePopup ("Error!", "\a Slope can not be zero!");
						return 0;
					}
					frequency = 1.0;
					SamplingFreq = frequency/CPS;
					if (AllocAndCheck (&Points, (10*SamplingFreq+1))) return 0;		  //Generate arrays of required size
					break;
				case RandomNoise:
					frequency = 10.0;
					SamplingFreq = frequency/CPS;
					if (AllocAndCheck (&Points, (10*SamplingFreq+1))) return 0;
					break;
				case Sawtooth:
				case Sine:
				case Square:
				case Triangular:
					if (CheckSignalGenInputs (Amplitude,frequency)) return 0;
					SamplingFreq = frequency/CPS;
					if (AllocAndCheck (&Points, (10*SamplingFreq+1))) return 0;  
			}	 
			SignalGenON = 1;
			MainSignal = 1;
			switch (waveform)
			{
				case RampWave:
					for (i = 0;i<NOE;i++) Points[i] = i*CPS;
					break;
				case Sawtooth:
					SawtoothWave (NOE, Amplitude, CPS, &phase, Points);
					break;															  //Generate the function
				case Sine:
					SineWave (NOE, Amplitude, CPS, &phase, Points);
					break;
				case Square:
					SquareWave (NOE, Amplitude, CPS, &phase, duty_cycle, Points);
					break;
				case Triangular:
					TriangleWave (NOE, Amplitude, CPS, &phase, Points);
					break;
				case RandomNoise:
					Uniform (NOE, Random(0,100), Points);
					break;
			}
			LinEv1D (Points, NOE, slope, DCShift, Points);
			DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform (MainInterface, MainInt_SignalGenGraph, Points, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE,
						  VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			CheckStateOfOutputs ();
			break;
			
		}
	return 0;
}
 
int CVICALLBACK SignalGenOFF_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
			SignalGenON = 0;
			CheckStateOfOutputs ();
			MainSignal = 0;
			SetCtrlAttribute (SignalGenPanel, SignalGen_Output, ATTR_CTRL_VAL, 0);  
			break;
		}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////SignalGenPanel related functions////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CheckSignalGenInputs (double amplitude, double frequency)
{
	char error_msg[40] = "";
	if (amplitude == 0.0) strcpy (error_msg,"\a Check amplitude!\n");	 
	if (frequency == 0.0) strcat (error_msg,"\a Check frequency!\n");
	if (strcmp(error_msg,"")!=0)												//Check the function generator parameters
	{
		MessagePopup("Error!",error_msg);
		SetCtrlAttribute (SignalGenPanel, SignalGen_Output, ATTR_CTRL_VAL, 0);
		return 1;
	}
	return 0;
}  

/**************************************************************************************************************************************
***************************************************SignalProcPanel callbacks************************************************************
**************************************************************************************************************************************/

int CVICALLBACK PeakDetector_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int pd;
	switch (event)														  //Dim/Visible appropriate controls
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_CTRL_VAL, &pd);
			SignalProcSET  = 1;
			switch (pd)
			{
				case 0:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 0, 2, SignalProc_UpperThreshold, SignalProc_LowerThreshold);
					SetCtrlAttribute (SignalProcPanel, SignalProc_FilterMethod, ATTR_DIMMED, 0);
					SignalProcSET = 0;
					SignalProcON = 0;
					SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
					DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
					SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
					break;
				case PeaksInRange:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 1, 2, SignalProc_UpperThreshold, SignalProc_LowerThreshold);
					SetCtrlAttribute (SignalProcPanel, SignalProc_FilterMethod, ATTR_DIMMED, 1);
					break;
				case AllPeaks:
				case PositivePeaks:
				case NegativePeaks:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 0, 2, SignalProc_UpperThreshold, SignalProc_LowerThreshold);
					SetCtrlAttribute (SignalProcPanel, SignalProc_FilterMethod, ATTR_DIMMED, 1);
					break;
			}
			SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
			SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
			SignalProcON = 0;
			CheckStateOfOutputs ();
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

int CVICALLBACK FilterMethod_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int fm;																		 //Dim/Visible appropriate controls
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (SignalProcPanel, SignalProc_FilterMethod, ATTR_CTRL_VAL, &fm);
			switch (fm)
			{
				case 0:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 0, 5, SignalProc_FilterType, SignalProc_FilterOrder, SignalProc_CutOff, SignalProc_LowerCutOff, SignalProc_HigherCutOff);
					SetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_DIMMED, 0);
					SetCtrlAttribute (SignalProcPanel, SignalProc_FilterType, ATTR_CTRL_VAL, 0);
					SignalProcSET = 0;
					SignalProcON = 0;
					SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
					DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
					SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
					SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
					HidePanel (CopySignalPanel);
					break;
				case Butterworth:
				case Chebyshev:
				case InvChebyshev:
				case Elliptical:
					SetCtrlAttribute (SignalProcPanel, SignalProc_FilterType, ATTR_VISIBLE, 1);
					SetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_DIMMED, 1);
					break;
			}
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

int CVICALLBACK FilterType_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int ft, pos = 320, sig_gen, fm, daq;									   //Dim/Visible appropriate controls
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			SignalProcSET = 1;
			GetCtrlAttribute (SignalProcPanel, SignalProc_FilterType, ATTR_CTRL_VAL, &ft);
			switch (ft)
			{
				case 0:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 0, 4, SignalProc_FilterOrder, SignalProc_CutOff, SignalProc_LowerCutOff, SignalProc_HigherCutOff);
					SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_DIMMED, 1);
					SignalProcSET = 0;
					SignalProcON = 0;
					SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
					DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
					SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
					SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
					HidePanel (CopySignalPanel);
					break;
				case LowPass:
				case HighPass:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 1, 2, SignalProc_FilterOrder, SignalProc_CutOff);
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 0, 2, SignalProc_LowerCutOff, SignalProc_HigherCutOff);
					DistributeCtrls (SignalProcPanel, VAL_TB_VERTICAL_DISTRIBUTION, VAL_TB_FIXED_GAP_SPACING, &pos, 7, 2,
									 SignalProc_FilterOrder, SignalProc_CutOff);
					break;
				case BandPass:
				case BandStop:
					SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 1, 3, SignalProc_FilterOrder, SignalProc_LowerCutOff, SignalProc_HigherCutOff);
					SetCtrlAttribute (SignalProcPanel, SignalProc_CutOff, ATTR_VISIBLE, 0);
					DistributeCtrls (SignalProcPanel, VAL_TB_VERTICAL_DISTRIBUTION, VAL_TB_FIXED_GAP_SPACING, &pos, 7, 3, SignalProc_FilterOrder, SignalProc_LowerCutOff, SignalProc_HigherCutOff);
					break;	 
			}
			CheckStateOfOutputs ();
		break;
		}
	return 0;
}

int CVICALLBACK SignalProcOutput_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump, PD_or_F = 0, PD_type = 0, F_method = 0, F_type = 0, filter_order;
	double cut_off, lower_cut_off, higher_cut_off; 
	switch (event)
	{																			  //Activate timer of the processor
		case EVENT_COMMIT:
			GetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, &dump);
			if (dump == 0)
			{
				DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
				SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
				SignalProcON = 0; 
				SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
				HidePanel (CopySignalPanel);
			}
			else 
			{
				SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 1);
			}
			CheckStateOfOutputs ();
			break;
	}
	return 0;
}
int CVICALLBACK SignalProcTimer_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump, PD_or_F = 0, PD_type = 0, F_method = 0, F_type = 0, filter_order;
	double cut_off, lower_cut_off, higher_cut_off; 
	switch (event)
		{														   //perform necessary processor function
		case EVENT_TIMER_TICK:
			if (!MainSignal)
			{
				DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
				SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
				SignalProcON = 0; 
				SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
				HidePanel (CopySignalPanel);   
				SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
				CheckStateOfOutputs ();
				return 0;
			}		
			GetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_CTRL_VAL, &dump);
			if (dump != 0) PD_or_F = PeakDetect;
			else PD_or_F = Filter;
			switch (PD_or_F)
			{
				case PeakDetect:
					GetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_CTRL_VAL, &PD_type);
					break;
				case Filter:
					GetCtrlAttribute (SignalProcPanel, SignalProc_FilterMethod, ATTR_CTRL_VAL, &F_method);
					GetCtrlAttribute (SignalProcPanel, SignalProc_FilterType, ATTR_CTRL_VAL, &F_type); 
					break;
			}
			switch (PD_or_F)
			{
				case PeakDetect:
					switch (PD_type)
					{
						case AllPeaks:
							DetectPeaks (AllPeaks,0,0);
							return 0;
						case PositivePeaks:
							DetectPeaks (PositivePeaks,0,0);
							return 0;
						case NegativePeaks:
							DetectPeaks (NegativePeaks,0,0);
							return 0;
						case PeaksInRange:
							GetCtrlAttribute (SignalProcPanel, SignalProc_UpperThreshold, ATTR_CTRL_VAL, &higher_cut_off);
							GetCtrlAttribute (SignalProcPanel, SignalProc_LowerThreshold, ATTR_CTRL_VAL, &lower_cut_off);
							DetectPeaks (PeaksInRange, lower_cut_off, higher_cut_off);
							return 0;
					}
					break;
				case Filter:
					GetCtrlAttribute (SignalProcPanel, SignalProc_FilterOrder, ATTR_CTRL_VAL, &filter_order);
					GetCtrlAttribute (SignalProcPanel, SignalProc_CutOff, ATTR_CTRL_VAL, &cut_off);
					GetCtrlAttribute (SignalProcPanel, SignalProc_LowerCutOff, ATTR_CTRL_VAL, &lower_cut_off);
					GetCtrlAttribute (SignalProcPanel, SignalProc_HigherCutOff, ATTR_CTRL_VAL, &higher_cut_off);
					switch (F_type)
					{
						case LowPass:
						case HighPass:									 
							if (CheckFilterInputs(HighPass,cut_off,2*cut_off+1)) return 0;
							if (AllocAndCheck (&FilteredPoints, (10*SamplingFreq+1))) return 0;
							break;
						case BandPass:
						case BandStop:
							if (CheckFilterInputs(BandStop,lower_cut_off,higher_cut_off)) return 0;
							if (AllocAndCheck (&FilteredPoints, (10*SamplingFreq+1))) return 0;
							break;
					}
					switch (F_method)
					{
						case Butterworth:
							switch (F_type)
							{
								case LowPass:
									Bw_LPF (Points, NOE, SamplingFreq, cut_off, filter_order, FilteredPoints);
									break;
								case HighPass:									 
									Bw_HPF (Points, NOE, SamplingFreq, cut_off, filter_order, FilteredPoints);
									break;
								case BandPass:
									Bw_BPF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, filter_order, FilteredPoints);
									break;
								case BandStop:
									Bw_BSF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, filter_order, FilteredPoints);
									break;
							}
							break;
						case Chebyshev:
							switch (F_type)
							{
								case LowPass:
									Ch_LPF (Points, NOE, SamplingFreq, cut_off, 0.1, filter_order, FilteredPoints);
									break;
								case HighPass:									 
									Ch_HPF (Points, NOE, SamplingFreq, cut_off, 0.1, filter_order, FilteredPoints);
									break;
								case BandPass:
									Ch_BPF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, 0.1, filter_order, FilteredPoints);
									break;
								case BandStop:
									Ch_BSF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, 0.1, filter_order, FilteredPoints);
									break;
							}
							break;
						case InvChebyshev:
							switch (F_type)
							{
								case LowPass:
									InvCh_LPF (Points, NOE, SamplingFreq, cut_off, 40.0, filter_order, FilteredPoints);
									break;
								case HighPass:
									InvCh_HPF (Points, NOE, SamplingFreq, cut_off, 40.0, filter_order, FilteredPoints);
									break;
								case BandPass:
									InvCh_BPF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, 40.0, filter_order, FilteredPoints);
									break;
								case BandStop:
									InvCh_BSF (Points, NOE, SamplingFreq, lower_cut_off, higher_cut_off, 40.0, filter_order, FilteredPoints);
									break;
							}
							break;
						case Elliptical:
							switch (F_type)
							{
								case LowPass:
									Elp_LPF (Points, NOE, 5*cut_off, cut_off, 0.1, 40.0, filter_order, FilteredPoints);
									break;
								case HighPass:
									Elp_HPF (Points, NOE, 5*cut_off, cut_off, 0.1, 40.0, filter_order, FilteredPoints);
									break;
								case BandPass:
									Elp_BPF (Points, NOE, 5*higher_cut_off, lower_cut_off, higher_cut_off, 0.1, 40.0, filter_order, FilteredPoints);
									break;
								case BandStop:
									Elp_BSF (Points, NOE, 5*higher_cut_off, lower_cut_off, higher_cut_off, 0.1, 40.0, filter_order, FilteredPoints);
									break;
							}
							break;
					}
					break;
			}
			DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform (MainInterface, MainInt_SignalProcGraph, FilteredPoints, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 1);
			DisplayPanel (CopySignalPanel);
			SignalProcON = 1;
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

int CVICALLBACK LowerThreshold_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double dump;
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (SignalProcPanel, SignalProc_LowerThreshold, ATTR_CTRL_VAL, &dump);
			SetCtrlAttribute (SignalProcPanel, SignalProc_UpperThreshold, ATTR_MIN_VALUE, dump+0.1);
			break;
		}
	return 0;
}

int CVICALLBACK UpperThreshold_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (SignalProcPanel, SignalProc_UpperThreshold, ATTR_CTRL_VAL, &dump);
			SetCtrlAttribute (SignalProcPanel, SignalProc_LowerThreshold, ATTR_MAX_VALUE, dump-0.1);
			
			break;
		}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////SignalProcPanel related functions////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CheckFilterInputs (int filter_type, double lower_cut_off, double higher_cut_off)		//Check processor inputs
{
	char error_msg[250] = "", nyquist_error[200];
	if (lower_cut_off == 0 || higher_cut_off == 0) strcat (error_msg,"\a Cut-off frequency can not be zero!\n");
	if (lower_cut_off >= higher_cut_off) strcat (error_msg,"\a Lower cut-off cannot be greater than higher cut-off!\n");
	if (filter_type == BandPass || filter_type == BandStop)
	{
		if (higher_cut_off > (SamplingFreq/2) || lower_cut_off > (SamplingFreq/2))
		{
			sprintf(nyquist_error,"\a Nyquist criteria violated! Check cut-off frequency!\n The signal is being sampled at %.2f Hz. (Signal freq./Sampling freq = %1.2e)\n Cut-off frequency < %.2f.\n", SamplingFreq, CPS, SamplingFreq/2); 
			strcat(error_msg,nyquist_error);
		}
	}
	else if (filter_type == LowPass || filter_type == HighPass) 
	{
		if (lower_cut_off > (SamplingFreq/2))
		{
			sprintf(nyquist_error,"\a Nyquist criteria violated! Check cut-off frequency!\n The signal is being sampled at %.2f Hz. (Signal freq./Sampling freq = %1.2e)\n Cut-off frequency < %.2f.\n", SamplingFreq, CPS, SamplingFreq/2); 
			strcat(error_msg,nyquist_error);
		}
	}
	if (strcmp(error_msg,"")!=0)
	{
		MessagePopup("Error!",error_msg);
		DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
		SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
		SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
		SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
		HidePanel (CopySignalPanel);
		SignalProcON = 0;
		CheckStateOfOutputs ();
		return 1;
	}
	return 0;
}  
void DetectPeaks (int type, double lower_thr, double high_thr)
{
	int i, count;																 //comprehensive peak detector
	double *peak_loc, *peak_amp, *peak_sd;
	DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
	PlotWaveform (MainInterface, MainInt_SignalProcGraph, Points, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE,VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
	switch (type)
	{
		case AllPeaks:
			PeakDetector (Points, NOE, NegativeInfinity (), 3, 0, 1, 1, &count, &peak_loc, &peak_amp, &peak_sd);
			for (i=0;i<count;i++) 
				PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			PeakDetector (Points, NOE, PositiveInfinity (), 3, 1, 1, 1, &count, &peak_loc, &peak_amp,
						  &peak_sd);
			for (i=0;i<count;i++) 
				PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			break;
		case PositivePeaks:
			PeakDetector (Points, NOE, NegativeInfinity (), 4, 0, 1, 1, &count, &peak_loc, &peak_amp, &peak_sd);
			for (i=0;i<count;i++) 
				PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			break;
		case NegativePeaks:
			PeakDetector (Points, NOE, PositiveInfinity (), 3, 1, 1, 1, &count, &peak_loc, &peak_amp, &peak_sd);
			for (i=0;i<count;i++) 
				PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			break;
		case PeaksInRange:
			PeakDetector (Points, NOE, NegativeInfinity (), 3, 0, 1, 1, &count, &peak_loc, &peak_amp, &peak_sd);
			for (i=0;i<count;i++) 
			{
				if (peak_amp[i] > lower_thr && peak_amp[i]< high_thr)
					PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			
			}
			PeakDetector (Points, NOE, PositiveInfinity (), 3, 1, 1, 1, &count, &peak_loc, &peak_amp, &peak_sd);
			for (i=0;i<count;i++) 
			{	
				if (peak_amp[i] > lower_thr && peak_amp[i]< high_thr)  
					PlotPoint (MainInterface, MainInt_SignalProcGraph, peak_loc[i]*(1/SamplingFreq), peak_amp[i], VAL_X, VAL_BLUE);
			}
			break;
	}
	SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
	SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
	SignalProcON = 0;
	CheckStateOfOutputs ();
}

/**************************************************************************************************************************************
***************************************************DAQModePanel callbacks************************************************************
**************************************************************************************************************************************/

int CVICALLBACK DAQOutput_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (DAQModePanel, DAQMode_Output, ATTR_CTRL_VAL, &dump);
			if (dump == 0)
			{
				SetCtrlAttribute (DAQModePanel, DAQMode_RefreshGraph, ATTR_ENABLED, 0); 				
				DAQmxClearTask (DAQTask);
				SetCtrlAttribute (DAQModePanel, DAQMode_SamplingFreq, ATTR_DIMMED, 0);
				DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
				SetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_DIMMED, 1);
				SetAttributeForCtrls (SignalProcPanel, ATTR_VISIBLE, 1, 3, SignalProc_TEXTMSG, SignalProc_TEXTMSG_2, SignalProc_TEXTMSG_3);
				DAQModeON = 0;
				MainSignal = 0;
				CheckStateOfOutputs ();
				return 0;														//create and start the daq task
			}
			SetCtrlAttribute (DAQModePanel, DAQMode_SamplingFreq, ATTR_DIMMED, 1);
			GetCtrlAttribute (DAQModePanel, DAQMode_SamplingFreq, ATTR_CTRL_VAL, &SamplingFreq);
			DAQmxCreateTask ("Get Data", &DAQTask);
			DAQmxCreateAIVoltageChan (DAQTask, "Dev2/ai0", "Channel0", DAQmx_Val_Cfg_Default, -5.0, 5.0,
									  DAQmx_Val_Volts, "");
			if (AllocAndCheck (&Points, (10*SamplingFreq+1))) return 0; 
			DAQmxCfgSampClkTiming (DAQTask, "OnboardClock", SamplingFreq, DAQmx_Val_Rising, DAQmx_Val_ContSamps,2*(0.5*SamplingFreq));
			DAQmxStartTask (DAQTask);
			SetCtrlAttribute (DAQModePanel, DAQMode_RefreshGraph, ATTR_ENABLED, 1);
			DAQModeON = 1;
			MainSignal = 1;
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

int CVICALLBACK RefreshGraph_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int count;
	double *points;																//read the buffer and plot 
	points = calloc (2*(0.5*SamplingFreq),sizeof(double));
	switch (event)
		{
		case EVENT_TIMER_TICK:
			SetCtrlAttribute (SignalProcPanel, SignalProc_PeakDetector, ATTR_DIMMED, 1);
			DAQmxReadAnalogF64 (DAQTask, DAQmx_Val_Auto, DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByChannel, points,
								2*(0.5*SamplingFreq), &count, 0);
			if (count)
			{
				Shift (Points, NOE, count, Points);
				Copy1D (points, count, Points);
			}
			DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform (MainInterface, MainInt_SignalGenGraph, Points, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			break;
		}
	return 0;
}

/*************************************************************************************************************************************
***************************************************FileModePanel callbacks************************************************************
**************************************************************************************************************************************/

int CVICALLBACK LoadFromFile_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double *x_axis;
	int status,file_handle,i;
	char load_path[300],dump[50];											  //load file following file select pop up
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (FileModePanel, FileMode_LoadFromFile, ATTR_CTRL_VAL, &i);
			if (!i)
			{
				DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
				FileModeON = 0;
				MainSignal = 0;
				CheckStateOfOutputs ();
				return 0;
			}
			status = FileSelectPopup ("", "*.txt", "Text File (*.txt);DAT File (*.dat)", "Load from file", VAL_LOAD_BUTTON, 0, 0, 1, 0, load_path);
			if (status == VAL_NO_FILE_SELECTED)
			{
				SetCtrlAttribute (FileModePanel, FileMode_LoadFromFile, ATTR_CTRL_VAL, 0);
				return 0;
			}
			file_handle = OpenFile (load_path, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
			ScanFile (file_handle, "%d", &NOE);
			AllocAndCheck (&Points,NOE);
			ScanFile (file_handle, "%f", &SamplingFreq);
			for (i = 0; i < NOE; i++)
				ScanFile (file_handle, "%f", &Points[i]);
			CloseFile (file_handle);
			DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform (MainInterface, MainInt_SignalGenGraph, Points, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
						  VAL_SOLID, 1, VAL_RED);
			FileModeON = 1;
			MainSignal = 1;
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

int CVICALLBACK SaveToFile_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dump, file_handle, status, i;
	double *points;
	char load_path[300];
	switch (event)
		{																	    
		case EVENT_COMMIT:
			GetCtrlAttribute (FileModePanel, FileMode_SignalSourceToSave, ATTR_CTRL_VAL, &dump);
			status = FileSelectPopup ("", "*.txt", "Text File (*.txt);DAT File (*.dat)", "Save to file", VAL_SAVE_BUTTON, 0, 0, 1, 1,
									  load_path);
			if (status == VAL_NO_FILE_SELECTED)
				break;
			file_handle = OpenFile (load_path, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
			FmtFile (file_handle, "%d\n", NOE);
			FmtFile (file_handle, "%f\n", SamplingFreq);								
			if (DAQModeON) SetCtrlAttribute (DAQModePanel, DAQMode_RefreshGraph, ATTR_ENABLED, 0);
			if (SignalProcON) SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
			AllocAndCheck (&points,NOE);				//to avoid the situation 
			if (dump == 1)								//where the global variables    --> guessing that this situation
				Copy1D (Points, NOE, points);			//Points or FilteredPoints is	  	would arise
			else if (dump == 2)							//flushed during writing to file
				Copy1D (FilteredPoints, NOE, points);
			for (i=0;i<NOE;i++)
					FmtFile (file_handle, "%f\n", points[i]);
			if (DAQModeON) SetCtrlAttribute (DAQModePanel, DAQMode_RefreshGraph, ATTR_ENABLED, 1);
			if (SignalProcON) SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 1);
			CloseFile (file_handle);
			break;
		}
	return 0;
}

/*************************************************************************************************************************************
***************************************************CopySignalPanel callbacks************************************************************
**************************************************************************************************************************************/

int CVICALLBACK CopySignal_code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)												   //copies "FILTERED" signal to main signal
		{
		case EVENT_COMMIT:
			Copy1D (FilteredPoints, NOE, Points);
			DeleteGraphPlot (MainInterface, MainInt_SignalProcGraph, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot (MainInterface, MainInt_SignalGenGraph, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform (MainInterface, MainInt_SignalGenGraph, Points, NOE, VAL_DOUBLE, 1.0, 0.0, 0.0, 1/SamplingFreq, VAL_THIN_LINE,
						  VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			SetCtrlAttribute (SignalProcPanel, SignalProc_Splitter_2, ATTR_VISIBLE, 0);
			HidePanel (CopySignalPanel);
			SetCtrlAttribute (SignalGenPanel, SignalGen_Output, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (SignalProcPanel, SignalProc_Output, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (DAQModePanel, DAQMode_Output, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (FileModePanel, FileMode_LoadFromFile, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (SignalProcPanel, SignalProc_SignalProcTimer, ATTR_ENABLED, 0);
			SetCtrlAttribute (DAQModePanel, DAQMode_RefreshGraph, ATTR_ENABLED, 0); 				
			DAQmxClearTask (DAQTask);
			SetCtrlAttribute (DAQModePanel, DAQMode_SamplingFreq, ATTR_DIMMED, 0);
			SignalGenON = 0;
			SignalProcON = 0;
			DAQModeON = 0;
			FileModeON = 0;
			MainSignal = 1;
			CheckStateOfOutputs ();
			break;
		}
	return 0;
}

/*************************************************************************************************************************************
***************************************************MenuBar callbacks************************************************************
**************************************************************************************************************************************/

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
			case MainMenu_File_SignalGenerator:						  
				control = MainInt_SignalGenerator;
				NextPanel = SignalGenPanel; 
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalProcessor, MainInt_DAQMode, MainInt_FileMode);
				break;
			case MainMenu_File_SignalProcessor:
				control = MainInt_SignalProcessor;
				NextPanel = SignalProcPanel;
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_DAQMode, MainInt_FileMode);
				break;
			case MainMenu_File_DAQMode:
				control = MainInt_DAQMode;
				NextPanel = DAQModePanel;
				SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_SignalProcessor, MainInt_FileMode);
				break;
			case MainMenu_File_FileMode:
				control = MainInt_FileMode;
				NextPanel = FileModePanel;
			SetAttributeForCtrls (MainInterface, ATTR_CTRL_VAL, 0, 3, MainInt_SignalGenerator, MainInt_SignalProcessor, MainInt_DAQMode);
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
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n           Developed by Rajiv Ram V\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n           Developed by Rajiv Ram V\n              rajivram.v@gmail.com\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n              rajivram.v@gmail.com\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
	MessagePopup ("Credits!", "``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``\n\n\n``'*'``'*'``'*'``'*'``'*'``'*'``'*'``'*'``");
}
