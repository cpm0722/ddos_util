#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/make_tcp.h"

struct tcphdr prepare_empty_tcp() {

	struct tcphdr tcp_head;
	memset(&tcp_head, 0, sizeof(struct tcphdr));

	tcp_head.source = 0; //port
	tcp_head.dest = 0; //port
	tcp_head.seq = 0; //fill in later;
	tcp_head.ack_seq = 0; //fill in later
	tcp_head.doff = 5;

	tcp_head.ack = 0;
	tcp_head.syn = 0;
	tcp_head.fin = 0;

	tcp_head.res1 = 0;
	tcp_head.urg = 0;
	tcp_head.psh = 0;
	tcp_head.rst = 0;
	tcp_head.res2 = 0;

	tcp_head.window = htons(5840);
	tcp_head.check = 0;

	return tcp_head;
}

struct tcphdr tcp_set_source(struct tcphdr tcph, __u16 src_port) {
	tcph.source = htons(src_port);
	return tcph;
}

struct tcphdr tcp_set_dest(struct tcphdr tcph, __u16 dest_port) {
	tcph.dest = htons(dest_port);
	return tcph;
}

struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq) {
	tcph.seq = htonl(seq);
	return tcph;
}

struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq) {
	tcph.ack_seq = htonl(ack_seq);
	return tcph;
}

struct tcphdr tcp_set_syn_flag(struct tcphdr tcph) {
	tcph.syn = 1;
	return tcph;
}

struct tcphdr tcp_set_ack_flag(struct tcphdr tcph) {
	tcph.ack = 1;
	return tcph;
}

__u32 tcp_get_seq(struct tcphdr tcph) {

	return tcph.seq;
}
__u32 tcp_get_source(struct tcphdr tcph) {
	return tcph.source;
}
__u32 tcp_get_dest(struct tcphdr tcph) {
	return tcph.dest;
}

char* tcp_add_data(struct tcphdr tcph, void *data, int datasize) {
	char *return_data = (char*) malloc(sizeof(struct tcphdr) + datasize);
	memcpy(return_data, &tcph, sizeof(tcph));
	memcpy(return_data + sizeof(tcph), data, datasize);
	return return_data;
}

struct tcp_pseudo_header tcp_prepare_pseudo(struct iphdr ipv4h,
		struct tcphdr tcph, int add_datasize) {
	struct tcp_pseudo_header psh;
	memset(&psh, 0, sizeof(struct tcp_pseudo_header));
	psh.source_address = ipv4h.saddr;
	psh.dest_address = ipv4h.daddr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + add_datasize);
	return psh;
}

struct tcphdr tcp_get_checksum(struct iphdr ipv4h, struct tcphdr tcph,
		void *data, int datasize) {
	struct tcp_pseudo_header psh;
	psh = tcp_prepare_pseudo(ipv4h, tcph, datasize);
	int psize = sizeof(struct tcp_pseudo_header) + sizeof(struct tcphdr)
			+ datasize;
	char *assembled = (char*) malloc(psize);
	memcpy(assembled, (char*) &psh, sizeof(struct tcp_pseudo_header));
	memcpy(assembled + sizeof(struct tcp_pseudo_header), &tcph,
			sizeof(struct tcphdr));
	if (data != NULL && datasize != 0)
		memcpy(
				assembled + sizeof(struct tcp_pseudo_header)
						+ sizeof(struct tcphdr), data, datasize);

	tcph.check = in_cksum((__u16*) assembled, psize);

	return tcph;
}

//3way handshake completed socket
int tcp_make_connection(__u32 src_ip, __u32 dest_ip, int src_port,
		int dest_port, int type) {
	int sock;
	struct sockaddr_in local_addr, remote_addr;


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("sock create error\n");
		exit(1);
	}

	/*local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(src_port);
	local_addr.sin_addr.s_addr = src_ip;

	if (bind(sock, (struct sockaddr*) &local_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind failed\n");
		exit(1);
	}

	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval))
			== -1) {
		perror("sock opt err\n");
		exit(1);
	}
*/
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = dest_ip;
	remote_addr.sin_port = htons(dest_port);
	int connecting_v = -1;
	if ( (connecting_v = connect(sock, (struct sockaddr*) &remote_addr, sizeof(remote_addr)))
			!= 0) {
		perror("connect failed\n");
		exit(1);
	}
	printf("Connecting ");
	while(connecting_v!=0)
		printf(".");
	printf(" Connected!\n");

	return 0;
}

void tcp_send_syn(int sock, int seq, __u32 src_ip, __u32 dest_ip, int src_port,
		int dest_port) {
	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);

	//modify tcpsyn_src_ip, increment 1.
	//next_ip_addr(tcpsyn_src_ip, 1);

	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);

	tcp_h = tcp_set_seq(tcp_h, seq);
	//tcp_h = tcp_set_ack_seq(tcp_h,35623);

	tcp_h = tcp_set_syn_flag(tcp_h);

	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
	char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

	send_packet(sock, ipv4_h, packet, dest_port);

	free(packet);
}
void tcp_send_ack(int sock, int seq, int syn_ack_seq, __u32 src_ip,
		__u32 dest_ip, int src_port, int dest_port) {
	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	tcp_h = tcp_set_seq(tcp_h, seq + 1);

	tcp_h = tcp_set_ack_seq(tcp_h, syn_ack_seq + 1);

	tcp_h = tcp_set_ack_flag(tcp_h);

	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
	char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

	send_packet(sock, ipv4_h, packet, dest_port);
	free(packet);
}

int tcp_make_pseudo_connection(__u32 src_ip, __u32 dest_ip, int src_port,
		int dest_port) {
	int sock = make_socket(IPPROTO_TCP);

	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);

	//modify tcpsyn_src_ip, increment 1.
	//next_ip_addr(tcpsyn_src_ip, 1);

	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	int seq = (int) (rand() % __UINT_MAXIMUM__);
	tcp_h = tcp_set_seq(tcp_h, seq);
	//tcp_h = tcp_set_ack_seq(tcp_h,35623);

	tcp_h = tcp_set_syn_flag(tcp_h);

	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
	char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

	send_packet(sock, ipv4_h, packet, dest_port);

	free(packet);

	struct timespec specs = { 0 };
	specs.tv_nsec = 500 * 1000000;

	nanosleep(&specs, NULL);

	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	tcp_h = prepare_empty_tcp();
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	tcp_h = tcp_set_seq(tcp_h, seq + 1);

	int syn_ack_seq = (int) (rand() % __UINT_MAXIMUM__);

	tcp_h = tcp_set_ack_seq(tcp_h, syn_ack_seq + 1);

	tcp_h = tcp_set_ack_flag(tcp_h);

	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(tcp_h));
	packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));

	send_packet(sock, ipv4_h, packet, dest_port);
	free(packet);

}
