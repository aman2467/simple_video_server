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
extern int g_writeflag;
extern int g_osdflag;

/****************************************************************************
 * @func    : Initializes all OSD windows
 * @arg     : void
 * @return  : void
 ***************************************************************************/
void init_OSDWindows(void)
{
	int window;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	for(window = 0; window < OSD_MAX_WINDOW; window++) {
		switch(window) {
			/* Icon Window */
			case OSD_WINDOW_ZERO:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_ZERO_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_ZERO_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_ZERO_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_ZERO_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].file,OSD_DATABASE"/win_0.yuyv");
				break;
			case OSD_WINDOW_ONE:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_ONE_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_ONE_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_ONE_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_ONE_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].file,OSD_DATABASE"/win_1.yuyv");
				break;
			case OSD_WINDOW_TWO:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_TWO_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_TWO_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_TWO_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_TWO_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].file,OSD_DATABASE"/win_2.yuyv");
				break;
			case OSD_WINDOW_THREE:
				serverConfig->osdwin[window].enable = TRUE;
				serverConfig->osdwin[window].x = OSD_WINDOW_THREE_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_THREE_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_THREE_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_THREE_H;
				serverConfig->osdwin[window].transparency = TRUE;
				strcpy(serverConfig->osdwin[window].file,OSD_DATABASE"/win_3.yuyv");
				break;
			case OSD_WINDOW_FOUR:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_FOUR_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_FOUR_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_FOUR_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_FOUR_H;
				serverConfig->osdwin[window].transparency = TRUE;
				strcpy(serverConfig->osdwin[window].file,OSD_DATABASE"/win_4.yuyv");
				break;
			/* Text Window */
			case OSD_WINDOW_FIVE:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_FIVE_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_FIVE_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_FIVE_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_FIVE_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].osdtext,OSD_WINDOW_FIVE_STR);
				break;
			case OSD_WINDOW_SIX:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_SIX_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_SIX_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_SIX_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_SIX_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].osdtext,OSD_WINDOW_SIX_STR);
				break;
			case OSD_WINDOW_SEVEN:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_SEVEN_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_SEVEN_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_SEVEN_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_SEVEN_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].osdtext,OSD_WINDOW_SEVEN_STR);
				break;
			case OSD_WINDOW_EIGHT:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_EIGHT_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_EIGHT_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_EIGHT_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_EIGHT_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].osdtext,OSD_WINDOW_EIGHT_STR);
				break;
			case OSD_WINDOW_NINE:
				serverConfig->osdwin[window].enable = FALSE;
				serverConfig->osdwin[window].x = OSD_WINDOW_NINE_X;
				serverConfig->osdwin[window].y = OSD_WINDOW_NINE_Y;
				serverConfig->osdwin[window].width = OSD_WINDOW_NINE_W;
				serverConfig->osdwin[window].height = OSD_WINDOW_NINE_H;
				serverConfig->osdwin[window].transparency = FALSE;
				strcpy(serverConfig->osdwin[window].osdtext,OSD_WINDOW_NINE_STR);
				break;
			default:
				break;
		}
	}
}

/****************************************************************************
 * @func    : OSD Thread main funtion
 * @arg1    : void
 * @return  : void
 ***************************************************************************/
void *osdThread(void)
{
	int i = 0, cnt = 0, osd_size, window;
	char *ptr, *ptri, *osd_data_ptr = NULL;
	FILE *fp;
	SERVER_CONFIG *serverConfig = GetServerConfig();

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
		memcpy(g_osdbuff[i],g_framebuff[i],serverConfig->capture.framesize);
		for(window = 0; window < OSD_MAX_WINDOW; window++) {
			update_osd_window(window);
			if(serverConfig->osdwin[window].enable == TRUE) {
				cnt = 0;
				osd_size = serverConfig->osdwin[window].width*serverConfig->osdwin[window].height*2;
				osd_data_ptr = realloc(osd_data_ptr,osd_size);
				memset(osd_data_ptr,0,osd_size);
				if(window < OSD_WINDOW_FIVE) {
					fp = fopen(serverConfig->osdwin[window].file,"rb");
					if(fread(osd_data_ptr,1,osd_size,fp) != osd_size) {
						return NULL;
					}
					fclose(fp);
				} else {
						get_osd_string(serverConfig->osdwin[window].osdtext,osd_data_ptr);
				}
				ptri = osd_data_ptr;
				ptr = g_osdbuff[i];
				ptr += (serverConfig->osdwin[window].y)*serverConfig->capture.width*2 + 
											(serverConfig->osdwin[window].x)*2;
				while(cnt < serverConfig->osdwin[window].height) {
					fill_osd_data(ptr,ptri,serverConfig->osdwin[window].width*2,
											serverConfig->osdwin[window].transparency);
					ptr += serverConfig->capture.width*2;
					ptri += serverConfig->osdwin[window].width*2;
					cnt++;
				}
			}
		}
		apply_algo(g_osdbuff[i],serverConfig->algo_type);
		if(serverConfig->enable_display_thread) {
			memcpy(serverConfig->disp.display_frame,g_osdbuff[i],serverConfig->capture.framesize);
		}
		g_osdflag = 0;
		g_writeflag = 1;
		if(serverConfig->enable_imagesave_thread && serverConfig->image.osd_on) {
			if(serverConfig->image.recordenable) {
				serverConfig->image.recordenable = FALSE;
				serverConfig->jpeg.framebuff = calloc(serverConfig->capture.framesize, 1);
				memcpy(serverConfig->jpeg.framebuff,g_osdbuff[i],serverConfig->capture.framesize);
			}
		}
		i++;
		if(i > 9) i = 0;
	}
	return 0;
}
