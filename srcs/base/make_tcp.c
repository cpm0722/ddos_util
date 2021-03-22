#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"

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

struct tcphdr tcp_set_psh_flag(struct tcphdr tcph) {
	tcph.psh = 1;
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

//3way handshake completed socket, returns socket;
int tcp_make_connection(__u32 src_ip, __u32 dest_ip, int *src_port_copy,
		int dest_port, int *seq_copy, int *ack_copy) {

	//srand(time(NULL));

	int sock = make_socket(IPPROTO_TCP);
	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);
	ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct tcphdr));
	// make tcp header.
	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	// set src port number random
	int src_port = rand() % 63535 + 1500;
	*(src_port_copy) = src_port;
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	int seq = rand() % 10000000;
	tcp_h = tcp_set_seq(tcp_h, seq);
	seq++;
	// ***For SYN TCP request, ACK seq should not be provided
	// tcp_h = tcp_set_ack_seq(tcp_h,35623);
	tcp_h = tcp_set_syn_flag(tcp_h);
	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	char *packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));
	send_packet(sock, ipv4_h, packet, dest_port);
	free(packet);

	unsigned char buffer[1000];
	int recv_size = 0;

	recv_size = recv(sock, buffer, 1000, 0);

	printf("recvd : %d\n", recv_size);
	int i;
	for (i = 20; i < recv_size; i++)
		printf("%x ", buffer[i]);
	printf("\n");

	unsigned long req_seq;
	memcpy(&req_seq, buffer + 24, 4);

	req_seq = ntohl(req_seq);
	printf("req_seq : %lu", req_seq);

	printf("\n\n");

	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);
	ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct tcphdr));
	// make tcp header.

	tcp_h = prepare_empty_tcp();
	// set src port number random
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);

	tcp_h = tcp_set_seq(tcp_h, seq);
	tcp_h = tcp_set_ack_seq(tcp_h, req_seq + 1);

	// ***For SYN TCP request, ACK seq should not be provided
	// tcp_h = tcp_set_ack_seq(tcp_h,35623);
	tcp_h = tcp_set_ack_flag(tcp_h);
	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL, 0);

	packet = packet_assemble(ipv4_h, &tcp_h, sizeof(tcp_h));
	send_packet(sock, ipv4_h, packet, dest_port);
	free(packet);

	*(seq_copy) = seq;
	*(ack_copy) = req_seq + 1;

	return sock;

}
void tcp_socket_send_ack(int sock, __u32 src_ip, __u32 dest_ip, int src_port,
		int dest_port, int seq, int ack) {

	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	// make tcp header.
	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	// set src port number random
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	tcp_h = tcp_set_seq(tcp_h, seq);
	tcp_h = tcp_set_ack_seq(tcp_h, ack);

	// ***For SYN TCP request, ACK seq should not be provided
	// tcp_h = tcp_set_ack_seq(tcp_h,35623);
	tcp_h = tcp_set_psh_flag(tcp_h);
	tcp_h = tcp_set_ack_flag(tcp_h);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct tcphdr) );
	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, NULL,0);

	char *packet = packet_assemble(ipv4_h, &tcp_h,
			sizeof(struct tcphdr) );

	send_packet(sock, ipv4_h, packet, dest_port);
	free(packet);
}

void tcp_socket_send_data(int sock, __u32 src_ip, __u32 dest_ip, int src_port,
		int dest_port, char *data, int data_size, int seq, int ack) {

	struct iphdr ipv4_h;
	ipv4_h = prepare_empty_ipv4();
	ipv4_h = ipv4_set_protocol(ipv4_h, IPPROTO_TCP);
	ipv4_h = ipv4_set_saddr(ipv4_h, src_ip);
	ipv4_h = ipv4_set_daddr(ipv4_h, dest_ip);

	// make tcp header.
	struct tcphdr tcp_h;
	tcp_h = prepare_empty_tcp();
	// set src port number random
	tcp_h = tcp_set_source(tcp_h, src_port);
	tcp_h = tcp_set_dest(tcp_h, dest_port);
	tcp_h = tcp_set_seq(tcp_h, seq);
	tcp_h = tcp_set_ack_seq(tcp_h, ack);

	// ***For SYN TCP request, ACK seq should not be provided
	// tcp_h = tcp_set_ack_seq(tcp_h,35623);
	tcp_h = tcp_set_psh_flag(tcp_h);
	tcp_h = tcp_set_ack_flag(tcp_h);

	ipv4_h = ipv4_add_size(ipv4_h, sizeof(struct tcphdr) + data_size);
	tcp_h = tcp_get_checksum(ipv4_h, tcp_h, data, data_size);

	char *tcp_with_data = malloc(sizeof(struct tcphdr) + data_size);
	memcpy(tcp_with_data, &tcp_h, sizeof(tcp_h));
	memcpy(tcp_with_data + sizeof(tcp_h), data, data_size);

	char *packet = packet_assemble(ipv4_h, tcp_with_data,
			sizeof(struct tcphdr) + data_size);
	free(tcp_with_data);

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
