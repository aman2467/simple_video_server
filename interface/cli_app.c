/* @file                : udp_client.c
 * @author              : Aman Kumar
 * @brief               : This is the program using udp socket for client.
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include <command_list.h>

main(int argc, char **argv) 
{

    int sock_fd;/*socket descriptor*/
    char wrbuff[30]={0};
    socklen_t slen;
    struct sockaddr_in ser_addr;
	int command;

    if(argc!=3) {
        printf("%s <port number> <server_IP>\n",argv[0]);
        exit(0);
    }

    if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))<0) { /* socket creation  */
        perror("socket");
        exit(0);
    }

    /*setting details for bind operation*/
    ser_addr.sin_family=AF_INET;/*domain family set to ipv4*/
    ser_addr.sin_port=htons(atoi(argv[1])); /* port number from command line argument*/
    ser_addr.sin_addr.s_addr=inet_addr(argv[2]); /*server-ip to be connected, also given from command line*/
    slen=sizeof(ser_addr);
    while(1) {

        memset(wrbuff,0,30);/*clearing write buffer*/
		command = TAKE_SNAPSHOT;
        /* sending message to the server*/
		memcpy(wrbuff,&command,sizeof(int));
        if((sendto(sock_fd,wrbuff,30,0,(const struct sockaddr *)&ser_addr,slen))<0) {
            perror("sendto");
            exit(0);
        }
		break;	
    }
    close(sock_fd);/*closing socket*/
}
