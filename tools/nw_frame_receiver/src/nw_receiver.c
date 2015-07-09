/* ===========================================================================
 * @file    : nw_receiver.c
 *
 * @description : This file contains the main code which receives video packets
 *				from network.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General Public License.
 *              You may obtain a copy of the GNU General Public License Version 2 or later
 *              at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <nw_receiver.h>

/* global variables : can be used by threads */
unsigned int q_size=0;
unsigned int tempcount=0;
unsigned int g_frame_count = 10;
unsigned int g_skipframe = 0;
unsigned int g_capture_width = 640;
unsigned int g_capture_height = 480;
unsigned int buffsize;
unsigned int packetsize;
int g_video_port = VIDEO_PORT;
int g_last_line = 480;
char g_sendcount[20];
char *g_displaybuff = NULL;
char g_board_ip[20] = {0};

lock_t q_lock;
lock_t cnt_lock;

VIDEO_DATA *q_head=NULL, *q_tail=NULL;

extern void *displayThread(void *);

/****************************************************************************
 * @usage : This function waits for given time
 *
 * @arg1   : void
 * @return     : void
 * *************************************************************************/
void spin(void)
{
	usleep(5);
}

/****************************************************************************
 * @usage : This function inits given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 * *************************************************************************/
void lock_init(lock_t *lock)
{
	__sync_lock_release(&lock->lock);
}

/****************************************************************************
 * @usage : This function locks given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 * *************************************************************************/
void lock(lock_t *lock)
{
	while (__sync_lock_test_and_set(&lock->lock, 1))
		while (lock->lock)
			spin();
}

/****************************************************************************
 * @usage : This function unlocks given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 * *************************************************************************/
void unlock(lock_t *lock)
{
	__sync_lock_release(&lock->lock);
}

void init_lock(void)
{
	lock_init(&q_lock);
	lock_init(&cnt_lock);
}

/**********************************************************************
 * @addpacket : This function add one node to the existing queue of data.
 *
 * @arg  : pointer to the received packet of data
 * @return value     : void
 * ********************************************************************/
void addpacket(char *video_packet)
{
	VIDEO_DATA *newpacket;
	char *tempdata = calloc(1, sizeof(int));
	int *num;

	num = (int *)tempdata;
	newpacket = (VIDEO_DATA *)calloc(1, sizeof(VIDEO_DATA));
	newpacket->packetbuff = (char *)calloc(1, packetsize - VALID_DATA);
	memcpy(tempdata, video_packet, LINE_NUM_SIZE);
	newpacket->line_num = *(int *)num;
	memcpy(tempdata, video_packet + LINE_NUM_SIZE, FRAME_NUM_SIZE);
	newpacket->frame_num = *(int *)num;
	memcpy(newpacket->packetbuff, video_packet + VALID_DATA, packetsize - VALID_DATA);
	newpacket->next=NULL;
	free(tempdata);

	lock(&q_lock);
	if(q_head == NULL) {
		q_head = newpacket;
	} else {
		q_tail->next = newpacket;
	}
	q_tail = newpacket;
	unlock(&q_lock);
	lock(&cnt_lock);
	q_size++;
	unlock(&cnt_lock);
}

/**********************************************************************
 * @poppacket : This function pops one node from the existing queue of data.
 *
 * @arg  : void
 * @return value     : void
 * ********************************************************************/
void poppacket(VIDEO_DATA *empty_packet)
{
	VIDEO_DATA *curr_head;

	lock(&q_lock);
	if(q_head == NULL) {
		unlock(&q_lock);
		return;
	}

	curr_head = q_head;
	empty_packet->frame_num = curr_head->frame_num;
	empty_packet->line_num = curr_head->line_num;
	memcpy(empty_packet->packetbuff, curr_head->packetbuff, packetsize - VALID_DATA);
	free(curr_head->packetbuff);
	q_head = curr_head->next;
	free(curr_head);
	unlock(&q_lock);

	lock(&cnt_lock);
	q_size--;
	unlock(&cnt_lock);
}

/**********************************************************************
 * @sizeofqueue : This function returns the no of nodes in the existing
 *				queue of data.
 *
 * @arg  : void
 * @return value     : no of nodes currently present in the queue of data.
 * ********************************************************************/
int sizeofqueue(void)
{
	return q_size;
}

/**********************************************************************
 * @usage : This function prints the usage and help menu.
 *
 * @arg  : char pointer to the first entered argument from command line
 * @return value     : void
 * ********************************************************************/
void usage(char *name)
{
	printf("Help : %s -h	(show this help)\n",name);
	printf("Usage: %s -w <width> -h <height> -v <video_port>\n",name);
}

/**********************************************************************
 * @main : This is the main function of the application.
 *
 * @arg  : command line arguments
 * @return value     : 0
 * ********************************************************************/
int main(int argc, char **argv)
{
	int threadStatus = 0;
	VIDEO_DATA *temp;
	pthread_t tDisplayThread;
	int line = LINE_CNT, i;
	int sd_vdo;/*socket descriptor*/
	socklen_t sendr_len, recvr_len;
	struct sockaddr_in sendr_addr, recvr_addr;
	char *recvbuff = NULL;

	if(((argc == 2) && (strcmp(argv[1], "-h") == 0))) {
		usage(argv[0]);
		exit(0);
	}

	for(i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-w") == 0) {
			g_capture_width = atoi(argv[++i]) + 1;
		} else if (strcmp(argv[i], "-h") == 0) {
			g_capture_height = atoi(argv[++i]);
			g_last_line = g_capture_height/LINE_CNT;
		} else if (strcmp(argv[i], "-v") == 0) {
			g_video_port = atoi(argv[++i]);
		}
	}

	pr_dbg("Video port : %d\n", g_video_port);
	pr_dbg("Resolution : %dx%d\n", g_capture_width, g_capture_height);

	buffsize = g_capture_width * g_capture_height * BPP;
	packetsize = LINE_CNT * g_capture_width * BPP + VALID_DATA;
	g_displaybuff = calloc(1, buffsize);

	init_lock();

	/* initialize socket */
	if((sd_vdo = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket vdo");
		exit(0);
	}

	/*domain family set to ipv4*/
	recvr_addr.sin_family = AF_INET;
	/*server-ip to be connected */
	recvr_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	/* port number where server will send video packets*/
	recvr_addr.sin_port = htons(g_video_port);
	recvr_len = sizeof(recvr_addr);
	sendr_len = sizeof(sendr_addr);

	if(bind(sd_vdo, (const struct sockaddr *)&recvr_addr, recvr_len)<0) {
		perror("bind");
		exit(0);
	}

	if(pthread_create(&tDisplayThread, NULL, displayThread, NULL)) {
		printf("Display Thread create fail\n");
		exit(0);
	}
	pr_dbg("Display Thread created\n");
	threadStatus |= DISP_THR;

	recvbuff = (char *)calloc(10, packetsize);
	while(TRUE) {
		/* receive video packets and push to queue */
		if((recvfrom(sd_vdo, recvbuff, packetsize, 0, (struct sockaddr *)&sendr_addr, &sendr_len))<0) {
			perror("recvfrom");
			exit(0);
		}
		addpacket(recvbuff);
	}

	if (threadStatus & DISP_THR) {
		KillDisplayThread = 1;
		pthread_join(tDisplayThread, NULL);
	}
	pr_dbg("Terminating Display Thread\n");

	if(sizeofqueue() > 0) {
		temp = (VIDEO_DATA *)calloc(1, sizeof(VIDEO_DATA));
		temp->packetbuff = calloc(1, packetsize - VALID_DATA);

		while(sizeofqueue() > 0) {
			poppacket(temp);
		}
		free(temp->packetbuff);
		free(temp);
	}

	if(g_displaybuff != NULL) {
		free(g_displaybuff);
	}

	free(recvbuff);
	close(sd_vdo);
	return 0;
}
