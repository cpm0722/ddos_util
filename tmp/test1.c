#include "header.h"
#include "make_ipv4.h"
#include "make_tcp.h"
int main() {
	char* tcpsyn_src_ip = (char*)malloc(sizeof(char)*15);
	memcpy(tcpsyn_src_ip,"10.0.2.15",15);
	char tcpsyn_dest_ip[] = "192.168.56.1";
	int tcpsyn_src_port = 12349;
	int tcpsyn_dest_port = 54323;


	while(1)
	{
		int port = 55555;
		int socket = tcp_make_socket(inet_addr(tcpsyn_src_ip),inet_addr(tcpsyn_dest_ip),port,55555);
		printf("Creating TCP connection with %s:%d\n",tcpsyn_src_ip,port);
		next_ip_addr(tcpsyn_src_ip,1);
		sleep(3);
	}



	getchar();
}
