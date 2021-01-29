#include <utility.h>
#include <ansi_c.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "lab_1.h"
static char pathtofile[300];
static float array[300],y_arr[300],x_arr[300],y_arr_sub[300],y_arr_sub_norm[300],saved_arr[300];
static int i,data,plot,panelHandle;
static int elements=0;
static float x;
static float norm = 0;
static float interp_centroid = 0;
static float val_centroid1,val_centroid2,pos_centroid1,pos_centroid2 = 0;
FILE *file;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "lab_1.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK Bye (int panel, int control, int event,
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

int CVICALLBACK input (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			FileSelectPopup ("C:\\Users\\bmile\\Documents", "*.*", ".txt", "Select a datafile", VAL_LOAD_BUTTON, 0, 1, 1, 0, pathtofile);
			file = fopen(pathtofile, "r");
			elements=0;
			do{
			data = getc(file);
			if(data == '\t')
				elements = elements + 2;
			}
			while(data != EOF);
			fclose(file);
			
			FileToArray (pathtofile, array, VAL_FLOAT, elements, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			for(i=0; i<elements; i++) {
				if(i<(elements/2))
					x_arr[i] = array[i];
				else
					y_arr[i-(elements/2)] = array[i];
			}
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE,
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);

			break;
	}
	return 0;
}

int CVICALLBACK subtract_baseline (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//needs to take the input to the button and subtract y_arr[] values and update the plot
			GetCtrlVal (panelHandle, PANEL_NUMERIC_2, &x);
			for(i=0;i<(elements/2);i++){
				y_arr_sub[i] = y_arr[i] - x;
			}
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, plot, VAL_IMMEDIATE_DRAW);
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr_sub, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE,
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);
			break;
	}
	return 0;
}

int CVICALLBACK normalize (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			float deltax[300];
			norm = 0;
			for(i=1;i<((elements/2));i++){
				deltax[i] = x_arr[i] - x_arr[i-1];
				//DebugPrintf("%f\n", deltax[i]);
				norm = norm + (y_arr_sub[i]*deltax[i]);
				//DebugPrintf("%f\n", norm);
			}
			norm = norm + (y_arr_sub[(elements/2)-1]*deltax[((elements/2)-1)]);
			for(i=0;i<(elements/2);i++){
				y_arr_sub_norm[i] = y_arr_sub[i]/norm;
			}
			SetCtrlVal (panelHandle, PANEL_NUMERIC_3, norm);
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, plot, VAL_IMMEDIATE_DRAW);
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr_sub_norm, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE,
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);
			break;
	}
	return 0;
}

int CVICALLBACK centroid (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			float deltax[300]={0};
			for(i=0;i<(elements/2)-1;i++){
				if(val_centroid1 < (0.5)){
					deltax[i] = x_arr[i+1] - x_arr[i];
					val_centroid1 += y_arr_sub_norm[i]*deltax[i];
					pos_centroid1 = x_arr[i];
				}
				if(val_centroid2 <= (0.5)){
					deltax[i] = x_arr[i+1] - x_arr[i];
					val_centroid2 += y_arr_sub_norm[i]*deltax[i];
					pos_centroid2 = x_arr[i];
				}
			}
			val_centroid1 = val_centroid1 - y_arr_sub_norm[(int)pos_centroid1-1];
			pos_centroid1 = pos_centroid1 - 1;
			interp_centroid = (pos_centroid2-pos_centroid1)/(val_centroid2-val_centroid1)*(0.5-val_centroid1)+pos_centroid1;
			
			SetCtrlVal (panelHandle, PANEL_NUMERIC, interp_centroid);
			break;
	}
	return 0;
}
int CVICALLBACK savetofile (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			for(i=0;i<(elements);i++){
				if(i<(elements/2))
					saved_arr[i] = x_arr[i];
				else
					saved_arr[i] = y_arr_sub_norm[i-(elements/2)];
			}
			ArrayToFile ("c:\\Users\\bmile\\Documents\\saved_data1.txt", saved_arr, VAL_FLOAT, elements, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, 10, VAL_ASCII,
             VAL_TRUNCATE);
			break;
	}
	return 0;
}