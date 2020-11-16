#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int sd;		//socket descriptor
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){	//get socket descriptor for udp
		fprintf(stderr, "socket error\n");
		exit(1);
	}

	struct sockaddr_in src;
	src.sin_family = AF_INET;	//IPv4
	src.sin_addr.s_addr = htonl(INADDR_ANY);	//ip address

	struct sockaddr_in dst;
	src.sin_family = AF_INET;	//IPv4
	src.sin_addr.s_addr = htonl(INADDR_ANY);	//ip address

	if(bind(sd, (struct sockaddr *)&src, sizeof(struct sockaddr)) < 0){		//bind socket with ip address
		fprintf(stderr, "bind error\n");
		exit(1);
	}

	void *buffer = (void *)malloc(BUFFER_SIZE);
	write(buffer, "Hello, This is message!\n", strlen("Hello, THis is mesage!\n"));

	free(buffer);

	close(sd);

	return 0;
}
