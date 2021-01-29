#include <utility.h>
#include <ansi_c.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include "lab_1.h"
static char pathtofile[300];										//global character array for the path to the input data file
static int i,data,plot,panelHandle;									//initialize int variables i, data, plot, and panelHandle
static int elements=0;											//initialize int variable elements to value of 0
static float x;												//initialize float variable x for baseline subtraction
static float norm,val_centroid1,val_centroid2,pos_centroid1,pos_centroid2,interp_centroid = 0;		//initialize some floats to 0
static float array[300],y_arr[300],x_arr[300],y_arr_sub[300],y_arr_sub_norm[300],saved_arr[300];	//initialize some float arrays to size of 300 (sufficiently large for our dataset)
FILE *file; 												//create file pointer to read input data with fopen

int main (int argc, char *argv[])									//main function to handle the user interface and load the panels
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

int CVICALLBACK Bye (int panel, int control, int event,							//function for "Quit" button
					 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:									//when "Quit" button is pressed:
			QuitUserInterface(0);								//it will call QuitUserInterface(0), closing the user interface panel
			break;
	}
	return 0;
}

int CVICALLBACK input (int panel, int control, int event,						//the "input" function loads in the input data file and plots the raw data
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			FileSelectPopup ("C:\\Users\\bmile\\Documents", "*.*", ".txt", "Select a datafile", VAL_LOAD_BUTTON, 0, 1, 1, 0, pathtofile); //creates pop-up window to choose the input text file
			file = fopen(pathtofile, "r"); 		//opening the file to enable reading the data
			elements=0;				//initializing number of data elements to 0
			
			do{					//do loop gets the characters of the input file one by one
			data = getc(file);			
			if(data == '\t')			//the value of "elements" is incremented by 2 for each tab, since there is one tab per line and two elements per line
				elements = elements + 2;
			}
			while(data != EOF);
			fclose(file);				//close the file after reaching the end of the file
			
			FileToArray (pathtofile, array, VAL_FLOAT, elements, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII); //function that converts input data with "elements" number of total elements to a 1D array, with x-data first, then y-data
			
			for(i=0; i<elements; i++) {				//this loop pulls the x-data out of the first half of the array, and the y-data out of the second half of the array
				if(i<(elements/2))
					x_arr[i] = array[i];
				else
					y_arr[i-(elements/2)] = array[i];
			}
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE,
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);				//plots the x_arr and y_arr data on an XY graph in the user interface, with elements/2 being the total number of x/y pairs
			break;
	}
	return 0;
}

int CVICALLBACK subtract_baseline (int panel, int control, int event,						//this function subtracts a baseline y-value from the plot and will replot the baseline-corrected data
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_NUMERIC_2, &x);						//this function gets the value from the numeric control for the baseline
			for(i=0;i<(elements/2);i++){								//this loop creates a new y array with the values subracted by the baseline
				y_arr_sub[i] = y_arr[i] - x;
			}
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, plot, VAL_IMMEDIATE_DRAW);			//this function deletes the previous plot
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr_sub, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE, //this function plots the baseline-adjusted plot
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);
			break;
	}
	return 0;
}

int CVICALLBACK normalize (int panel, int control, int event,							//this function normalizes the data so the total area under the data is equal to 1
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			float deltax[300];
			norm = 0;
			for(i=1;i<((elements/2));i++){								//this loop iterates through the length of the data array
				deltax[i] = x_arr[i] - x_arr[i-1];						//deltax is the "bin width" of the integration
				norm = norm + (y_arr_sub[i]*deltax[i]);						//norm is the running sum of the y-values mult. by the bin width
			}
			norm = norm + (y_arr_sub[(elements/2)-1]*deltax[((elements/2)-1)]);			//the last value of the deltax array for the bin width is assumed to be the same as the bin width for the previous point
			for(i=0;i<(elements/2);i++){								//this loop creates a new array with the normalized y-values
				y_arr_sub_norm[i] = y_arr_sub[i]/norm;
			}
			SetCtrlVal (panelHandle, PANEL_NUMERIC_3, norm);					//this sets the numeric indicator equal to the normalization factor
			DeleteGraphPlot (panelHandle, PANEL_GRAPH, plot, VAL_IMMEDIATE_DRAW);			//this deletes the old plot and replots the normalized plot
			plot = PlotXY (panelHandle, PANEL_GRAPH, x_arr, y_arr_sub_norm, (elements/2), VAL_FLOAT, VAL_FLOAT, VAL_THIN_LINE,
        VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);
			break;
	}
	return 0;
}

int CVICALLBACK centroid (int panel, int control, int event,							//this function calculates the centroid of the data
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			float deltax[300]={0};
			for(i=0;i<(elements/2)-1;i++){								//this if statement finds the x/y pair to the left of the centroid
				if(val_centroid1 < (0.5)){
					deltax[i] = x_arr[i+1] - x_arr[i];
					val_centroid1 += y_arr_sub_norm[i]*deltax[i];
					pos_centroid1 = x_arr[i];
				}
				if(val_centroid2 <= (0.5)){							//this if statement finds the x/y pair to the right of the centroid
					deltax[i] = x_arr[i+1] - x_arr[i];
					val_centroid2 += y_arr_sub_norm[i]*deltax[i];
					pos_centroid2 = x_arr[i];
				}
			}
			val_centroid1 = val_centroid1 - y_arr_sub_norm[(int)pos_centroid1-1];			//this corrects the y-value on the left of the centroid
			pos_centroid1 = pos_centroid1 - 1;							//this corrects the x-value on the left of the centroid
			interp_centroid = (pos_centroid2-pos_centroid1)/(val_centroid2-val_centroid1)*(0.5-val_centroid1)+pos_centroid1;
			//^^^^^ interp_centroid is the linear interpolated x-value of the centroid from the two x/y pairs on either side
			SetCtrlVal (panelHandle, PANEL_NUMERIC, interp_centroid);				//this sets the numeric indicator for the centroid to interp_centroid
			break;
	}
	return 0;
}
int CVICALLBACK savetofile (int panel, int control, int event,							//this function has a button which saves the file
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			for(i=0;i<(elements);i++){								//this loop combines x and y arrays into one array for the ArrayToFile function
				if(i<(elements/2))
					saved_arr[i] = x_arr[i];
				else
					saved_arr[i] = y_arr_sub_norm[i-(elements/2)];
			}
			ArrayToFile ("c:\\Users\\bmile\\Documents\\saved_data.txt", saved_arr, VAL_FLOAT, elements, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, 10, VAL_ASCII,
             VAL_TRUNCATE);
			//^^^^^ ArrayToFile is a function which converts the 1D array to a tab-separated text file with two columns, one for x data and one for y data
			break;
	}
	return 0;
}
