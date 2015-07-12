/* =============================================================================
 * @file    : network_stream.c
 *
 * @description : This file contains the source code to transmit the captured 
 *			video over network using UDP protocol.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU GPL.
 *              You may obtain a copy of the GNU GPL Version 2 or later
 *              at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <common.h>

extern char *g_streambuff;

/*******************************************************************************
 * @func    : get_frame
 *			This provides a frame to transmit 
 * @arg1    : pointer to active frame buffer
 * @return  : void
 ******************************************************************************/
char *get_frame(void)
{
	return g_streambuff;
}

/*******************************************************************************
 * @func    : networkstreamThread
 *			network stream thread function
 * @arg1    : void
 * @return  : void
 ******************************************************************************/
void *nwstreamThread(void)
{
	char *linebuff = NULL;
	unsigned int *lineptr, *frameptr;
	unsigned int bpl;
	char *transmit_frame = NULL;
	int sock_fd;
	socklen_t slen,clen;
	struct sockaddr_in ser_addr,cli_addr;
	char *ptr;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	/* socket creation  */
	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(VID_SER_PORT);
	ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	slen = sizeof(ser_addr);

	if(bind(sock_fd, (const struct sockaddr *)&ser_addr, slen) < 0) {
		perror("bind");
		exit(0);
	}

	bpl = BPP*serverConfig->capture.width;
	linebuff = (char *)calloc(1, LINE_CNT*bpl + 8);
	frameptr = (unsigned int *)(linebuff + 4);
	lineptr = (unsigned int *)linebuff;

	while(!KillStreamThread) {
		while(serverConfig->stream.enable == FALSE) {
			usleep(10);
		}
		cli_addr.sin_family = AF_INET;
		cli_addr.sin_port = htons(serverConfig->stream.video_port);
		cli_addr.sin_addr.s_addr = inet_addr(serverConfig->stream.client_ip);
		clen = sizeof(cli_addr);
		if(serverConfig->stream.enable == TRUE) {
			*lineptr = 0;
			*frameptr = 0;
			while(TRUE == serverConfig->stream.enable) {
				(*(frameptr))++;
				*(lineptr) = 0;
				transmit_frame = get_frame();
				ptr = transmit_frame;
				/* transmit a frame */
				while(ptr < transmit_frame + serverConfig->capture.framesize) {
					(*(lineptr))++;
					memcpy(linebuff + 8, ptr, LINE_CNT*bpl);
					if((sendto(sock_fd, linebuff, LINE_CNT*bpl + 8, 0, (const struct sockaddr *)&cli_addr, clen))<0) {
						perror("sendto");
						exit(0);
					}
					ptr += LINE_CNT*bpl;
				}
			}
		}
	}
	free(linebuff);
	close(sock_fd);
	printf("Network stream terminated\n");
	return 0;
}

