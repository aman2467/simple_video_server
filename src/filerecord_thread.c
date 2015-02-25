/* ==========================================================================
 * @file    : filerecord_thread.c
 *
 * @description : This file contains code to record raw video.
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
#include <unistd.h>
#include <errno.h>
#include <common.h>

extern char g_filename[30];
extern char *g_framebuff[NUM_BUFFER];
extern char *g_osdbuff[NUM_BUFFER];
extern unsigned int g_framesize;
extern int g_writeflag;
extern int g_enable_osdthread;
extern int g_algo_enable;

void *filerecordThread(void)
{
	int i = 0;
	FILE *fp;

	if((fp = fopen(g_filename,"w")) == NULL) {
		perror("FOPEN");
		return NULL;
	}

	while(!KillFilerecordThread) {
		while(!g_writeflag) {
			usleep(10);
		}
		if(g_enable_osdthread == TRUE) {
			apply_algo(g_osdbuff[i],g_algo_enable);
			if(fwrite(g_osdbuff[i],g_framesize, 1, fp) < 0) {
				perror("WRITE");
				continue;
			}
		} else {
			apply_algo(g_framebuff[i],g_algo_enable);
			if(fwrite(g_framebuff[i],g_framesize, 1, fp) < 0) {
				perror("WRITE");
				continue;
			}
		}
		g_writeflag = 0;
		i++;
		if(i > 9) i = 1;
	}
	fclose(fp);
	return 0;
}
