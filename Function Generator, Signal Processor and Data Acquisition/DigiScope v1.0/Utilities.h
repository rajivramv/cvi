
//************************ Utilities.h Header File **********************//

static int fGenPanel,plotPanel,mainPanel,sigProPanel;
int choice, state,numPoint, togSineState, togSquareState, togTriangularState, togSawtoothState, togRampState, togArbitraryState,zoomOn;
double freq, ampl,offset,dutycycle,phase,gain,timeBase;


void SetToolTips(void)
{
SetCtrlToolTipAttribute (mainPanel, MainPanel_FGen_Button, CTRL_TOOLTIP_ATTR_TEXT, "Function Generator");
SetCtrlToolTipAttribute (mainPanel, MainPanel_SigPro_Button, CTRL_TOOLTIP_ATTR_TEXT, "Signal Processor");
SetCtrlToolTipAttribute (mainPanel, MainPanel_DAQ_Button, CTRL_TOOLTIP_ATTR_TEXT, "Data Acquisitor");
SetCtrlToolTipAttribute (mainPanel, MainPanel_Options_Button, CTRL_TOOLTIP_ATTR_TEXT, "Options");
SetCtrlToolTipAttribute (mainPanel, MainPanel_Exit_Button, CTRL_TOOLTIP_ATTR_TEXT, "Exit to System");

SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleSine, CTRL_TOOLTIP_ATTR_TEXT, "Sine Wave");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleSquare, CTRL_TOOLTIP_ATTR_TEXT, "Square Wave");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleTriangular, CTRL_TOOLTIP_ATTR_TEXT, "Triangular Wave");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleSawtooth, CTRL_TOOLTIP_ATTR_TEXT, "Sawtooth Wave");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleRamp, CTRL_TOOLTIP_ATTR_TEXT, "Ramp");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleArbitrary, CTRL_TOOLTIP_ATTR_TEXT, "Arbitrary Wave");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_toggleOutput, CTRL_TOOLTIP_ATTR_TEXT, "Generate Output");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_fGenExit, CTRL_TOOLTIP_ATTR_TEXT, "Close Function Generator");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_dialFreq, CTRL_TOOLTIP_ATTR_TEXT, "Set Frequency");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_dialAmpl, CTRL_TOOLTIP_ATTR_TEXT, "Set Amplitude");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_dialDCOffset, CTRL_TOOLTIP_ATTR_TEXT, "Set DC Offset");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_dialDutyCycle, CTRL_TOOLTIP_ATTR_TEXT, "Set Duty Cycle");
SetCtrlToolTipAttribute (fGenPanel, FGenPanel_dialPhase, CTRL_TOOLTIP_ATTR_TEXT, "Set Phase");

SetCtrlToolTipAttribute (plotPanel, PlotPanel_dialGain, CTRL_TOOLTIP_ATTR_TEXT, "Set Gain");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_dialTimeBase, CTRL_TOOLTIP_ATTR_TEXT, "Set TimeBase Expansion");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_switchZoom, CTRL_TOOLTIP_ATTR_TEXT, "Set Zoom/Pan");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_toggleRunStop, CTRL_TOOLTIP_ATTR_TEXT, "Run/Stop");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_GraphSave, CTRL_TOOLTIP_ATTR_TEXT, "Save Plot Data");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_GraphPrint, CTRL_TOOLTIP_ATTR_TEXT, "Print Plot");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_GraphClose, CTRL_TOOLTIP_ATTR_TEXT, "Close Plot");
SetCtrlToolTipAttribute (plotPanel, PlotPanel_sigProBut, CTRL_TOOLTIP_ATTR_TEXT, "Run Signal Processor");

SetCtrlToolTipAttribute (sigProPanel, SProPanel_toggleSPOutput, CTRL_TOOLTIP_ATTR_TEXT, "Generate Output");
SetCtrlToolTipAttribute (sigProPanel, SProPanel_sigProExit, CTRL_TOOLTIP_ATTR_TEXT, "Close Signal Processor");




}


// #################### User Defined Functions Related to 'Function Generator' ####################//




//********************* To Hide or Unhide Input Controls ****************//

void HidefGenOptions(int state)
{

SetCtrlAttribute (fGenPanel, FGenPanel_dialFreq, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_dialAmpl, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_dialDCOffset, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_dialPhase, ATTR_DIMMED, state);

SetCtrlAttribute (fGenPanel, FGenPanel_txtFreq, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_txtAmpl, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_txtDCOffset, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_txtDutyCycle, ATTR_DIMMED, state);
SetCtrlAttribute (fGenPanel, FGenPanel_txtPhase, ATTR_DIMMED, state);


GetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, &togSquareState);

if (togSquareState==1)
{
SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_DIMMED, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_txtDutyCycle, ATTR_DIMMED, 0);
}
else
{
SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_DIMMED, 1);
SetCtrlAttribute (fGenPanel, FGenPanel_txtDutyCycle, ATTR_DIMMED, 1);
}
}



void ResetValues(void)

{
			SetCtrlAttribute (fGenPanel, FGenPanel_dialFreq, ATTR_CTRL_VAL, 1.0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialAmpl, ATTR_CTRL_VAL, 1.0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialDCOffset, ATTR_CTRL_VAL, 0.0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_CTRL_VAL, 0.0);
			SetCtrlAttribute (fGenPanel, FGenPanel_dialPhase, ATTR_CTRL_VAL, 0.0);
			
			SetCtrlAttribute (plotPanel, PlotPanel_dialGain, ATTR_CTRL_VAL, 1.0);
			SetCtrlAttribute (plotPanel, PlotPanel_dialTimeBase, ATTR_CTRL_VAL, 1.0);
			
			freq=1.0, ampl=1.0,offset=0.0,dutycycle=50.0,phase=0.0,gain=1.0,timeBase=1.0,numPoint=128;
			

}


//************ To Reset Other Buttons Except the Pressed One ************//


void ResetButtonExcept(int state)
{
switch (state)
{
case 0:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


case 1:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;

break;

case 2:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);

SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


break;

case 3:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


break;

case 4:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);

SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


break;

case 5:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);

SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


break;

case 6:

SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_VAL, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_VAL, 0);

togSineState=0;
togSquareState=0;
togTriangularState=0;
togSawtoothState=0;
togRampState=0;
togArbitraryState=0;


break;

}

SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_DIMMED, 0);
SetCtrlAttribute (plotPanel, PlotPanel_switchZoom, ATTR_CTRL_VAL, 0);
zoomOn=0;
ResumeTimerCallbacks();
}

//************************************ Setting Tab Order *******************************//
void SetmainPanelTabOrder(void)
{
SetCtrlAttribute (mainPanel, MainPanel_FGen_Button, ATTR_CTRL_TAB_POSITION, 0);
SetCtrlAttribute (mainPanel, MainPanel_SigPro_Button, ATTR_CTRL_TAB_POSITION, 1);
SetCtrlAttribute (mainPanel, MainPanel_DAQ_Button, ATTR_CTRL_TAB_POSITION, 2);
SetCtrlAttribute (mainPanel, MainPanel_Options_Button, ATTR_CTRL_TAB_POSITION, 3);
SetCtrlAttribute (mainPanel, MainPanel_Exit_Button, ATTR_CTRL_TAB_POSITION, 4);
}


void SetfGenPanelTabOrder(void)
{
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSine, ATTR_CTRL_TAB_POSITION, 0);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSquare, ATTR_CTRL_TAB_POSITION, 1);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleTriangular, ATTR_CTRL_TAB_POSITION, 2);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleSawtooth, ATTR_CTRL_TAB_POSITION, 3);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleRamp, ATTR_CTRL_TAB_POSITION, 4);
SetCtrlAttribute (fGenPanel, FGenPanel_toggleArbitrary, ATTR_CTRL_TAB_POSITION, 5);

SetCtrlAttribute (fGenPanel, FGenPanel_dialFreq, ATTR_CTRL_TAB_POSITION, 6);
SetCtrlAttribute (fGenPanel, FGenPanel_dialAmpl, ATTR_CTRL_TAB_POSITION, 7);
SetCtrlAttribute (fGenPanel, FGenPanel_dialDCOffset, ATTR_CTRL_TAB_POSITION, 8);
SetCtrlAttribute (fGenPanel, FGenPanel_dialDutyCycle, ATTR_CTRL_TAB_POSITION, 9);
SetCtrlAttribute (fGenPanel, FGenPanel_dialPhase, ATTR_CTRL_TAB_POSITION, 10);
																			  
SetCtrlAttribute (fGenPanel, FGenPanel_toggleOutput, ATTR_CTRL_TAB_POSITION, 11);
SetCtrlAttribute (fGenPanel, FGenPanel_fGenExit, ATTR_CTRL_TAB_POSITION, 12);
}

void SetplotPanelTabOrder(void)
{
SetCtrlAttribute (plotPanel, PlotPanel_dialGain, ATTR_CTRL_TAB_POSITION, 0);
SetCtrlAttribute (plotPanel, PlotPanel_dialTimeBase, ATTR_CTRL_TAB_POSITION, 1);
SetCtrlAttribute (plotPanel, PlotPanel_GRAPH, ATTR_CTRL_TAB_POSITION, 2);
SetCtrlAttribute (plotPanel, PlotPanel_switchZoom, ATTR_CTRL_TAB_POSITION, 3);

SetCtrlAttribute (plotPanel, PlotPanel_GraphSave, ATTR_CTRL_TAB_POSITION, 4);
SetCtrlAttribute (plotPanel, PlotPanel_GraphPrint, ATTR_CTRL_TAB_POSITION, 5);
SetCtrlAttribute (plotPanel, PlotPanel_GraphClose, ATTR_CTRL_TAB_POSITION, 6);

}



// #################### User Defined Functions Related to 'Signal Processor' ####################//

void sigProPanelTabOrder(void)
{
SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_TAB_POSITION, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_TAB_POSITION, 1);

SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_CTRL_TAB_POSITION, 2);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_CTRL_TAB_POSITION, 3);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_CTRL_TAB_POSITION, 4);

SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_CTRL_TAB_POSITION, 5);
SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_CTRL_TAB_POSITION, 6);

SetCtrlAttribute (sigProPanel, SProPanel_ringSampling, ATTR_CTRL_TAB_POSITION, 7);
SetCtrlAttribute (sigProPanel, SProPanel_ringOrder, ATTR_CTRL_TAB_POSITION, 8);
SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_CTRL_TAB_POSITION, 9);
SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_CTRL_TAB_POSITION, 10);



SetCtrlAttribute (sigProPanel, SProPanel_toggleSPOutput, ATTR_CTRL_TAB_POSITION, 11);
SetCtrlAttribute (sigProPanel, SProPanel_sigProExit, ATTR_CTRL_TAB_POSITION, 12);

}


void UsePeakDetector(int state)
{

SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_txtUnit1, ATTR_VISIBLE, state);

SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_txtUnit2, ATTR_VISIBLE, state);

//SetCtrlAttribute (sigProPanel, SProPanel_ringOrder, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_ringSampling, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_txtHz1, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_txtHz2, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_txtHz3, ATTR_VISIBLE, !state);
}

void UseFilter(int state)
{

SetCtrlAttribute (sigProPanel, SProPanel_ringSampling, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_ringOrder, ATTR_VISIBLE, state);

SetCtrlAttribute (sigProPanel, SProPanel_ringLowCut, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_ringUpCut, ATTR_VISIBLE, state); 

SetCtrlAttribute (sigProPanel, SProPanel_txtHz1, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_txtHz2, ATTR_VISIBLE, state);
SetCtrlAttribute (sigProPanel, SProPanel_txtHz3, ATTR_VISIBLE, state);

//SetCtrlAttribute (sigProPanel, SProPanel_ringLT, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_ringUT, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_txtUnit1, ATTR_VISIBLE, !state);
//SetCtrlAttribute (sigProPanel, SProPanel_txtUnit2, ATTR_VISIBLE, !state);
}


void ResetSigPro(void)
{
SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_VISIBLE, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_VISIBLE, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_VISIBLE, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_VISIBLE, 0);

UsePeakDetector(0);
UseFilter(0);
SetCtrlAttribute (sigProPanel, SProPanel_TimerSigPro, ATTR_ENABLED, 0);
choice=0;

//SetCtrlAttribute (sigProPanel, SProPanel_selectSignal, ATTR_CTRL_INDEX, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectionRing, ATTR_CTRL_INDEX, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectPeak, ATTR_CTRL_INDEX, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilter, ATTR_CTRL_INDEX, 0);
SetCtrlAttribute (sigProPanel, SProPanel_selectFilterType, ATTR_CTRL_INDEX, 0);
}
