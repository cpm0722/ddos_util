#include <stdlib.h>
#include <string.h>


#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>


#include "make_tcp.h"
#include "make_ipv4.h"


int main(void)
{
	int a =0;
	while(1)
	{

	int sock = make_socket(IPPROTO_TCP);
	char src_ip[] = "10.0.2.15";
	char dest_ip[] = "192.168.1.1";
	
	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(src_ip));
	ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(dest_ip));	

	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h,33333);
	tcp_h = tcp_set_dest(tcp_h,33333);
	tcp_h = tcp_set_seq(tcp_h,a++);
	//tcp_h = tcp_set_ack_seq(tcp_h,35623);

	tcp_h = tcp_set_syn_flag(tcp_h);

	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, 0);

	char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));
	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));

	send_packet(sock,ipv4_h, packet,tcp_h.dest);

	free(packet);
	}
}
