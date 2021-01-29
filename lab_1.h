/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_COMMANDBUTTON              2       /* control type: command, callback function: Bye */
#define  PANEL_COMMANDBUTTON_2            3       /* control type: command, callback function: input */
#define  PANEL_GRAPH                      4       /* control type: graph, callback function: (none) */
#define  PANEL_NUMERIC_2                  5       /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_3            6       /* control type: command, callback function: subtract_baseline */
#define  PANEL_NUMERIC_3                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_4            8       /* control type: command, callback function: normalize */
#define  PANEL_COMMANDBUTTON_5            9       /* control type: command, callback function: centroid */
#define  PANEL_NUMERIC                    10      /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_6            11      /* control type: command, callback function: savetofile */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Bye(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK centroid(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK input(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK normalize(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK savetofile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK subtract_baseline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif