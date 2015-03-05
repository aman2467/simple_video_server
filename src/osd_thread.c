/* ==========================================================================
 * @file    : osd_thread.c
 *
 * @description : This file contains the osd thread.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common.h>
#include <osd_thread.h>

extern char *g_framebuff[NUM_BUFFER];
extern char *g_osdbuff[NUM_BUFFER];
extern int g_capture_width;
extern int g_capture_height;
extern unsigned int g_framesize;
extern int g_writeflag;
extern int g_osdflag;
extern struct osdwindow osdwin[OSD_MAX_WINDOW];

void init_OSDWindows()
{
	int window;

	for(window = 0; window < OSD_MAX_WINDOW; window++) {
		switch(window) {
			/* Icon Window */
			case OSD_WINDOW_ZERO:
				osdwin[window].enable = TRUE;
				osdwin[window].x = OSD_WINDOW_ZERO_X;
				osdwin[window].y = OSD_WINDOW_ZERO_Y;
				osdwin[window].width = OSD_WINDOW_ZERO_W;
				osdwin[window].height = OSD_WINDOW_ZERO_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].file,"database/win_0.yuyv");
				break;
			case OSD_WINDOW_ONE:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_ONE_X;
				osdwin[window].y = OSD_WINDOW_ONE_Y;
				osdwin[window].width = OSD_WINDOW_ONE_W;
				osdwin[window].height = OSD_WINDOW_ONE_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].file,"database/win_1.yuyv");
				break;
			case OSD_WINDOW_TWO:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_TWO_X;
				osdwin[window].y = OSD_WINDOW_TWO_Y;
				osdwin[window].width = OSD_WINDOW_TWO_W;
				osdwin[window].height = OSD_WINDOW_TWO_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].file,"database/win_2.yuyv");
				break;
			case OSD_WINDOW_THREE:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_THREE_X;
				osdwin[window].y = OSD_WINDOW_THREE_Y;
				osdwin[window].width = OSD_WINDOW_THREE_W;
				osdwin[window].height = OSD_WINDOW_THREE_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].file,"database/win_3.yuyv");
				break;
			case OSD_WINDOW_FOUR:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_FOUR_X;
				osdwin[window].y = OSD_WINDOW_FOUR_Y;
				osdwin[window].width = OSD_WINDOW_FOUR_W;
				osdwin[window].height = OSD_WINDOW_FOUR_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].file,"database/win_4.yuyv");
				break;
			/* Text Window */
			case OSD_WINDOW_FIVE:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_FIVE_X;
				osdwin[window].y = OSD_WINDOW_FIVE_Y;
				osdwin[window].width = OSD_WINDOW_FIVE_W;
				osdwin[window].height = OSD_WINDOW_FIVE_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].osdtext,OSD_WINDOW_FIVE_STR);
				break;
			case OSD_WINDOW_SIX:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_SIX_X;
				osdwin[window].y = OSD_WINDOW_SIX_Y;
				osdwin[window].width = OSD_WINDOW_SIX_W;
				osdwin[window].height = OSD_WINDOW_SIX_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].osdtext,OSD_WINDOW_SIX_STR);
				break;
			case OSD_WINDOW_SEVEN:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_SEVEN_X;
				osdwin[window].y = OSD_WINDOW_SEVEN_Y;
				osdwin[window].width = OSD_WINDOW_SEVEN_W;
				osdwin[window].height = OSD_WINDOW_SEVEN_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].osdtext,OSD_WINDOW_SEVEN_STR);
				break;
			case OSD_WINDOW_EIGHT:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_EIGHT_X;
				osdwin[window].y = OSD_WINDOW_EIGHT_Y;
				osdwin[window].width = OSD_WINDOW_EIGHT_W;
				osdwin[window].height = OSD_WINDOW_EIGHT_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].osdtext,OSD_WINDOW_EIGHT_STR);
				break;
			case OSD_WINDOW_NINE:
				osdwin[window].enable = FALSE;
				osdwin[window].x = OSD_WINDOW_NINE_X;
				osdwin[window].y = OSD_WINDOW_NINE_Y;
				osdwin[window].width = OSD_WINDOW_NINE_W;
				osdwin[window].height = OSD_WINDOW_NINE_H;
				osdwin[window].transparency = FALSE;
				strcpy(osdwin[window].osdtext,OSD_WINDOW_NINE_STR);
				break;
			default:
				break;
		}
	}
}

/****************************************************************************
 * @func    : v4l2CaptureThread
 *          Main thread function
 * @arg1    : void
 * @return  : void
 ***************************************************************************/
void *osdThread(void)
{
	int i = 0, cnt = 0, osd_size, window;
	char *ptr, *ptri, *osd_data_ptr = NULL;
	FILE *fp;

	if(osd_init(osd_data_ptr) == FAIL) {
		printf("OSD init Failed\n");
		return 0;
	}
	init_OSDWindows();
	
	if((osd_data_ptr = calloc(400*400*2,1)) == NULL) {
		printf("Memory not allocated to OSD\n");
		return 0;
	}
	while(!KillOsdThread) {
		while(!g_osdflag) {
			usleep(5);
		}
		memcpy(g_osdbuff[i],g_framebuff[i],g_framesize);
		for(window = 0; window < OSD_MAX_WINDOW; window++) {
			update_osd_window(window);
			if(osdwin[window].enable == TRUE) {
				cnt = 0;
				osd_size = osdwin[window].width*osdwin[window].height*2;
				osd_data_ptr = realloc(osd_data_ptr,osd_size);
				memset(osd_data_ptr,0,osd_size);
				if(window < OSD_WINDOW_FIVE) {
					fp = fopen(osdwin[window].file,"rb");
					if(fread(osd_data_ptr,1,osd_size,fp) != osd_size) {
						return NULL;
					}
					fclose(fp);
				} else {
					get_osd_string(osdwin[window].osdtext,osd_data_ptr);
				}
				ptri = osd_data_ptr;
				ptr = g_osdbuff[i];
				ptr += (osdwin[window].y)*g_capture_width*2 + 
											(osdwin[window].x)*2;
				while(cnt < osdwin[window].height) {
					fill_osd_data(ptr,ptri,osdwin[window].width*2,
											osdwin[window].transparency);
					ptr += g_capture_width*2;
					ptri += osdwin[window].width*2;
					cnt++;
				}
			}
		}
		g_osdflag = 0;
		g_writeflag = 1;
		i++;
		if(i > 9) i = 0;
	}
	return 0;
}
