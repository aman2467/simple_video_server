/* ==========================================================================
 * @file    : main.c
 *
 * @description : This file contains main funtion which creates necessary
 *				threads.
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
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <common.h>
#include <osd_thread.h>

int	g_enable_osdthread;
int	g_enable_jpegsavethread;
int	g_enable_filerecordthread;
char g_filename[40]="records/videos/raw/raw.yuv";
char g_video_device[30]="/dev/video0";
int g_capture_width = 752;
int g_capture_height = 416;
int g_writeflag = 0;
int g_osdflag = 0;
unsigned int g_framesize = 0;
unsigned char *g_jpeg_frame;
int g_jpeg_quality = 100;
int g_jpeg_save;
int g_algo_bw = 1;
int g_algo_cartoon = 0;
int g_algo_enable = 1;
int g_take_snapshot;
char *g_framebuff[NUM_BUFFER] = {NULL};
char *g_osdbuff[NUM_BUFFER] = {NULL};
char *ascii_string;
struct osdwindow osdwin[OSD_MAX_WINDOW];
char ascii_data[STRING_WIDTH*TEXT_HEIGHT*TEXT_WIDTH*BPP];
int current_task;

extern void *captureThread(void *);
extern void *filerecordThread(void *);
extern void *osdThread(void *);
extern void *jpegsaveThread(void *);

/****************************************************************************
 * @usage : This function prints the usage and help menu.
 *
 * @arg  : char pointer to the first entered argument from command line
 * @return value     : void
 * *************************************************************************/
void usage(char *exename)
{
	pr_dbg(YELLOW"\n/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/"NONE"\n\n");
	printf(CYAN"\tHelp : "YELLOW"%s -h"CYAN"    (show this help)\n",exename);
	printf(CYAN"\tUsage: "YELLOW"%s "GREEN"<options>"NONE"\n",exename);
	printf(CYAN"\toptions:\n");
	printf(GREEN"\t\tosd              "NONE":"YELLOW" To enable OSD\n");
	printf(GREEN"\t\trawsave          "NONE":"YELLOW" To enable raw video save\n");
	printf(GREEN"\t\tjpeg             "NONE":"YELLOW" To enable JPEG snapshot save\n");
	printf(GREEN"\t\t-o <filename>    "NONE":"YELLOW" To save raw video to given filename\n");
	printf(GREEN"\t\t-d <device>      "NONE":"YELLOW" To capture video from given device\n");
	printf(GREEN"\t\t-w <width>       "NONE":"YELLOW" Supported capture width\n");
	printf(GREEN"\t\t-h <height>      "NONE":"YELLOW" Supported capture height"NONE"\n");
	pr_dbg(YELLOW"\n/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/"NONE"\n\n");
	exit(0);
}

/****************************************************************************
 * @main : This is the main function of the application. It creates separate
 *          threads for receive packets and save video data to file.
 *
 * @arg  : command line arguments
 * @return value     : 0
 * *************************************************************************/
int main(int argc, char **argv)
{
	int threadStatus = 0;
	pthread_t tCaptureThread, tOsdThread, tFilerecordThread, tJpegsaveThread;
	int i;

	g_enable_osdthread = FALSE;
	g_enable_filerecordthread = FALSE;

	if((argc==2 && strcmp(argv[1],"-h")==0)) {
		usage(argv[0]);
	}

	for(i=1;i<argc;i++) {
		if (strcmp(argv[i], "osd") == 0) {
			g_enable_osdthread = TRUE;
		} else if (strcmp(argv[i], "rawsave") == 0) {
			g_enable_filerecordthread = TRUE;
		} else if (strcmp(argv[i], "jpeg") == 0) {
			g_enable_jpegsavethread = TRUE;
		} else if (strcmp(argv[i], "-o") == 0) {
			strcpy(g_filename+19,argv[++i]);
		} else if (strcmp(argv[i], "-d") == 0) {
			strcpy(g_video_device,argv[++i]);
		} else if (strcmp(argv[i], "-w") == 0) {
			g_capture_width = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-h") == 0) {
			g_capture_height = atoi(argv[++i]);
		}
	}
	g_framesize = g_capture_width*g_capture_height*2;
	for(i = 0; i < NUM_BUFFER ;i++) {
		g_framebuff[i] = (char *)calloc(1,g_framesize);
		memset(g_framebuff[i],0,g_framesize);
		if(g_enable_osdthread) {
			g_osdbuff[i] = (char *)calloc(1,g_framesize);
			memset(g_framebuff[i],0,g_framesize);
		}
	}

	KillCaptureThread = 0;
	if(pthread_create(&tCaptureThread, NULL, captureThread, NULL)) {
		printf("Capture Thread create fail\n");
		exit(0);
	}
	pr_dbg("Capture Thread created\n");
	threadStatus |= CAPTURE_THR;

	if(g_enable_osdthread == TRUE) {
		KillOsdThread = 0;
		if(pthread_create(&tOsdThread, NULL, osdThread, NULL)) {
			perror("OSD Thread create fail\n");
			exit(0);
		}
		pr_dbg("OSD Thread created\n");
		threadStatus |= OSD_THR;
	}

	if(g_enable_filerecordthread == TRUE) {
		KillFilerecordThread = 0;
		if(pthread_create(&tFilerecordThread, NULL, filerecordThread, NULL)) {
			perror("File Record Thread create fail\n");
			exit(0);
		}
		pr_dbg("File Record Thread created\n");
		threadStatus |= FILERECORD_THR;
	}

	if(g_enable_jpegsavethread == TRUE) {
		KillJpegsaveThread = 0;
		if(pthread_create(&tJpegsaveThread, NULL, jpegsaveThread, NULL)) {
			perror("JPEG Save Thread create fail\n");
			exit(0);
		}
		pr_dbg("JPEG Save Thread created\n");
		threadStatus |= JPEGSAVE_THR;
	}

#ifdef _DEBUG
	pr_dbg(YELLOW"/************************************************************/"NONE"\n");
	pr_dbg(CYAN"\tCapture Device \t\t:\t"GREEN" %s"NONE"\n", g_video_device);
	pr_dbg(CYAN"\tCapture resolution \t:\t"GREEN" %d x %d"NONE"\n",g_capture_width,
															g_capture_height);
	if(g_enable_osdthread == TRUE) {
		pr_dbg(CYAN"\tOSD \t\t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tOSD \t\t\t:\t"RED" NO"NONE"\n");
	}
	if(g_enable_filerecordthread == TRUE) {
		pr_dbg(CYAN"\tFile recording \t\t:\t"GREEN" YES"NONE"\n");
		pr_dbg(CYAN"\tOutput filename \t:\t"GREEN" %s"NONE"\n",g_filename+19);
	} else {
		pr_dbg(CYAN"\tFile recording \t\t:\t"RED" NO"NONE"\n");
	}
	if(g_enable_jpegsavethread == TRUE) {
		pr_dbg(CYAN"\tJPEG save \t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tJPEG save \t\t:\t"RED" NO"NONE"\n");
	}
	pr_dbg(YELLOW"/************************************************************/"NONE"\n");
#endif

	while(1) {
		


		usleep(10);
	}

	if(g_enable_jpegsavethread == TRUE) {
		if(threadStatus & JPEGSAVE_THR){
			KillJpegsaveThread = 1;
			pthread_join(tJpegsaveThread, NULL);
		}
	}
	if(g_enable_filerecordthread == TRUE) {
		if(threadStatus & FILERECORD_THR){
			KillFilerecordThread = 1;
			pthread_join(tFilerecordThread, NULL);
		}
	}
	if(g_enable_osdthread == TRUE) {
		if (threadStatus & OSD_THR) {
			KillOsdThread = 1;
			pthread_join(tOsdThread, NULL);
		}
	}
	if(threadStatus & CAPTURE_THR){
		KillCaptureThread = 1;
		pthread_join(tCaptureThread, NULL);
	}
	return 0;
}
