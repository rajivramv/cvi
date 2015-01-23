#include <formatio.h>
#include <analysis.h>
#include "toolbox.h"
#include <utility.h>
#include <cvirte.h>		
#include <userint.h>
#include "DigiScope.h"
#include "Utilities.h"


static int splashScreen,mainPanel,fGenPanel,plotPanel,sigProPanel,fileHandle,plotHandle,processedPlotHandle,peakHandle,valleyHandle,filteredPlotHandle;
int i=0,j,flag,source=0,Count=0,choice=0,status=0,timerStatus1,timerStatus2,length,currentIndex,Index,numPoint=128,fGenStatus,sigProStatus,color,zoomOn,signalExist=0,processedSignalExist=0,drawLoadedSignal=0,fName=0,fType=0,fOrder=5;
int togSineState, togSquareState, togTriangularState, togSawtoothState, togRampState, togArbitraryState,waveTableSize=10;
double freq=1.0, ampl=1.0,offset=0.0,dutycycle=50.0,phase=0.0,gain=1.0,timeBase=1.0,samplingFreq=128.0;
double X[1024000],Y[1024000],sigToProcess[1024000],processedSignal[1024000],LoadedSignal[1024000],arrayToSave[1024000],waveTable[10];
char pathName[500],printerName[100],statusMsg[1000];
double *peakLoc, *peakAmpl, *peakSecDev;
double *valleyLoc, *valleyAmpl, *valleySecDev;
int height,width;
double lowerTh=0,upperTh=0,lowerCut=1.0,upperCut=1.0;

//**************************** Main Function ***************************//

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((splashScreen = LoadPanel (0, "DigiScope.uir", splashScrn)) < 0)
		return -1;
	
	if ((mainPanel = LoadPanel (0, "DigiScope.uir", MainPanel)) < 0)
		return -1;
	if ((fGenPanel = LoadPanel (0, "DigiScope.uir", FGenPanel)) < 0)
		return -1;
	if ((plotPanel = LoadPanel (0, "DigiScope.uir", PlotPanel)) < 0)
		return -1;
	if ((sigProPanel = LoadPanel (0, "DigiScope.uir", SProPanel)) < 0)
		return -1;
	
	
	SetToolTips(); //Set ToolTips for All the Controls
	GetScreenSize (&height, &width);
	
	//MinimizeAllWindows ();
	SetStdioPort (CVI_STDIO_WINDOW);
	SetStdioWindowVisibility(0);
	
	DisplayPanel (splashScreen);
	SetCtrlAttribute (splashScreen, splashScrn_splashScrnTimer, ATTR_ENABLED, 1);
	RunUserInterface ();
	DiscardPanel (splashScreen);
	return 0;
}


//************************ Timer for Progress Bar ***********************//

int CVICALLBACK splashScrnTimer_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:

			SetCtrlAttribute (splashScreen, splashScrn_ProgressBar, ATTR_CTRL_VAL, i++);
			if (i==501) 
			{
			SetCtrlAttribute (splashScreen, splashScrn_splashScrnTimer, ATTR_ENABLED, 0);
			HidePanel(splashScreen);
			SetmainPanelTabOrder();
			SetPanelPos (mainPanel, 30,(width/2)-282);
			DisplayPanel (mainPanel);
			}
			break;
		}
	return 0;
}

//************************ MenuBar Codes ***********************//

void CVICALLBACK FunctionGenerator_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
			ResetSigPro();
			if (fGenStatus==0)
			{
			HidePanel(sigProPanel);
			sigProStatus=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			ResetTextBox (fGenPanel, FGenPanel_StatusPanel, "");
			ResetButtonExcept(0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			
			SetPanelPos (fGenPanel, 200, (width/2)-466);
			HidefGenOptions(1);
			DisplayPanel(fGenPanel);
			SetfGenPanelTabOrder();
			fGenStatus=1;
			}
			else 
			{
			HidePanel (fGenPanel);
			HidePanel(plotPanel);
			fGenStatus=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			signalExist=0;
			}

}

void CVICALLBACK SignalProcessor_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
		ResetSigPro();
		SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
		if (sigProStatus==0)
		{
		if(signalExist==0)
		{
			MessagePopup ("Info", "Use Function Generator to Generate a Signal!");
			
			if (fGenStatus==0)
			{
			ResetTextBox (fGenPanel, FGenPanel_StatusPanel, "");
			ResetButtonExcept(0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			
			SetPanelPos (fGenPanel, 200, (width/2)-466);
			HidefGenOptions(1);
			DisplayPanel(fGenPanel);
			SetfGenPanelTabOrder();
			fGenStatus=1;
			HidePanel (plotPanel);
			}
		}
		else
		{
		HidePanel (fGenPanel);
		fGenStatus=0;
		SetPanelPos (sigProPanel, 200, (width/2)-466);
		DisplayPanel(sigProPanel);
		sigProPanelTabOrder();
		sigProStatus=1;
		SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
		DisplayPanel(plotPanel);
		SetCtrlAttribute (plotPanel, PlotPanel_sigProBut, ATTR_DIMMED, 1);
		}
		
		}
		else
		{
		HidePanel (fGenPanel);
		HidePanel(sigProPanel);
		sigProStatus=0;
		HidePanel(plotPanel);
		}
				
}

void CVICALLBACK DataAcquisitor_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
}

void CVICALLBACK Exit_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
			status = ConfirmPopup ("Exit?", "Are you sure you want to Exit?");
			if(status==1)
			QuitUserInterface(0);

}

void CVICALLBACK Contents_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
status = OpenDocumentInDefaultViewer ("help.chm", VAL_MAXIMIZE);
if(status!=0) 
MessagePopup ("Error!", "Help File Not Found!\nPlease Reinstall the Proggram!");

}

void CVICALLBACK Instructions_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
OpenDocumentInDefaultViewer ("DigiScope_v1.0_Function_Generator_Signal_Processor_and_Data_Acquisition.pdf", VAL_MAXIMIZE);
}


void CVICALLBACK Credits_Code (int menuBar, int menuItem, void *callbackData,
		int panel)
{
MessagePopup ("Credits!", "DigiScope v1.0\n\nDeveloped By PINAKI DEY\nROLL:113300003");
}

//************************ Codes for MainPanel Buttons ***********************//

int CVICALLBACK FGen_Button_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			ResetSigPro();
			if (fGenStatus==0)
			{
			HidePanel(sigProPanel);
			sigProStatus=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			ResetTextBox (fGenPanel, FGenPanel_StatusPanel, "");
			ResetButtonExcept(0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			
			SetPanelPos (fGenPanel, 200, (width/2)-466);
			HidefGenOptions(1);
			DisplayPanel(fGenPanel);
			SetfGenPanelTabOrder();
			fGenStatus=1;
			}
			else 
			{
			HidePanel (fGenPanel);
			HidePanel(plotPanel);
			fGenStatus=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			signalExist=0;
			}
			
			break;
		}
	return 0;
}

int CVICALLBACK SigPro_Button_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
		ResetSigPro();
		SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
		if (sigProStatus==0)
		{
		if(signalExist==0)
		{
			MessagePopup ("Info", "Use Function Generator to Generate a Signal!");
			
			if (fGenStatus==0)
			{
			ResetTextBox (fGenPanel, FGenPanel_StatusPanel, "");
			ResetButtonExcept(0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			
			SetPanelPos (fGenPanel, 200, (width/2)-466);
			HidefGenOptions(1);
			DisplayPanel(fGenPanel);
			SetfGenPanelTabOrder();
			fGenStatus=1;
			HidePanel (plotPanel);
			}
		}
		else
		{
		HidePanel (fGenPanel);
		fGenStatus=0;
		SetPanelPos (sigProPanel, 200, (width/2)-466);
		DisplayPanel(sigProPanel);
		sigProPanelTabOrder();
		sigProStatus=1;
		SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
		DisplayPanel(plotPanel);
		SetCtrlAttribute (plotPanel, PlotPanel_sigProBut, ATTR_DIMMED, 1);
		}
		
		}
		else
		{
		HidePanel (fGenPanel);
		HidePanel(sigProPanel);
		sigProStatus=0;
		HidePanel(plotPanel);
		}
		
		
			break;
		}
	return 0;
}

int CVICALLBACK DAQ_Button_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			break;
		}
	return 0;
}

int CVICALLBACK Options_Button_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			break;
		}
	return 0;
}

int CVICALLBACK Exit_Button_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			status = ConfirmPopup ("Exit?", "Are you sure you want to Exit?");
			if(status==1)
			QuitUserInterface(0);
			break;
		}
	return 0;
}

//************************ Codes for Function Generator Buttons ***********************//

int CVICALLBACK toggleSine_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togSineState==0)
			{
			//here I'll put 'multiple graph' check later
			ResetButtonExcept(1);
			HidefGenOptions(0);
			togSineState=1;
			}
			else
			{
			HidefGenOptions(1);
			togSineState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}
			
			break;
		}
	return 0;
}

int CVICALLBACK toggleSquare_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togSquareState==0)
			{
			ResetButtonExcept(2);
			HidefGenOptions(0);
			togSquareState=1;
			}
			else
			{
			HidefGenOptions(1);
			togSquareState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}

			break;
		}
	return 0;
}

int CVICALLBACK toggleTriangular_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togTriangularState==0)
			{
			ResetButtonExcept(3);
			HidefGenOptions(0);
			togTriangularState=1;
			}
			else
			{
			HidefGenOptions(1);
			togTriangularState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}

			break;
		}
	return 0;
}

int CVICALLBACK toggleSawtooth_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togSawtoothState==0)
			{
			ResetButtonExcept(4);
			HidefGenOptions(0);
			togSawtoothState=1;
			}
			else
			{
			HidefGenOptions(1);
			togSawtoothState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}

			break;
		}
	return 0;
}

int CVICALLBACK toggleRamp_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togRampState==0)
			{
			ResetButtonExcept(5);
			HidefGenOptions(0);
			togRampState=1;
			SetCtrlAttribute (fGenPanel, FGenPanel_dialFreq, ATTR_DIMMED, 1);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialAmpl, ATTR_DIMMED, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialDCOffset, ATTR_DIMMED, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_DIMMED, 1);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialPhase, ATTR_DIMMED, 1);
			
			}
			else
			{
			HidefGenOptions(1);
			togRampState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}

			break;
		}
	return 0;
}

int CVICALLBACK toggleArbitrary_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (togArbitraryState==0)
			{
			ResetButtonExcept(6);
			HidefGenOptions(0);
			togArbitraryState=1;
			}
			else
			{
			HidefGenOptions(1);
			togArbitraryState=0;
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			}

			break;
		}
	return 0;
}

//******************************** Codes for Input Controls ******************************//

int CVICALLBACK dialFreq_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (fGenPanel, FGenPanel_dialFreq, ATTR_CTRL_VAL, &freq);
			break;
		}
	return 0;
}

int CVICALLBACK dialAmpl_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (fGenPanel, FGenPanel_dialAmpl, ATTR_CTRL_VAL, &ampl);
			break;
		}
	return 0;
}

int CVICALLBACK dialDCOffset_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (fGenPanel, FGenPanel_dialDCOffset, ATTR_CTRL_VAL, &offset);
			break;
		}
	return 0;
}

int CVICALLBACK dialDutyCycle_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_CTRL_VAL, &dutycycle);
			break;
		}
	return 0;
}

int CVICALLBACK dialPhase_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (fGenPanel, FGenPanel_dialPhase, ATTR_CTRL_VAL, &phase);
			break;
		}
	return 0;
}


//*********************************** Output Button Code ********************************//

int CVICALLBACK toggleOutput_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (fGenPanel, FGenPanel_toggleOutput, &status);
			DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			if (status==1)
			{
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 1);
			SetPanelPos (plotPanel, 200, ((width/2)+10));
			SetplotPanelTabOrder();
			DisplayPanel(plotPanel);
			SetCtrlAttribute (plotPanel, PlotPanel_sigProBut, ATTR_DIMMED, 0);
			}
			else
			{
			HidePanel(plotPanel);
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			signalExist=0;
			}
			break;
		}
	return 0;
}

//*********************************** Exit Button Code *********************************//

int CVICALLBACK fGenExit_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel (fGenPanel);
			fGenStatus=0;
			HidePanel(plotPanel);
			
			
			SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			signalExist=0;
			ResetValues();
			
			break;
		}
	return 0;
}

//************************************* Timer Code ************************************//

int CVICALLBACK Timer_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:

			DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			
			if(togSineState==1)
			{
			SineWave (numPoint, ampl/2, freq*7.8125E-3, &phase, Y);
			color=VAL_RED;
			}
			
			if(togSquareState==1)
			{
			SquareWave (numPoint, ampl/2, freq*7.8125E-3, &phase, dutycycle, Y);
			color=VAL_BLUE;
			}
			
			if(togTriangularState==1)
			{
			TriangleWave (numPoint, ampl/2, freq*7.8125E-3, &phase, Y);
			color=VAL_GREEN;
			}
			
			if(togSawtoothState==1)
			{
			SawtoothWave (numPoint, ampl/2, freq*7.8125E-3, &phase, Y);
			color=VAL_CYAN;
			}
			
			if(togRampState==1)
			{
			Ramp (numPoint, 0.0, ampl, Y);
			color=VAL_MAGENTA;
			}
			
			if(togArbitraryState==1)
			{
			Uniform(waveTableSize,1,waveTable);
			ArbitraryWave (numPoint, ampl/2, freq*7.8125E-3, &phase, waveTable, waveTableSize, 0, Y);
			color=VAL_YELLOW;
			}
			

			break;
		}
	

	SetAxisRange (plotPanel, PlotPanel_GRAPH, VAL_MANUAL, 0.0, timeBase, VAL_NO_CHANGE, 0.0, 1.0);
	plotHandle = PlotWaveform (plotPanel, PlotPanel_GRAPH, Y, numPoint, VAL_DOUBLE, gain, offset, 0.0, 1/(1.0*(numPoint-1)), VAL_FAT_LINE,
							   VAL_SOLID_SQUARE, VAL_SOLID, 1, color);
	 
	if (togSineState==0 && togSquareState==0 && togTriangularState==0 && togSawtoothState==0 && togRampState==0 && togArbitraryState==0)
	DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);

	
	Fmt(statusMsg,"%s<%f[p3]%s%f[p3]%s%f[p3]%s",freq," Hz\n",ampl," Vpp\n",offset," DC Offset\n");
	RemoveSurroundingWhiteSpace(statusMsg);
	ResetTextBox (fGenPanel, FGenPanel_StatusPanel, statusMsg);
	
	signalExist=1;
	Clear1D(arrayToSave,1024000);
	Copy1D(Y,numPoint,arrayToSave);
	return 0;
}




//***************************************** Codes for PlotPanel Controls *****************************************//

int CVICALLBACK dialGain_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (plotPanel, PlotPanel_dialGain, ATTR_CTRL_VAL, &gain);
			DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			plotHandle = PlotWaveform (plotPanel, PlotPanel_GRAPH, Y, numPoint, VAL_DOUBLE, gain, offset, 0.0, 1/(1.0*(numPoint-1)), VAL_FAT_LINE,
							   VAL_SOLID_SQUARE, VAL_SOLID, 1, color);
	
			break;
		}
	return 0;
}

int CVICALLBACK dialTimeBase_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlAttribute (plotPanel, PlotPanel_dialTimeBase, ATTR_CTRL_VAL, &timeBase);
			SetAxisRange (plotPanel, PlotPanel_GRAPH, VAL_MANUAL, 0.0, timeBase, VAL_NO_CHANGE, 0.0, 1.0);
			break;
		}
	return 0;
}

int CVICALLBACK GraphSave_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			status = FileSelectPopup ("", "*.txt", "*.txt;*.dat;*.xls", "Select Save Location...", VAL_SAVE_BUTTON, 0, 0, 1, 1, pathName);
			
			if (status>0)
			{
				ArrayToFile (pathName, arrayToSave, VAL_DOUBLE, numPoint, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII,
							 VAL_TRUNCATE);
				status = ConfirmPopup ("Open?", "Would you like to open the file you just saved?");
				
				if(status==1) OpenDocumentInDefaultViewer (pathName, VAL_MAXIMIZE);
			}
			break;
		}
	return 0;
}

int CVICALLBACK GraphPrint_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			//status = FileSelectPopup ("", "*.txt", "*.txt;*.dat;*.xls", "Select Save Location...", VAL_SAVE_BUTTON, 0, 0, 1, 1, pathName);
			GetPrintAttribute (ATTR_PRINTER_NAME, printerName);
			//if (status>0) 
			//{
			if(strlen(printerName)>3)
			{
			status = PrintCtrl (plotPanel, PlotPanel_GRAPH, 0, 1, 1);
			}
			else
			//if (status!=0)
			{
				status = ConfirmPopup ("Error", "Printer Not Found or Internal Error!\nWould you like to save the plot as image?");
				if(status==1)
				{
					status = FileSelectPopup ("", "*.bmp", "*.bmp", "Select Save Location...", VAL_SAVE_BUTTON, 0, 0, 1, 1, pathName);
					if(status!=0) SaveCtrlDisplayToFile (plotPanel, PlotPanel_GRAPH, 1, -1, -1, pathName);
				}
			}
			//}
			break;
		}
	return 0;
}

int CVICALLBACK GraphClose_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel(plotPanel);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);
			break;
		}
	return 0;
}

int CVICALLBACK switchZoom_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (plotPanel, PlotPanel_switchZoom, &zoomOn);
			
			SetCtrlAttribute (plotPanel, PlotPanel_GRAPH, ATTR_ENABLE_ZOOM_AND_PAN, zoomOn);
			
			if (zoomOn==1) 
			{
			SuspendTimerCallbacks();
			SetCtrlAttribute (plotPanel, PlotPanel_toggleRunStop, ATTR_CTRL_VAL, 1);
			}
			else 
			{
			ResumeTimerCallbacks();
			SetCtrlAttribute (plotPanel, PlotPanel_toggleRunStop, ATTR_CTRL_VAL, 0);
			}
			
			break;
		}
	return 0;
}

int CVICALLBACK toggleRunStop_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (plotPanel, PlotPanel_toggleRunStop, ATTR_CTRL_VAL, &status);
			
			if(status==1)
			SuspendTimerCallbacks();
			if (status==0)
			ResumeTimerCallbacks();
			
			break;
		}
	return 0;
}


int CVICALLBACK sigProBut_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
		ResetSigPro();
		SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
		if (sigProStatus==0)
		{
		if(signalExist==0)
		{
			MessagePopup ("Info", "Use Function Generator to Generate a Signal!");
			
			if (fGenStatus==0)
			{
			ResetTextBox (fGenPanel, FGenPanel_StatusPanel, "");
			ResetButtonExcept(0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 1);
			
			SetPanelPos (fGenPanel, 200, (width/2)-466);
			HidefGenOptions(1);
			DisplayPanel(fGenPanel);
			SetfGenPanelTabOrder();
			fGenStatus=1;
			HidePanel (plotPanel);
			}
		}
		else
		{
		HidePanel (fGenPanel);
		fGenStatus=0;
		SetPanelPos (sigProPanel, 200, (width/2)-466);
		DisplayPanel(sigProPanel);
		sigProPanelTabOrder();
		sigProStatus=1;
		DisplayPanel(plotPanel);
		SetCtrlAttribute (plotPanel, PlotPanel_sigProBut, ATTR_DIMMED, 1);
		}
		
		}
		else
		{
		HidePanel (fGenPanel);
		HidePanel(sigProPanel);
		sigProStatus=0;
		HidePanel(plotPanel);
		}

			break;
		}
	return 0;
}


//***************************************** Codes for Signal Processor Panel Controls *****************************************//


int CVICALLBACK sigProExit_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel (sigProPanel);
			sigProStatus=0;
			ResetSigPro();
			HidePanel(plotPanel);
			signalExist=0;

			break;
		}
	return 0;
}

int CVICALLBACK selectSignal_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	//SetCtrlAttribute (sigProPanel, SProPanel_Timer_FileLoad, ATTR_ENABLED, 0);
	SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
	switch (event)
		{
		case EVENT_COMMIT:
			fName=0;
			fType=0;
			drawLoadedSignal=0;
			GetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, &currentIndex);
			ResetSigPro();
			switch (currentIndex)
			{
				case 0:
				//SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 0);
				//SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_VISIBLE, 0);
				//SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_Timer_FileLoad, ATTR_ENABLED, 0);
				UsePeakDetector(0);
				UseFilter(0);
				ResetSigPro();
				choice=0;
				drawLoadedSignal=0;
				
				break;
				
				case 1:
				SetCtrlAttribute (sigProPanel, SProPanel_Timer_FileLoad, ATTR_ENABLED, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				Clear1D(sigToProcess,1024000);
				Copy1D(Y,1024000,sigToProcess);
				SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 1);
				//drawLoadedSignal=0;
				length=numPoint;
				source=1;
				break;
				
				case 2:
				SetCtrlAttribute (sigProPanel, SProPanel_Timer_FileLoad, ATTR_ENABLED, 0);
				Clear1D(sigToProcess,1024000);
				if (processedSignalExist==1)
				{
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				Copy1D(processedSignal,1024000,sigToProcess);
				length=numPoint;
				source=2;
				}
				else 
				{
				ResetSigPro();
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 0);
				MessagePopup ("Warning!", "No Signal Has Been Processed Yet!");
				SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
				}
				//drawLoadedSignal=0;
				
				break;
				
				case 3:
				Clear1D(LoadedSignal,1024000);
				status = FileSelectPopup ("", "*.txt", "*.txt;*.dat;*.xls", "Select File Location...", VAL_LOAD_BUTTON, 0, 0, 1, 0, pathName);
				
				if (status==1)
				{
				SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
				//DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				
				FileToArray (pathName, LoadedSignal, VAL_DOUBLE, numPoint, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
				Clear1D(sigToProcess,1024000);
				Copy1D(LoadedSignal,1024000,sigToProcess);
				
				DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				
				SetAxisRange (plotPanel, PlotPanel_GRAPH, VAL_MANUAL, 0.0, timeBase, VAL_NO_CHANGE, 0.0, 1.0);
				plotHandle = PlotWaveform (plotPanel, PlotPanel_GRAPH, sigToProcess, numPoint, VAL_DOUBLE, gain, offset, 0.0, 1/(1.0*(numPoint-1)), VAL_FAT_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
				signalExist=1;
				Copy1D(sigToProcess,numPoint,arrayToSave); 
				DisplayPanel(plotPanel);
				
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				length=numPoint;
				source=3;
				SetCtrlAttribute (sigProPanel, SProPanel_Timer_FileLoad, ATTR_ENABLED, 1);
				}
				else
				SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
				
				break;
			}

			break;
		}
	return 0;
}

int CVICALLBACK selectionRing_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			fName=0;
			fType=0;
			GetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_INDEX, &currentIndex);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			switch (currentIndex)
			{
				case 0:
				ResetSigPro();
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				UsePeakDetector(0);
				UseFilter(0);
				choice=0;
				break;
				
				case 1:
				ResetSigPro();
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_INDEX, 1);
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_VISIBLE, 1);
				//SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_VISIBLE, 0);
				choice=0;
				break;
				
				case 2:
				ResetSigPro();
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_INDEX, 2);
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_VISIBLE, 1);
				//SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_VISIBLE, 0);
				choice=0;
				break;
			}
			break;
		}
	return 0;
}

int CVICALLBACK selectPeak_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			fName=0;
			fType=0;
			choice=0;
			GetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_CTRL_INDEX, &currentIndex);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);
			
			switch (currentIndex)
			{
			
				case 0:
				UsePeakDetector(0);
				UseFilter(0);
				choice=0;
				break;
				
				case 1:
				UsePeakDetector(1);
				UseFilter(0);
				
				SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit1, ATTR_VISIBLE, 0);

				SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit2, ATTR_VISIBLE, 0);
				
				choice=1;
				break;
				
				case 2:
				UsePeakDetector(1);
				UseFilter(0);
				
				SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit1, ATTR_VISIBLE, 0);

				SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit2, ATTR_VISIBLE, 0);
				
				choice=2;
				break;
				
				case 3:
				UsePeakDetector(1);
				UseFilter(0);
				
				SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit1, ATTR_VISIBLE, 0);

				SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_VISIBLE, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_txtUnit2, ATTR_VISIBLE, 0);
				
				choice=3;
				break;
				
				case 4:
				UsePeakDetector(1);
				UseFilter(0);
				
				choice=4;
				break;
			}

			break;
		}
	return 0;
}

int CVICALLBACK selectFilter_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			
			fName=0;
			fType=0;
			choice=0;
			
			GetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_CTRL_INDEX, &currentIndex);
			SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_CTRL_INDEX, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			UseFilter(0);
			UsePeakDetector(0);
		switch(currentIndex)	
		{
			case 0:
			SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_VISIBLE, 0);
			fName=0;
			break;
			
			case 1:
			fName=1;
			break;
			
			case 2:
			fName=2;
			break;
			
			case 3:
			fName=3;
			break;
			
			case 4:
			fName=4;
			break;
		}	
			
			
			break;
		}
	return 0;
}

int CVICALLBACK selectFilterType_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
		
			upperCut=1.0;
			SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_CTRL_VAL, upperCut);
			
			fType=0;
			choice=0;
		
			GetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_CTRL_INDEX, &currentIndex);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			UseFilter(0);
			UsePeakDetector(0);
			SetCtrlAttribute (sigProPanel, SProPanel_ringSampling, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_ringOrder, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_LABEL_TEXT, "Lower Cutoff Freq.");
		switch(currentIndex)	
		{
			case 0:
			
			fType=0;
			UseFilter(0);
			UsePeakDetector(0);
			break;
			
			case 1: //Low Pass
			fType=1;
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_LABEL_TEXT, "Cutoff Freq.");
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, 1);
			
			break;
			
			case 2: //High Pass
			fType=2;
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_LABEL_TEXT, "Cutoff Freq.");
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, 1);
			
			break;
			
			case 3: //Band Pass
			fType=3;
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_VISIBLE, 1); 

			break;
			
			case 4: //Band Stop
			fType=4;
			SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_VISIBLE, 1); 

			break;
		}	
			break;
		}
	return 0;
}


int CVICALLBACK ringLT_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_CTRL_VAL, &lowerTh);
			break;
		}
	return 0;
}

int CVICALLBACK ringUT_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_CTRL_VAL, &upperTh);
			break;
		}
	return 0;
}

int CVICALLBACK ringOrder_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringOrder, ATTR_CTRL_VAL, &fOrder);
			break;
		}
	return 0;
}

int CVICALLBACK ringLowCut_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_CTRL_VAL, &lowerCut);
			break;
		}
	return 0;
}

int CVICALLBACK ringUpCut_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_CTRL_VAL, &upperCut);
			break;
		}
	return 0;
}

int CVICALLBACK ringSampling_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlAttribute (sigProPanel, SProPanel_ringSampling, ATTR_CTRL_VAL, &samplingFreq);
			break;
		}
	return 0;
}

int CVICALLBACK toggleSPOutput_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (sigProPanel, SProPanel_toggleSPOutput, &status);
			
			if (status==1)
			{
			//SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 1);
			SetPanelPos (plotPanel, 200, ((width/2)+10));
			SetplotPanelTabOrder();
			DisplayPanel(plotPanel);
			if (fName>0 && fType>0)
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 1);
			}
			else
			{
			HidePanel(plotPanel);
			//SetCtrlAttribute (fGenPanel, FGenPanel_Timer, ATTR_ENABLED, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			}
			
			break;
		}
	return 0;
}

int CVICALLBACK TimerOutPutControl_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:

			GetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, &timerStatus1);
			GetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, &timerStatus2);

			if (timerStatus1==0 && timerStatus2==0)
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);

			sigProPanelTabOrder();
			GetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_INDEX, &Index);
			
			switch(Index)
			{
			case 0:
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 1);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			break;
			
			case 1:
			if (choice==0)
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 1);
			else 
			{
			SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 0);
			}
			break;
			
			case 2:
			if (fName>0 && fType>0)
			{
			SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
			SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 0);
			flag=0;
				if (samplingFreq<(2*lowerCut) || samplingFreq<(2*upperCut))
				{
				ResetTextBox (sigProPanel, SProPanel_StatusPanel, "Nyquist Error:\nSampling Frequency Must Not Be\nLess Than Twice of the Cutoff Frequency!");
				SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 0);
				flag=1;
				SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_VAL, 0);
				SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 1);  
				}
				else 
				{
				ResetTextBox (sigProPanel, SProPanel_StatusPanel, "");
				if (flag==1) 
				SetCtrlAttribute (sigProPanel, SProPanel_TimerFilter, ATTR_ENABLED, 1);
				}
			}
			else SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_DIMMED, 1);
			
			break;
			
			}
			break;
		}
	return 0;
}

int CVICALLBACK TimerSigPro (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	
	switch (event)
		{
		case EVENT_TIMER_TICK:
			
			switch (choice)
			{
				case 0:
				SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
				//DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, processedPlotHandle, VAL_IMMEDIATE_DRAW);
				
				DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, peakHandle, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, valleyHandle, VAL_IMMEDIATE_DRAW);
				break;
				
				case 1:
				if (source==1)
				Copy1D(Y,1024000,sigToProcess);
				
				PeakDetector (sigToProcess, length, 0.0, 3, 0, 1, 1, &Count, &peakLoc, &peakAmpl, &peakSecDev);
				Clear1D(arrayToSave,1024000);
				for(i=0;i<Count;i++)
				{
				peakHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(peakLoc+i))/(length-1), *(peakAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((peakLoc+i),1,&arrayToSave[i]);
				j=i;
				}
				PeakDetector (sigToProcess, length, 0.0, 3, 1, 1, 1, &Count, &valleyLoc, &valleyAmpl, &valleySecDev);
				for(i=0;i<Count;i++)
				{
				valleyHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(valleyLoc+i))/(length-1), *(valleyAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((valleyLoc+i),1,&arrayToSave[++j]);
				}
				break;
				
				case 2:
			
				if (source==1)
				Copy1D(Y,1024000,sigToProcess);
				
				PeakDetector (sigToProcess, length, 0.0, 3, 0, 1, 1, &Count, &peakLoc, &peakAmpl, &peakSecDev);
				Clear1D(arrayToSave,1024000);
				for(i=0;i<Count;i++)
				{
				peakHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(peakLoc+i))/(length-1), *(peakAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((peakLoc+i),1,&arrayToSave[i]);
				j=i;
				}
				
				break;
				
				case 3:
				if (source==1)
				Copy1D(Y,1024000,sigToProcess);
				j=i;
				
				PeakDetector (sigToProcess, length, 0.0, 3, 1, 1, 1, &Count, &valleyLoc, &valleyAmpl, &valleySecDev);
				for(i=0;i<Count;i++)
				{
				valleyHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(valleyLoc+i))/(length-1), *(valleyAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((valleyLoc+i),1,&arrayToSave[++j]);
				}
				
				break;
				
				case 4:
				if (source==1)
				Copy1D(Y,1024000,sigToProcess);
				
				PeakDetector (sigToProcess, length, upperTh, 3, 0, 1, 1, &Count, &peakLoc, &peakAmpl, &peakSecDev);
				Clear1D(arrayToSave,1024000);
				for(i=0;i<Count;i++)
				{
				peakHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(peakLoc+i))/(length-1), *(peakAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((peakLoc+i),1,&arrayToSave[i]);
				j=i;
				}
				PeakDetector (sigToProcess, length, lowerTh, 3, 1, 1, 1, &Count, &valleyLoc, &valleyAmpl, &valleySecDev);
				for(i=0;i<Count;i++)
				{
				valleyHandle = PlotPoint (plotPanel, PlotPanel_GRAPH, (*(valleyLoc+i))/(length-1), *(valleyAmpl+i), VAL_SOLID_SQUARE, VAL_WHITE);
				Copy1D((valleyLoc+i),1,&arrayToSave[++j]);
				}
				
				break;
				
			}


			break;
		}
	return 0;
}

int CVICALLBACK Timer_FileLoad_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:

				DeleteGraphPlot (plotPanel, PlotPanel_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				
				SetAxisRange (plotPanel, PlotPanel_GRAPH, VAL_MANUAL, 0.0, timeBase, VAL_NO_CHANGE, 0.0, 1.0);
				plotHandle = PlotWaveform (plotPanel, PlotPanel_GRAPH, sigToProcess, numPoint, VAL_DOUBLE, gain, offset, 0.0, 1/(1.0*(numPoint-1)), VAL_FAT_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
				signalExist=1;
				
				Copy1D(sigToProcess,numPoint,arrayToSave); 
				
				SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 1);
				length=numPoint;
				source=3;
			break;
		}
	return 0;
}

int CVICALLBACK TimerFilter_Code (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	switch (event)
		{
		case EVENT_TIMER_TICK:

			switch (fName)
			{
			case 0:
			break;
			
			case 1:
			
			switch(fType)
			{
			case 1:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Bw_LPF (sigToProcess, length, samplingFreq, lowerCut, fOrder, arrayToSave);
			break;
			
			case 2:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Bw_HPF (sigToProcess, length, samplingFreq, lowerCut, fOrder, arrayToSave);
			break;
			
			case 3:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Bw_BPF (sigToProcess, length, samplingFreq, lowerCut, upperCut, fOrder, arrayToSave);
			break;
			
			case 4:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Bw_BSF (sigToProcess, length, samplingFreq, lowerCut,upperCut, fOrder, arrayToSave);
			break;
			}
			
			break;

			//////////////////////////////////////////////////////////////////////////////////////////
			
			case 2:
			
			switch(fType)
			{
			case 1:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Ch_LPF (sigToProcess, length, samplingFreq, lowerCut, 0.1, fOrder, arrayToSave);
			break;
			
			case 2:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Ch_HPF (sigToProcess, length, samplingFreq, lowerCut, 0.1, fOrder, arrayToSave);
			break;
			
			case 3:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Ch_BPF (sigToProcess, length, samplingFreq, lowerCut,upperCut, 0.1, fOrder, arrayToSave);
			break;
			
			case 4:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Ch_BSF (sigToProcess, length, samplingFreq, lowerCut,upperCut, 0.1, fOrder, arrayToSave);
			break;
			}
			
			
			break;
			
			//////////////////////////////////////////////////////////////////////////////////////////
			
			case 3:
			switch(fType)
			{
			case 1:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			InvCh_LPF (sigToProcess, length, samplingFreq, lowerCut, 40, fOrder, arrayToSave);
			break;
			
			case 2:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			InvCh_HPF (sigToProcess, length, samplingFreq, lowerCut, 40, fOrder, arrayToSave);
			break;
			
			case 3:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			InvCh_BPF (sigToProcess, length, samplingFreq, lowerCut,upperCut, 40, fOrder, arrayToSave);
			break;
			
			case 4:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			InvCh_BSF (sigToProcess, length, samplingFreq, lowerCut,upperCut, 40, fOrder, arrayToSave);
			break;
			}
			break;
			
			//////////////////////////////////////////////////////////////////////////////////////////
			
			case 4:
			
			switch(fType)
			{
			case 1:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Elp_LPF (sigToProcess, length, samplingFreq, lowerCut,0.1, 40, fOrder, arrayToSave);
			break;
			
			case 2:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Elp_HPF (sigToProcess, length, samplingFreq, lowerCut,0.1, 40, fOrder, arrayToSave);
			break;
			
			case 3:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Elp_BPF (sigToProcess, length, samplingFreq, lowerCut,upperCut,0.1, 40, fOrder, arrayToSave);
			break;
			
			case 4:
			if (source==1)
			Copy1D(Y,1024000,sigToProcess);
				
			Clear1D(arrayToSave,1024000);
			Elp_BSF (sigToProcess, length, samplingFreq, lowerCut,upperCut,0.1, 40, fOrder, arrayToSave);
			break;
			}
			break;
			
			//////////////////////////////////////////////////////////////////////////////////////////
			
			}
			if (samplingFreq>=(2*lowerCut) && samplingFreq>=(2*upperCut))
			filteredPlotHandle = PlotWaveform (plotPanel, PlotPanel_GRAPH, arrayToSave, length, VAL_DOUBLE, gain, offset, 0.0, 1/(1.0*(length-1)),
											   VAL_FAT_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			
			
			break;
		}
	return 0;
}
