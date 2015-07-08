/* ================================================================================
 * @file    : display.c
 *
 * @description : This file contains code to display valid received video frame.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General Public License.
 *              You may obtain a copy of the GNU General Public License Version 2 or later
 *              at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <nw_receiver.h>
#ifdef LOCAL_DISPLAY
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#else
#include <fcntl.h>
#endif

extern char *g_displaybuff;

extern unsigned int packetsize;
extern unsigned int g_capture_width;
extern unsigned int g_capture_height;
extern unsigned int buffsize;
extern unsigned int q_size;
extern unsigned int g_skipframe;
extern int g_last_line;

void displayThread(void)
{
#ifdef LOCAL_DISPLAY
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	char resolution[20] = {0};
#else
	FILE *fp = NULL;
#endif
	char *lineseeker = NULL;
	int linecnt = 0;
	int prevline = 0;
	int prevframe = 0;
	int flag = 0;
	VIDEO_DATA *popped = NULL;
	int frame_cnt = 0;

	popped = (VIDEO_DATA *)calloc(1, sizeof(VIDEO_DATA));
	popped->packetbuff = (char *)calloc(1, packetsize - VALID_DATA);
#ifdef LOCAL_DISPLAY
	SDL_Init(SDL_INIT_VIDEO);
	snprintf(resolution, 20, "Capture : %dx%d", g_capture_width,g_capture_height);
	win = SDL_CreateWindow(resolution, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			g_capture_width, 
			g_capture_height, 
			SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_YUY2,
			SDL_TEXTUREACCESS_STREAMING,
			g_capture_width,
			g_capture_height);
#else
	fp = fopen("file.yuv","wb");
#endif
	while(!KillDisplayThread) {
		if((sizeofqueue() > 0) && (g_displaybuff)) {
			poppacket(popped);
		} else {
			continue;
		}

		if(flag == 0) {
			g_skipframe = popped->frame_num;
			prevframe = popped->frame_num;
			pr_dbg("Frame %d is going to skip\n",g_skipframe);
			flag = 1;
		}

		if(g_skipframe == popped->frame_num) {
			continue;
		}
		if(popped->line_num == FIRST_LINE) {
			lineseeker = g_displaybuff;
			memcpy(lineseeker, popped->packetbuff, packetsize - VALID_DATA);
			lineseeker += (packetsize - VALID_DATA);
			linecnt ++;
			prevline = FIRST_LINE;
		} else if(popped->line_num == g_last_line) {
			memcpy(lineseeker, popped->packetbuff, packetsize - VALID_DATA);
			linecnt++;
			if(linecnt == g_last_line) {
#ifdef LOCAL_DISPLAY
				/* display frame*/
				SDL_UpdateTexture(texture, 0,
						g_displaybuff,
						g_capture_width*BPP);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
#else
				/* save frame */
				fwrite(g_displaybuff, buffsize, 1, fp);
				if(frame_cnt++ > 100) exit(0);
#endif
			}
			lineseeker = g_displaybuff;
			linecnt = 0;
			prevline = 0;
		} else if(prevframe == popped->frame_num) {
			if(popped->line_num == prevline + 1) {
				memcpy(lineseeker, popped->packetbuff, packetsize - VALID_DATA);
				lineseeker += packetsize - VALID_DATA;
				linecnt ++;
			}
			prevline = popped->line_num;
		} else {
			memset(g_displaybuff, 0, buffsize);
			lineseeker = g_displaybuff;
			linecnt = 0;
			g_skipframe = popped->frame_num;
			prevline = popped->line_num;
		}
		if(prevframe != popped->frame_num) {
			prevframe = popped->frame_num;
		}
	}
#ifdef LOCAL_DISPLAY
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
#else
	fclose(fp);
#endif
	free(popped->packetbuff);
	free(popped);
	pr_dbg("Display Thread Terminated\n");

}
