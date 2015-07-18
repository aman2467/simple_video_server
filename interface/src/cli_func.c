/* ==========================================================================
 * @file    : cli_func.c
 *
 * @description : This file contains definition to CLI functions used for
 *                  communication to video server.
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <cli_app.h>
#include <command_list.h>

extern char g_video_server_ip[16];
extern int quit_flag;

#define DOT "."

/****************************************************************************
 * @usage : Called when SIGALRM signal is received, terminates the program
 *
 * @arg	: signal number
 * @return     : void
 * *************************************************************************/
void watchdog(int sig)
{
	char server_msg[] = "\nVideo Server Not Alive..";
	char exit_msg[] = "\nTerminating Application...";
	char *ch;

	if(quit_flag == TRUE) {
		ch = server_msg;
		while(*ch != '\0') {
			printf("\x1b[01;32m%c",*ch);
			fflush(stdout);
			usleep(100000);
			ch++;
		}
		sleep(2);
		ch = exit_msg;
		while(*ch != '\0') {
			printf("\x1b[01;31m%c",*ch);
			fflush(stdout);
			usleep(100000);
			ch++;
		}
		sleep(2);
		printf("\x1b[0m\n");
		exit(0);
	}
}

/****************************************************************************
 * @usage : helper function to validate digits of an IP string
 *
 * @arg	: IP string
 * @return     : success/failure
 * *************************************************************************/
int valid_digit(char *ip_str)
{
	while (*ip_str) {
		if (*ip_str >= '0' && *ip_str <= '9')
			++ip_str;
		else
			return 0;
	}
	return 1;
}

/****************************************************************************
 * @usage : Validate the correctness of an IP string
 *
 * @arg	: IP string
 * @return     : success/failure
 * *************************************************************************/
int is_valid_ip(char *ip_str)
{
	int i, num, dots = 0;
	char *ptr;

	if (ip_str == NULL)
		return 0;
	ptr = strtok(ip_str, DOT);
	if (ptr == NULL)
		return 0;
	while (ptr) {
		/* after parsing string, it must contain only digits */
		if (!valid_digit(ptr))
			return 0;
		num = atoi(ptr);
		/* check for valid IP */
		if (num >= 0 && num <= 255) {
			/* parse remaining string */
			ptr = strtok(NULL, DOT);
			if (ptr != NULL)
				++dots;
		} else
			return 0;
	}

	/* valid IP string must contain 3 dots */
	if (dots != 3)
		return 0;
	return 1;
}

/****************************************************************************
 * @usage : This function sends a command to video server using UDP socket.
 *
 * @arg1  : command value
 * @arg2  : argument for command
 * @return     : success/failure of function
 * *************************************************************************/
int sendCommand(int command, char *arg)
{
	int sock_fd;
    char wrbuff[30]={0};
	char status[10]={0};
	char response[20]={0};
    socklen_t slen;
    struct sockaddr_in ser_addr;

    if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) { /* socket creation  */
        perror("socket");
        return FAILURE;
    }

    /*setting details for bind operation*/
    ser_addr.sin_family=AF_INET;/*domain family set to ipv4*/
    ser_addr.sin_port=htons(VIDEO_SERVER_PORT); /* port number*/
    ser_addr.sin_addr.s_addr=inet_addr(g_video_server_ip); /*server-ip to be connected*/
    slen=sizeof(ser_addr);

	memset(wrbuff,0,30);/*clearing write buffer*/
    /* sending message to the server*/
	memcpy(wrbuff,&command,sizeof(int));
	memcpy(wrbuff+4,arg,26);
	quit_flag = TRUE;
	alarm(WAIT_TIME);
	if((sendto(sock_fd,wrbuff,30,0,(const struct sockaddr *)&ser_addr,slen))<0) {
        perror("sendto");
        return FAILURE;
    }
	if((recvfrom(sock_fd,status,10,0,(struct sockaddr *)&ser_addr,&slen))<0) {
		perror("sendto");
		return FAILURE;
	}
	strcpy(response,status);
	if(strcmp(status,"UNKNOWN") == 0) {
		strcat(response," SERVICE");
	}
	quit_flag = FALSE;
	printf("\nServer(%s) message : %s\n",inet_ntoa(ser_addr.sin_addr),response);
    close(sock_fd);/*closing socket*/
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to take a snap.
 *
 * @arg  : value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int takesnap(int value)
{
	char arg[26] = {0};

	memcpy(arg,&value,sizeof(value));
	if(sendCommand(COMMAND_SET_TAKE_SNAPSHOT,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to record video.
 *
 * @arg  : value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int recordvideo(int value)
{
	char arg[26] = {0};

	memcpy(arg,&value,sizeof(value));
	if(sendCommand(COMMAND_SET_RECORD_VIDEO,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to set algo type
 *
 * @arg  : value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int setalgotype(int type)
{
	char arg[26] = {0};

	memcpy(arg,&type,sizeof(type));
	if(sendCommand(COMMAND_SET_ALGO_TYPE,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable OSD window
 *
 * @arg1  : OSD window number input from command line
 * @arg2  : enable/disable value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdwinenable(int win, int enable)
{
	char arg[26] = {0};

	memcpy(arg,&win,sizeof(win));
	memcpy(arg+sizeof(win),&enable,sizeof(enable));
	if(sendCommand(COMMAND_SET_OSD_ENABLE,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to display over OSD window
 *
 * @arg1  : OSD window number input from command line
 * @arg2  : text input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdwintext(int win, char *text)
{
	char arg[26] = {0};

	memcpy(arg,&win,sizeof(win));
	memcpy(arg+sizeof(win),text,strlen(text));
	if(sendCommand(COMMAND_SET_OSD_TEXT,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to change the position of OSD window
 *
 * @arg1  : OSD window number input from command line
 * @arg2  : position input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdwinpos(int win, int x, int y)
{
	char arg[26] = {0};

	memcpy(arg,&win,sizeof(win));
	memcpy(arg+sizeof(win),&x,sizeof(x));
	memcpy(arg+sizeof(win)+sizeof(x),&y,sizeof(y));
	if(sendCommand(COMMAND_SET_OSD_POSITION,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable OSD on saved image
 *
 * @arg  : value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdonimage(int enable)
{
	char arg[26] = {0};

	memcpy(arg,&enable,sizeof(enable));
	if(sendCommand(COMMAND_SET_OSD_ON_IMAGE,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable OSD on saved video.
 *
 * @arg  : value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdonvideo(int enable)
{
	char arg[26] = {0};

	memcpy(arg,&enable,sizeof(enable));
	if(sendCommand(COMMAND_SET_OSD_ON_VIDEO,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable OSD transparency.
 *
 * @arg1  : OSD window number input from command line
 * @arg2  : enable/disable value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int osdwintrans(int win, int enable)
{
	char arg[26] = {0};

	memcpy(arg,&win,sizeof(win));
	memcpy(arg+sizeof(win),&enable,sizeof(enable));
	if(sendCommand(COMMAND_SET_OSD_TRANSPARENCY,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable network streaming.
 *
 * @arg  : enable/disable value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int nw_stream(int enable)
{
	char arg[26] = {0};

	memcpy(arg, &enable, sizeof(enable));
	if(sendCommand(COMMAND_SET_ENABLE_STREAM, arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable date stamp.
 *
 * @arg  : enable/disable value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int enable_date(int enable)
{
	char arg[26] = {0};

	memcpy(arg, &enable, sizeof(enable));
	if(sendCommand(COMMAND_SET_ENABLE_DATE, arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

/****************************************************************************
 * @usage : This function sends command to enable/disable time stamp.
 *
 * @arg  : enable/disable value input from command line
 * @return     : success/failure of function
 * *************************************************************************/
int enable_time(int enable)
{
	char arg[26] = {0};

	memcpy(arg, &enable, sizeof(enable));
	if(sendCommand(COMMAND_SET_ENABLE_TIME, arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

