#include "header.h"
#include "make_ipv4.h"
#include "make_tcp.h"
#include "receiver.h"
int main() {
	char tcpsyn_src_ip[] = "192.168.56.3";
	char tcpsyn_dest_ip[] = "192.168.56.1";
	int tcpsyn_src_port = 12349;
	int tcpsyn_dest_port = 55555;
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
		struct iphdr *ip_p = (struct iphdr*)malloc(sizeof(struct iphdr));
		struct tcphdr *tcp_p = (struct tcphdr*)malloc(sizeof(struct tcphdr));

		unsigned char *data = (unsigned char*)malloc(sizeof(struct tcphdr));
		packet_dismantle(recvd,ip_p,data);
		printf("size of tcphdr : %d\n",sizeof(struct tcphdr));
		printf("size of iphdr : %d\n",sizeof(struct iphdr));

		__u32 seq_num;
		memcpy(&seq_num,data+4,4);
		printf("written : ");
		int i;
		for(i=4;i<4+4;i++)
		{
			printf("%x,",data[i]);
		}
		printf("\n");


		unsigned int seq = tcp_get_seq(*tcp_p);
		unsigned int src = tcp_get_source(*tcp_p);
		unsigned int dest = tcp_get_dest(*tcp_p);

		printf("seq : %u\n src : %u\n dest : %u\n",seq_num,src,dest);




}
