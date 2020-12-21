#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

int main(int argc, char *argv[])
{

	char msg[BUF_SIZE];

	int sock;
	if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "socket error %d %s\n", errno, strerror(errno));
		exit(1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = inet_addr("192.168.50.144");

	if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
		exit(1);
	}

	char *method = "GET / HTTP/1.1\r\nHost: www.google.co.kr\r\n\r\n";
	if(send(sock, method, strlen(method), 0) < 0){
		fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
		exit(1);
	}

	recv(sock,msg,BUF_SIZE,0);
	printf("%s \n",msg);

	recv(sock,msg,BUF_SIZE,0);
	printf("%s \n",msg);

	close(sock);
	return 0;
}
