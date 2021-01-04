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
#define SMALL_BUF 200

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
	addr.sin_addr.s_addr = inet_addr("192.168.50.197");

	if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect error %d %s\n", errno, strerror(errno));
		exit(1);
	}

	char Request[BUF_SIZE];
	sprintf(Request,
			"POST / HTTP/1.1\r\nHost: %s\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n",
			"192.168.50.41", SMALL_BUF);
	int len = strlen(Request);
	for(int i = 0; i < SMALL_BUF; i++)
		Request[len+i] = 'a';

	char *method = "GET / HTTP/1.1\r\nHost: 192.168.50.41\r\n\r\n";
	printf("%s\n", Request);
	printf("%d\n", (int)strlen(Request));
	//if(send(sock, method, strlen(method), 0) < 0){
	if(send(sock, Request, strlen(Request), 0) < 0){
		fprintf(stderr, "send error %d %s\n", errno, strerror(errno));
		exit(1);
	}

	recv(sock,msg,BUF_SIZE,0);
	printf("%s \n",msg);

	//recv(sock,msg,BUF_SIZE,0);
	//printf("%s \n",msg);

	close(sock);
	return 0;
}
