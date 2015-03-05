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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <common.h>

extern char *g_framebuff[NUM_BUFFER];
extern char *g_osdbuff[NUM_BUFFER];
extern unsigned int g_framesize;
extern int g_writeflag;
extern int g_enable_osdthread;
extern int g_algo_enable;
extern int g_videosavetype;
extern int g_record_video;


void get_video_filename(char *name, int val)
{
	DATE_TIME current;

	getcurrenttime(&current);
	if(system("mkdir -p records") < 0) {
		printf("Fail to create 'records' directory\n");
	}
	if(system("mkdir -p records/videos") < 0) {
		printf("Fail to create 'records/videos' directory\n");
	}
	if(val == 0) {
		snprintf(name,50,"records/videos/raw/VID_%d%d%d_%d%d%d.raw", current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		if(system("mkdir -p records/videos/raw") < 0) {
			printf("Fail to create 'records/videos/raw' directory\n");
		}
	} else if(val == 1){
		snprintf(name,50,"records/videos/h264/VID_%d%d%d_%d%d%d.h264", current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		if(system("mkdir -p records/videos/h264") < 0) {
			printf("Fail to create 'records/videos/h264' directory\n");
		}
	}
}

void *filerecordThread(void)
{
	int i = 0;
	FILE *fp;
	char filename[50];


	while(!KillFilerecordThread) {
		while(!g_writeflag) {
			usleep(5);
		}
		if(g_record_video == TRUE) {
			get_video_filename(filename,g_videosavetype);
			if((fp = fopen(filename,"w")) == NULL) {
				perror("FOPEN");
				return NULL;
			}
			while(g_record_video == TRUE) {
				while(!g_writeflag) usleep(1);
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
				i++;
				if(i > 9) i = 1;
				g_writeflag = FALSE;
			}
			fclose(fp);
		}
		g_writeflag = FALSE;
		i++;
		if(i > 9) i = 1;
	}
	return 0;
}
