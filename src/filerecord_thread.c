/* ==========================================================================
 * @file    : filerecord_thread.c
 *
 * @description : This file contains code to record/save video.
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <common.h>
#include <pwd.h>

extern char *g_framebuff[NUM_BUFFER];
extern char *g_osdbuff[NUM_BUFFER];
extern int g_writeflag;
extern lock_t buf_lock;

/****************************************************************************
 * @usage : This function creates a file name based on timestamp.
 *
 * @arg1  : pointer to file name
 * @arg2  : video type
 * @return     : void
 * *************************************************************************/
void get_video_filename(char *name, int val)
{
	DATE_TIME current;
	char path[50] = {0};
	char cmd[100] = {0};
	struct passwd *pwd;

	getcurrenttime(&current);
	pwd = getpwuid(getuid());

	if(pwd) {
		strcpy(path,pwd->pw_dir);	
	}
	snprintf(cmd,100,"mkdir -p %s/Videos/svs",path);
	if(system(cmd) < 0) {
		printf("Fail to create 'svs' directory\n");
	}
	if(val == 0) {
		snprintf(name,100,"%s/Videos/svs/raw/VID_%d%d%d_%d%d%d.raw", path, current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		strcat(cmd,"/raw");
		if(system(cmd) < 0) {
			printf("Fail to create 'svs/raw' directory\n");
		}
	} else if(val == 1){
		snprintf(name,100,"%s/Videos/svs/h264/VID_%d%d%d_%d%d%d.h264", path, current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		strcat(cmd,"/h264");
		if(system(cmd) < 0) {
			printf("Fail to create 'svs/h264' directory\n");
		}
	}
}

/****************************************************************************
 * @usage : This is the thread function for recording/saving a video file.
 *
 * @arg  : void
 * @return     : void
 * *************************************************************************/
void *filerecordThread(void)
{
	int i = 0;
	FILE *fp;
	char filename[100];

	SERVER_CONFIG *serverConfig = GetServerConfig();

	while(!KillFilerecordThread) {
		while(!g_writeflag) {
			usleep(50);
		}
		if(serverConfig->video.recordenable == TRUE) {
			get_video_filename(filename,serverConfig->video.type);
			if((fp = fopen(filename,"w")) == NULL) {
				perror("FOPEN");
				return NULL;
			}
			while(serverConfig->video.recordenable == TRUE) {
				while(!g_writeflag) usleep(1);
				if(serverConfig->enable_osd_thread == TRUE) {
					if(fwrite(g_osdbuff[i],serverConfig->capture.framesize, 1, fp) < 0) {
						perror("WRITE");
						continue;
					}
				} else {
					lock(&buf_lock);
					apply_algo(g_framebuff[i],serverConfig->algo_type);
					if(fwrite(g_framebuff[i],serverConfig->capture.framesize, 1, fp) < 0) {
						perror("WRITE");
						continue;
					}
					unlock(&buf_lock);
				}
				i++;
				if(i > NUM_BUFFER-1) i = 0;
				g_writeflag = FALSE;
			}
			fclose(fp);
		}
		g_writeflag = FALSE;
		i++;
		if(i > NUM_BUFFER-1) i = 0;
	}
	return 0;
}
