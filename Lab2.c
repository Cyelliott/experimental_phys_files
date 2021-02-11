#include <utility.h>
#include <formatio.h>
#include <NIDAQmx.h>
#include <advanlys.h>
#include <cvirte.h>
#include <userint.h>
#include "Lab_2.h"

static char time_path[300],freq_path[300];
static int i,panelHandle,plot_v_t,plot_f_vf;
static int32 actual_num_samples;
static TaskHandle task_handle;
static double time, samplerate, num_samples;
static double time_arr[10000], freq_arr[10000], t_arr[10000], v_arr[10000], f_arr[10000], vf_arr[10000], spec_arr[10000];


int main (int argc, char *argv[])
{
if (InitCVIRTE (0, argv, 0) == 0)
return -1; /* out of memory */
if ((panelHandle = LoadPanel (0, "Lab_2.uir", PANEL)) < 0)
return -1;
DisplayPanel (panelHandle);
RunUserInterface ();
DiscardPanel (panelHandle);
return 0;
}

int CVICALLBACK acquire_data (int panel, int control, int event,
 void *callbackData, int eventData1, int eventData2)
{
switch (event)
{
case EVENT_COMMIT:
GetCtrlVal(panelHandle, PANEL_NUMERIC, &time); //get total number of seconds to acquire data
GetCtrlVal(panelHandle, PANEL_NUMERIC_2, &samplerate); //get number of samples per second to acquire data (Hz)
num_samples = (time)*samplerate; //get total number of samples of data
DebugPrintf("num_samples is %lf", num_samples);
DAQmxCreateTask ("Data Acquisition Task", &task_handle);
//create task
DAQmxCreateAIVoltageChan (task_handle, "Dev1/ai0", "ch1", DAQmx_Val_Diff, -5.0, 5.0, DAQmx_Val_Volts, "");
//create voltage channel
DAQmxCfgSampClkTiming (task_handle, "OnboardClock", samplerate, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, num_samples);
//configure sample rate and total number of samples
DAQmxStartTask (task_handle);
//start task
DAQmxReadAnalogF64 (task_handle, DAQmx_Val_Auto, time*10, DAQmx_Val_GroupByChannel, v_arr, num_samples, &actual_num_samples, 0);
//read data into array of size "num_samples"
for(i=0;i<num_samples;i++){//get corresponding time elapsed (in seconds) for each data point and recasts voltage array to "spectrum" array
t_arr[i] = i/samplerate;
spec_arr[i] = v_arr[i];
}
DAQmxStopTask (task_handle); //stop task
DAQmxClearTask (task_handle); //clear task
Spectrum (spec_arr, num_samples); //calculates the power spectrum of the voltage vs. time data
for(i=0;i<(num_samples);i++){ //convert spectrum data to frequency and v(t) data ---->>>>>> need to figure out array format first
f_arr[i] = 1/(time)*i;  
vf_arr[i] = spec_arr[i];
}
if(plot_v_t > 0)
DeleteGraphPlot(panelHandle, PANEL_GRAPH, plot_v_t, VAL_IMMEDIATE_DRAW);
if(plot_f_vf > 0)
DeleteGraphPlot(panelHandle, PANEL_GRAPH_2, plot_f_vf, VAL_IMMEDIATE_DRAW);
plot_v_t = PlotXY (panelHandle, PANEL_GRAPH, t_arr, v_arr, num_samples, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
plot_f_vf = PlotXY (panelHandle, PANEL_GRAPH_2, f_arr, vf_arr, num_samples, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
break;
}
return 0;
}

int CVICALLBACK save_to_file (int panel, int control, int event,
 void *callbackData, int eventData1, int eventData2)
{
switch (event)
{
case EVENT_COMMIT:
for(i=0;i<(num_samples*2);i++){ //re-format array to condense time-domain data
if(i<(num_samples))
time_arr[i] = t_arr[i];
else
time_arr[i] = v_arr[i-(int)num_samples];
}
for(i=0;i<(num_samples*2);i++){ //re-format array to condense frequency-domain data
if(i<(num_samples))
freq_arr[i] = f_arr[i];
else
freq_arr[i] = vf_arr[i-(int)num_samples];
}
FileSelectPopupEx ("C:\\Users\\elliott.cy\\Documents\\lab 2 Cy and Brad", "*.*", ".txt", "Save time domain", VAL_SAVE_BUTTON, 0, 0, time_path);
ArrayToFile (time_path, time_arr, VAL_DOUBLE, num_samples*2, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, 10, VAL_ASCII,
             VAL_TRUNCATE);
FileSelectPopupEx ("C:\\Users\\elliott.cy\\Documents\\lab 2 Cy and Brad", "*.*", ".txt", "Save frequency domain", VAL_SAVE_BUTTON, 0, 0, freq_path);
ArrayToFile (freq_path, freq_arr, VAL_DOUBLE, num_samples*2, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, 10, VAL_ASCII,
             VAL_TRUNCATE);
break;
}
return 0;
}

int CVICALLBACK bye (int panel, int control, int event,
void *callbackData, int eventData1, int eventData2)
{
switch (event)
{
case EVENT_COMMIT:
QuitUserInterface(0);
break;
}
return 0;
}