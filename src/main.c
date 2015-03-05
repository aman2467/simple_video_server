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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <common.h>
#include <command_list.h>
#include <osd_thread.h>

int	g_enable_osdthread;
int	g_enable_jpegsavethread;
int	g_enable_filerecordthread;

int g_videosavetype = 0;
int g_imagesavetype = 0;
char g_video_device[30]="/dev/video0";
int g_capture_width = 848;
int g_capture_height = 480;

int g_writeflag = 0;
int g_osdflag = 0;

unsigned int g_framesize = 0;
unsigned char *g_jpeg_frame;
int g_jpeg_quality = 100;
int g_image_save;

int g_algo_bw = 0;
int g_algo_cartoon = 0;
int g_algo_enable = 0;

int g_take_snapshot;
int g_record_video;

char *g_framebuff[NUM_BUFFER] = {NULL};
char *g_osdbuff[NUM_BUFFER] = {NULL};
char *ascii_string;
struct osdwindow osdwin[OSD_MAX_WINDOW];
char ascii_data[STRING_WIDTH*TEXT_HEIGHT*TEXT_WIDTH*BPP];
int current_task;
SERVER_CONFIG g_server_config;

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
	printf(GREEN"\t\tvideosave        "NONE":"YELLOW" To enable video save\n");
	printf(GREEN"\t\timagesave        "NONE":"YELLOW" To enable snapshot save\n");
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
	int command;
	int value;
	int ret = 0;
	char commandstr[4];
	char status[5]={0};
	char userdata[30];
	char data[26];
	int sock_fd;
	socklen_t slen,clen;
	struct sockaddr_in ser_addr,cli_addr;

	g_enable_osdthread = FALSE;
	g_enable_filerecordthread = FALSE;

	if((argc==2 && strcmp(argv[1],"-h")==0)) {
		usage(argv[0]);
	}

	for(i=1;i<argc;i++) {
		if (strcmp(argv[i], "osd") == 0) {
			g_enable_osdthread = TRUE;
		} else if (strcmp(argv[i], "videosave") == 0) {
			g_enable_filerecordthread = TRUE;
		} else if (strcmp(argv[i], "imagesave") == 0) {
			g_enable_jpegsavethread = TRUE;
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
	if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) {/* socket creation  */
		perror("socket");
		exit(0);
	}
	ser_addr.sin_family=AF_INET;/* domain family is set to ipv4*/
	ser_addr.sin_port=htons(SER_PORT);
	ser_addr.sin_addr.s_addr=inet_addr("0.0.0.0");
	slen=sizeof(ser_addr);
	if(bind(sock_fd,(const struct sockaddr *)&ser_addr,slen)<0) {
		perror("bind");
		exit(0);
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
		cli_addr.sin_family=AF_INET;/* domain family is set to ipv4*/
		cli_addr.sin_port=htons(SER_PORT);
		clen=sizeof(ser_addr);
		memset(userdata,0,30);
		if((recvfrom(sock_fd,userdata,30,0,(struct sockaddr *)&cli_addr,&clen)) < 0) {
			perror("recvfrom");
			exit(0);
		}
		memcpy(commandstr,userdata,sizeof(int));
		command = *((int *)commandstr);
		memcpy(data,userdata+4,26);
		switch(command) {
			case COMMAND_SET_TAKE_SNAPSHOT:
				value = *((int *)data);
				g_imagesavetype = value;
				g_take_snapshot = TRUE;
				ret = 0;
				break;
			case COMMAND_SET_RECORD_VIDEO:
				value = *((int *)data);
				g_record_video = value;
				ret = 0;
				break;
		}
		if(ret == 0) {
			strcpy(status,"OK");
		} else if(ret == -1) {
			strcpy(status,"FAIL");
		}
		if((sendto(sock_fd,status,5,0,(const struct sockaddr *)&cli_addr,clen))<0) {
			perror("sendto");
		}
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
