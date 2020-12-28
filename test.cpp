#include "header.h"
#include "make_ipv4.h"
#include "make_tcp.h"
#include "receiver.h"
int main() {
	char tcpsyn_src_ip[] = "192.168.56.3";
	char tcpsyn_dest_ip[] = "192.168.56.1";
	int tcpsyn_src_port = 11111;
	int tcpsyn_dest_port = 11111;
/*
	int sock = make_socket(IPPROTO_TCP);
	char msg[]= "GET / HTTP/1.1\r\n"
	                "Host: 192.241.213.46:6880\r\n"
	                "Upgrade-Insecure-Requests: 1\r\n"
	                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,/;q=0.8\r\n"
	                "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8\r\n"
	                "Accept-Language: en-us\r\n"
	                "Accept-Encoding: gzip, deflate\r\n"
	                "Connection: keep-alive\r\n\r\n";
	printf("MSG size : %d\n",strlen(msg));
	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

	/*** If you want to modify ip address*/
	//next_ip_addr(tcpsyn_src_ip, 1);

	/*
	ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
	tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
	tcp_h = tcp_set_seq(tcp_h, 1);
*/
	//tcp_h = tcp_set_ack_seq(tcp_h,35623);
	/***For SYN TCP request, ACK seq should not be provided.*/

	//tcp_h = tcp_set_syn_flag(tcp_h);
/*
	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, msg,strlen(msg));
	char *tcp_with_data = tcp_add_data(tcp_h, msg, strlen(msg));


	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h)+strlen(msg));
	char *packet = packet_assemble(ipv4_h, tcp_with_data, sizeof(tcp_h)+strlen(msg));
	//send_packet(sock, ipv4_h, packet, tcpsyn_dest_port);

	printf("Sent Size: %d\n",sizeof(ipv4_h)+sizeof(tcp_h)+strlen(msg));

	free(tcp_with_data);
	free(packet);
int SIZE=100;
	char *str = (char*)malloc(sizeof(char)*SIZE);
	memset(str,'#',SIZE);
	str[SIZE-1] = '\0';
	printf("%s\n",str);

	int socket = tcp_make_socket(inet_addr(tcpsyn_src_ip),inet_addr(tcpsyn_dest_ip),12345,55555);
*/
	while(1)
	{
	int sock = make_socket(IPPROTO_TCP);
		struct iphdr ipv4_h;
		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

		/*** If you want to modify ip address*/
		//next_ip_addr(tcpsyn_src_ip, 1);
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h;

		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, 1);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));




		char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

		printf("%d\n", ((struct iphdr*) packet)->tot_len);

		send_packet(sock, ipv4_h, packet, tcpsyn_dest_port);
		free(packet);

		struct recv recvd;
		recvd = get_response(sock);

		unsigned char data[__MAX_RECV_MSG_LENGTH__];
		memset(data,0,__MAX_RECV_MSG_LENGTH__);
		packet_dismantle(recvd,NULL,data);
		printf("size of tcphdr : %d\n",sizeof(struct tcphdr));
		printf("size of iphdr : %d\n",sizeof(struct iphdr));

		__u32 seq_num;
		__u32 ack_seq_num;

		memcpy(&seq_num,data+4,4);
		memcpy(&ack_seq_num,data+8,4);
		seq_num = ntohl(seq_num);
		ack_seq_num = ntohl(ack_seq_num);

		struct iphdr ipv4_h2;
		ipv4_h2 = prepare_empty_ipv4();
		ipv4_h2 = ipv4_set_protocol(ipv4_h2, IPPROTO_TCP);
		ipv4_h2 = ipv4_set_saddr(ipv4_h2, inet_addr(tcpsyn_src_ip));

		/*** If you want to modify ip address*/
		//next_ip_addr(tcpsyn_src_ip, 1);
		ipv4_h2 = ipv4_set_daddr(ipv4_h2, inet_addr(tcpsyn_dest_ip));

		struct tcphdr tcp_h2;
		tcp_h2 = prepare_empty_tcp();
		tcp_h2 = tcp_set_source(tcp_h2, tcpsyn_src_port);
		tcp_h2 = tcp_set_dest(tcp_h2, tcpsyn_dest_port);
		//tcp_h2 = tcp_set_ack_seq(tcp_h2, seq_num);
		tcp_h2 = tcp_set_seq(tcp_h2, ack_seq_num);
		tcp_h2 = tcp_set_ack_seq(tcp_h2,seq_num+1);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h2 = tcp_set_ack_flag(tcp_h2);

		tcp_h2 = tcp_get_checksum(ipv4_h2, tcp_h2,NULL, 0);

		ipv4_h2= ipv4_add_size(ipv4_h2, sizeof(tcp_h2));


		ipv4_h = prepare_empty_ipv4();
		ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
		ipv4_h = ipv4_set_saddr(ipv4_h, inet_addr(tcpsyn_src_ip));

		/*** If you want to modify ip address*/
		//next_ip_addr(tcpsyn_src_ip, 1);
		ipv4_h = ipv4_set_daddr(ipv4_h, inet_addr(tcpsyn_dest_ip));

		tcp_h = prepare_empty_tcp();
		tcp_h = tcp_set_source(tcp_h, tcpsyn_src_port);
		tcp_h = tcp_set_dest(tcp_h, tcpsyn_dest_port);
		tcp_h = tcp_set_seq(tcp_h, 1);

		//tcp_h = tcp_set_ack_seq(tcp_h,35623);
		/***For SYN TCP request, ACK seq should not be provided.*/

		tcp_h = tcp_set_syn_flag(tcp_h);

		tcp_h = tcp_get_checksum(ipv4_h, tcp_h,NULL, 0);

		ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));



		char *packet2 = packet_assemble(ipv4_h2, &tcp_h2, sizeof(tcp_h2));

		printf("%d\n", ((struct iphdr*) packet2)->tot_len);

		send_packet(sock, ipv4_h2, packet2, tcpsyn_dest_port);

		free(packet2);
/*
		ipv4_h2.tot_len -= sizeof(tcp_h2);
		tcp_h2.ack=0;
		tcp_h2.fin = 1;
		tcp_h2 = tcp_get_checksum(ipv4_h2,tcp_h2,NULL,0);

		char *packet3 = packet_assemble(ipv4_h2,&tcp_h2,sizeof(tcp_h2));

		send_packet(sock,ipv4_h2,packet3,tcpsyn_dest_port);
		free(packet3);*/

		close(sock);
		tcpsyn_src_port++;
		tcpsyn_dest_port++;

	exit(1);
		//free(recvd.msg);

		//next_ip_addr(tcpsyn_src_ip ,1);

	}
}
