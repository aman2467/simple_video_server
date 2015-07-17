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

int g_writeflag = 0;
int g_osdflag = 0;

char *g_framebuff[NUM_BUFFER] = {NULL};
char *g_osdbuff[NUM_BUFFER] = {NULL};
char *g_streambuff = NULL;
char *ascii_string;
char ascii_data[STRING_WIDTH*TEXT_HEIGHT*TEXT_WIDTH*BPP];

int current_task;
lock_t buf_lock;
lock_t stream_lock;
SERVER_CONFIG g_server_config;

extern void *captureThread(void *);
extern void *filerecordThread(void *);
extern void *osdThread(void *);
extern void *jpegsaveThread(void *);
extern void *displayThread(void *);
extern void *nwstreamThread(void *);

/****************************************************************************
 * @usage : This function returns pointer global configuration structure.
 *
 * @arg  : void
 * @return     : pointer to global SERVER_CONFIG structure
 * *************************************************************************/
SERVER_CONFIG *GetServerConfig(void)
{
	return (&g_server_config);
}

/****************************************************************************
 * @usage : This function prints the usage and help menu.
 *
 * @arg  : char pointer to the first entered argument from command line
 * @return     : void
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
	printf(GREEN"\t\tdisplay          "NONE":"YELLOW" To enable local display\n");
	printf(GREEN"\t\tstream           "NONE":"YELLOW" To enable network streaming\n");
	printf(GREEN"\t\t-d <device>      "NONE":"YELLOW" To capture video from given device\n");
	printf(GREEN"\t\t-w <width>       "NONE":"YELLOW" Supported capture width\n");
	printf(GREEN"\t\t-h <height>      "NONE":"YELLOW" Supported capture height"NONE"\n");
	pr_dbg(YELLOW"\n/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/"NONE"\n\n");
	exit(0);
}

/****************************************************************************
 * @usage : This is function to initialize Server configurations.
 *
 * @arg  : pointer to global server configuration structure
 * @return     : void
 * *************************************************************************/
void Init_Server(SERVER_CONFIG *serverConfig)
{
	/* threads settings */
	serverConfig->enable_osd_thread = FALSE;
	serverConfig->enable_imagesave_thread = FALSE;
	serverConfig->enable_videosave_thread = FALSE;
	serverConfig->enable_stream_thread = FALSE;
	serverConfig->enable_display_thread = FALSE;
	/* algorithm settings */
	serverConfig->algo_type = ALGO_NONE;
	/* JPEG Param settings */
	serverConfig->jpeg.quality = 300;
	serverConfig->jpeg.framebuff = NULL;
	/* Capture settings */
	strcpy(serverConfig->capture.device,"/dev/video0");
	serverConfig->capture.width = 640;
	serverConfig->capture.height = 480;
	serverConfig->capture.framesize = serverConfig->capture.width*serverConfig->capture.height*BPP;
	/* network settings */
	serverConfig->nw.port = CLI_SER_PORT;
	strcpy(serverConfig->nw.ip,"127.0.0.1");
	/* video record settings */
	serverConfig->video.recordenable = FALSE;
	serverConfig->video.osd_on = FALSE;
	serverConfig->video.type = TYPE_NONE;
	/* image record settings */
	serverConfig->image.recordenable = FALSE;
	serverConfig->image.osd_on = FALSE;
	serverConfig->image.type = TYPE_NONE;
	/* display settings */
	serverConfig->disp.display_frame = NULL;
	serverConfig->disp.sdl_frame = NULL;
	/* stream settings */
	serverConfig->stream.enable = FALSE;
	serverConfig->stream.video_port = STREAM_PORT;
	strcpy(serverConfig->stream.client_ip, CLIENT_IP);
}

/****************************************************************************
 * @main : This is the main function of the application. It creates separate
 *          threads for capture, save, etc and later acts as command server.
 *
 * @arg  : command line arguments
 * @return     : 0
 * *************************************************************************/
int main(int argc, char **argv)
{
	int threadStatus = 0;
	pthread_t tCaptureThread, tOsdThread, tFilerecordThread, tJpegsaveThread;
	pthread_t tDisplayThread, tStreamThread;
	int i;
	int command;
	int arg1, arg2, arg3;
	int ret = 0;
	char commandstr[4];
	char status[10]={0};
	char userdata[30];
	char data[26];
	int sock_fd;
	socklen_t slen,clen;
	struct sockaddr_in ser_addr,cli_addr;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if((argc==2 && strcmp(argv[1],"-h")==0)) {
		usage(argv[0]);
	}

	Init_Server(serverConfig);

	for(i=1;i<argc;i++) {
		if (strcmp(argv[i], "osd") == 0) {
			serverConfig->enable_osd_thread = TRUE;
			serverConfig->enable_videosave_thread = TRUE;
		} else if (strcmp(argv[i], "videosave") == 0) {
			serverConfig->enable_videosave_thread = TRUE;
		} else if (strcmp(argv[i], "imagesave") == 0) {
			serverConfig->enable_imagesave_thread = TRUE;
		} else if (strcmp(argv[i], "display") == 0) {
			serverConfig->enable_display_thread = TRUE;
		} else if (strcmp(argv[i], "stream") == 0) {
			serverConfig->enable_stream_thread = TRUE;
		} else if (strcmp(argv[i], "-d") == 0) {
			strcpy(serverConfig->capture.device,argv[++i]);
		} else if (strcmp(argv[i], "-w") == 0) {
			serverConfig->capture.width = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-h") == 0) {
			serverConfig->capture.height = atoi(argv[++i]);
		}
	}

	lock_init(&buf_lock);
	lock_init(&stream_lock);
	serverConfig->capture.framesize = serverConfig->capture.width*serverConfig->capture.height*BPP;
	if(serverConfig->enable_display_thread) {
		if(!serverConfig->enable_osd_thread) {
			serverConfig->disp.sdl_frame = calloc(1,serverConfig->capture.framesize);
		}
		serverConfig->disp.display_frame = calloc(1,serverConfig->capture.framesize);
	}
	for(i = 0; i < NUM_BUFFER ;i++) {
		if(serverConfig->enable_videosave_thread) {
			g_framebuff[i] = (char *)calloc(1,serverConfig->capture.framesize);
			memset(g_framebuff[i],0,serverConfig->capture.framesize);
		}
		if(serverConfig->enable_osd_thread) {
			g_osdbuff[i] = (char *)calloc(1,serverConfig->capture.framesize);
			memset(g_osdbuff[i],0,serverConfig->capture.framesize);
		}
	}
	if(serverConfig->enable_stream_thread) {
		g_streambuff = (char *)calloc(1,serverConfig->capture.framesize);
		memset(g_streambuff,0,serverConfig->capture.framesize);
	}
	KillCaptureThread = 0;
	if(pthread_create(&tCaptureThread, NULL, captureThread, NULL)) {
		printf("Capture Thread create fail\n");
		exit(0);
	}
	pr_dbg("Capture Thread created\n");
	threadStatus |= CAPTURE_THR;

	if(serverConfig->enable_osd_thread == TRUE) {
		KillOsdThread = 0;
		if(pthread_create(&tOsdThread, NULL, osdThread, NULL)) {
			perror("OSD Thread create fail\n");
			exit(0);
		}
		pr_dbg("OSD Thread created\n");
		threadStatus |= OSD_THR;
	}

	if(serverConfig->enable_videosave_thread == TRUE) {
		KillFilerecordThread = 0;
		if(pthread_create(&tFilerecordThread, NULL, filerecordThread, NULL)) {
			perror("File Record Thread create fail\n");
			exit(0);
		}
		pr_dbg("File Record Thread created\n");
		threadStatus |= FILERECORD_THR;
	}

	if(serverConfig->enable_imagesave_thread == TRUE) {
		KillJpegsaveThread = 0;
		if(pthread_create(&tJpegsaveThread, NULL, jpegsaveThread, NULL)) {
			perror("JPEG Save Thread create fail\n");
			exit(0);
		}
		pr_dbg("JPEG Save Thread created\n");
		threadStatus |= JPEGSAVE_THR;
	}

	if(serverConfig->enable_display_thread) {
		KillDisplayThread = 0;
		if(pthread_create(&tDisplayThread, NULL, displayThread, NULL)) {
			perror("Display Thread create fail\n");
			exit(0);
		}
		pr_dbg("Display Thread created\n");
		threadStatus |= DISPLAY_THR;
	}

	if(serverConfig->enable_stream_thread) {
		KillStreamThread = 0;
		if(pthread_create(&tStreamThread, NULL, nwstreamThread, NULL)) {
			perror("Network Stream Thread create fail\n");
			exit(0);
		}
		pr_dbg("Network Stream Thread created\n");
		threadStatus |= STREAM_THR;
	}

	if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) {/* socket creation  */
		perror("socket");
		exit(0);
	}
#ifndef STANDALONE
	ser_addr.sin_family=AF_INET;/* domain family is set to ipv4*/
	ser_addr.sin_port=htons(CLI_SER_PORT);
	ser_addr.sin_addr.s_addr=inet_addr("0.0.0.0");
	slen=sizeof(ser_addr);
	cli_addr.sin_family=AF_INET;/* domain family is set to ipv4*/
	cli_addr.sin_port=htons(CLI_SER_PORT);
	clen=sizeof(cli_addr);
	if(bind(sock_fd,(const struct sockaddr *)&ser_addr,slen)<0) {
		perror("bind");
		exit(0);
	}
#endif
#ifdef _DEBUG
	pr_dbg(YELLOW"/************************************************************/"NONE"\n");
	pr_dbg(CYAN"\tCapture Device \t\t:\t"GREEN" %s"NONE"\n", serverConfig->capture.device);
	pr_dbg(CYAN"\tCapture resolution \t:\t"GREEN" %d x %d"NONE"\n",serverConfig->capture.width,
															serverConfig->capture.height);
	if(serverConfig->enable_osd_thread == TRUE) {
		pr_dbg(CYAN"\tOSD \t\t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tOSD \t\t\t:\t"RED" NO"NONE"\n");
	}
	if(serverConfig->enable_videosave_thread == TRUE) {
		pr_dbg(CYAN"\tFile recording \t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tFile recording \t\t:\t"RED" NO"NONE"\n");
	}
	if(serverConfig->enable_imagesave_thread == TRUE) {
		pr_dbg(CYAN"\tJPEG save \t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tJPEG save \t\t:\t"RED" NO"NONE"\n");
	}
	if(serverConfig->enable_display_thread == TRUE) {
		pr_dbg(CYAN"\tDisplay \t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tDisplay \t\t:\t"RED" NO"NONE"\n");
	}
	if(serverConfig->enable_stream_thread == TRUE) {
		pr_dbg(CYAN"\tStreaming \t\t:\t"GREEN" YES"NONE"\n");
	} else {
		pr_dbg(CYAN"\tStreaming \t\t:\t"RED" NO"NONE"\n");
	}
	pr_dbg(YELLOW"/************************************************************/"NONE"\n");
#endif

	while(1) {
#ifndef STANDALONE
		memset(userdata,0,30);
		if((recvfrom(sock_fd,userdata,30,0,(struct sockaddr *)&cli_addr,&clen)) < 0) {
			perror("recvfrom");
			exit(0);
		}
		memcpy(commandstr,userdata,sizeof(int));
		command = *((int *)commandstr);
		memset(data,0,26);
		memcpy(data,userdata+4,26);
		switch(command) {
			case COMMAND_SET_TAKE_SNAPSHOT:
				arg1 = *((int *)data);
				if(serverConfig->enable_imagesave_thread) {
					serverConfig->image.type = arg1;
					serverConfig->image.recordenable = TRUE;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_RECORD_VIDEO:
				arg1 = *((int *)data);
				if(serverConfig->enable_videosave_thread) {
					serverConfig->video.recordenable = arg1;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_VIDEO_TYPE:
				arg1 = *((int *)data);
				if(serverConfig->enable_videosave_thread || serverConfig->enable_stream_thread) {
					serverConfig->video.type = arg1;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_ALGO_TYPE:
				arg1 = *((int *)data);
				serverConfig->algo_type = arg1;
				ret = 0;
				break;
			case COMMAND_SET_OSD_ENABLE:
				arg1 = *((int *)data);
				arg2 = *((int *)(data+4));
				if(serverConfig->enable_osd_thread) {
					set_osd_window_enable(arg1, arg2);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_OSD_TEXT:
				arg1 = *((int *)data);
				if(serverConfig->enable_osd_thread) {
					set_osd_window_text(arg1,data+4);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_OSD_POSITION:
				arg1 = *((int *)data);
				arg2 = *((int *)(data+4));
				arg3 = *((int *)(data+8));
				if(arg1 < 5 && arg2%2 !=0) arg2+=1;
				if(serverConfig->enable_osd_thread) {
					set_osd_window_position(arg1,arg2,arg3);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_OSD_ON_IMAGE:
				arg1 = *((int *)data);
				if(serverConfig->enable_osd_thread && serverConfig->enable_imagesave_thread) {
					serverConfig->image.osd_on = arg1;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_OSD_ON_VIDEO:
				arg1 = *((int *)data);
				if(serverConfig->enable_osd_thread && serverConfig->enable_videosave_thread) {
					serverConfig->video.osd_on = arg1;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_OSD_TRANSPARENCY:
				arg1 = *((int *)data);
				arg2 = *((int *)(data+4));
				if(serverConfig->enable_osd_thread) {
					set_osd_window_transparency(arg1, arg2);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_ENABLE_STREAM:
				arg1 = *((int *)data);
				if(serverConfig->enable_stream_thread) {
					serverConfig->stream.enable = arg1;
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_ENABLE_DATE:
				arg1 = *((int *)data);
				if(serverConfig->enable_osd_thread) {
					set_osd_window_enable(OSD_WINDOW_NINE, arg1);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			case COMMAND_SET_ENABLE_TIME:
				arg1 = *((int *)data);
				if(serverConfig->enable_osd_thread) {
					set_osd_window_enable(OSD_WINDOW_EIGHT, arg1);
					ret = 0;
				} else {
					ret = 1;
				}
				break;
			default:
				ret = -1;
				break;
		}
		if(ret == 0) {
			strcpy(status,"OK");
		} else if(ret == -1) {
			strcpy(status,"FAIL");
		} else if(ret == 1) {
			strcpy(status,"UNKNOWN");
		}
		if((sendto(sock_fd,status,10,0,(const struct sockaddr *)&cli_addr,clen))<0) {
			perror("sendto");
		}
#endif
		usleep(10);
	}

	if(serverConfig->enable_stream_thread == TRUE) {
		if(threadStatus & STREAM_THR){
			KillStreamThread = 1;
			pthread_join(tStreamThread, NULL);
		}
	}
	if(serverConfig->enable_display_thread == TRUE) {
		if(threadStatus & DISPLAY_THR){
			KillDisplayThread = 1;
			pthread_join(tDisplayThread, NULL);
		}
	}
	if(serverConfig->enable_imagesave_thread == TRUE) {
		if(threadStatus & JPEGSAVE_THR){
			KillJpegsaveThread = 1;
			pthread_join(tJpegsaveThread, NULL);
		}
	}
	if(serverConfig->enable_videosave_thread == TRUE) {
		if(threadStatus & FILERECORD_THR){
			KillFilerecordThread = 1;
			pthread_join(tFilerecordThread, NULL);
		}
	}
	if(serverConfig->enable_osd_thread == TRUE) {
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
