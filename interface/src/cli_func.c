/*****************************************************************************
 *
 * @file   cli_function.c
 * @brief  This program contains all the functions called by cli functions.
 *
 *****************************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <cli_app.h>
#include <command_list.h>

extern char g_video_server_ip[16];

int sendCommand(int command, char *arg)
{
	int sock_fd;
    char wrbuff[30]={0};
	char status[5]={0};
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

	if((sendto(sock_fd,wrbuff,30,0,(const struct sockaddr *)&ser_addr,slen))<0) {
        perror("sendto");
        return FAILURE;
    }
	if((recvfrom(sock_fd,status,5,0,(struct sockaddr *)&ser_addr,&slen))<0) {
		perror("sendto");
		return FAILURE;
	}
	printf("\nServer(%s) message : %s\n",inet_ntoa(ser_addr.sin_addr),status);
    close(sock_fd);/*closing socket*/
	return SUCCESS;
}

int savesnap(int value)
{
	char arg[26];

	memcpy(arg,&value,sizeof(value));
	if(sendCommand(COMMAND_SET_TAKE_SNAPSHOT,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

int recordvideo(int value)
{
	char arg[26];

	memcpy(arg,&value,sizeof(value));
	if(sendCommand(COMMAND_SET_RECORD_VIDEO,arg) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

