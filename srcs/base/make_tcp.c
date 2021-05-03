#include "header.h"
#include "base/make_ipv4.h"
#include "base/make_tcp.h"

extern int g_recv_flag;

struct tcphdr PrepareEmptyTcphdr(void)
{
	struct tcphdr tcp_head;
	memset(&tcp_head, 0, sizeof(struct tcphdr));
	tcp_head.source = 0;   // port
	tcp_head.dest = 0;     // port
	tcp_head.seq = 0;      // fill in later;
	tcp_head.ack_seq = 0;  // fill in later
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

struct tcphdr TcphdrSetSrcPort(struct tcphdr tcph, __u16 src_port)
{
	tcph.source = htons(src_port);
	return tcph;
}

struct tcphdr TcphdrSetDestPort(struct tcphdr tcph, __u16 dest_port)
{
	tcph.dest = htons(dest_port);
	return tcph;
}

struct tcphdr TcphdrSetSeq(struct tcphdr tcph, __u32 seq)
{
	tcph.seq = htonl(seq);
	return tcph;
}

struct tcphdr TcphdrSetAckSeq(struct tcphdr tcph, __u32 ack_seq)
{
	tcph.ack_seq = htonl(ack_seq);
	return tcph;
}

struct tcphdr TcphdrSetSynFlag(struct tcphdr tcph)
{
	tcph.syn = 1;
	return tcph;
}

struct tcphdr TcphdrSetAckFlag(struct tcphdr tcph)
{
	tcph.ack = 1;
	return tcph;
}

struct tcphdr TcphdrSetPshFlag(struct tcphdr tcph)
{
	tcph.psh = 1;
	return tcph;
}

struct tcphdr TcphdrSetWindowSize(struct tcphdr tcph, __u16 window_size)
{
	tcph.window = htons(window_size);
	return tcph;
}

struct tcphdr TcphdrGetChecksum(struct iphdr ipv4h,
															 struct tcphdr tcph,
															 void *data,
															 int datasize)
{
	struct PseudoTcphdr psh;
	memset(&psh, 0, sizeof(struct PseudoTcphdr));
	psh.source_address = ipv4h.saddr;
	psh.dest_address = ipv4h.daddr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + datasize);
	int psize = sizeof(struct PseudoTcphdr) +
							sizeof(struct tcphdr) +
							datasize;
	char *assembled = (char *) malloc(psize);
	memcpy(assembled, (char *) &psh, sizeof(struct PseudoTcphdr));
	memcpy(assembled + sizeof(struct PseudoTcphdr),
				 &tcph,
				 sizeof(struct tcphdr));
	if (data != NULL && datasize != 0)
		memcpy(assembled +
					 sizeof(struct PseudoTcphdr) +
					 sizeof(struct tcphdr), data, datasize);
	tcph.check = IphdrGetChecksum((__u16*) assembled, psize);
	return tcph;
}

// 3way handshake completed socket, returns socket;
int MakeTcpConnection(__u32 src_ip,
												__u32 dest_ip,
												int *src_port_copy,
												int dest_port,
												int *seq_copy,
												int *ack_copy,
												__u16 window_size)
{
	/*	CODE OPTIMIZATION
	 *  1. PREPARE ipv4 or tcp headers before, so that less calculation in loop
	 *  2. try using register keyword
	 *  3. OpenMP or use FORK
	 */

	// in make_ipv4.c -> make tcp socket via raw socket.
	int sock = MakeRawSocket(IPPROTO_TCP);
	struct iphdr ipv4_h;

	ipv4_h = PrepareEmptyIphdr();

	ipv4_h.protocol = IPPROTO_TCP;
	ipv4_h.saddr = src_ip;
	ipv4_h.daddr = dest_ip;
	ipv4_h.tot_len += sizeof(struct tcphdr);
	ipv4_h.check = IphdrGetChecksum((__u16 *) &ipv4_h,sizeof(struct iphdr) + sizeof(struct tcphdr));

	// make tcp header.
	struct tcphdr tcp_h;
	tcp_h = PrepareEmptyTcphdr();
	if (window_size != 0)
		tcp_h.window = htons(window_size);

	// set src port number random
	int src_port = rand() % 63535 + 1500;
	*(src_port_copy) = src_port;
	tcp_h.source = htons(src_port);
	tcp_h.dest = htons(dest_port);
	int seq = rand() % 10000000;
	tcp_h.seq = htonl(seq);
	seq++;
	// ***For kSynFlooding TCP request, ACK seq should not be provided
	// tcp_h = TcphdrSetAckSeq(tcp_h,35623);
	tcp_h.syn=1;
	tcp_h = TcphdrGetChecksum(ipv4_h, tcp_h, NULL, 0);

	char *packet = AssembleIphdrWithData(ipv4_h, &tcp_h, sizeof(tcp_h));
	SendPacket(sock, ipv4_h, packet, dest_port);
	free(packet);
	__u64 req_seq;

	if(g_recv_flag==1)
	{
	unsigned char buffer[1000];
	int recv_size = 0;

	recv_size = recv(sock, buffer, 1000, 0);

	//printf("recvd : %d\n", recv_size);
	int i;
	//for (i = 20; i < recv_size; i++)
	//	printf("%x ", buffer[i]);
	//printf("\n");


	memcpy(&req_seq, buffer + 24, 4);

	req_seq = ntohl(req_seq);
	//printf("req_seq : %lu", req_seq);

	//printf("\n\n");

	}
	else
		req_seq=rand() % 10000000;

	ipv4_h = PrepareEmptyIphdr();
	ipv4_h.protocol = IPPROTO_TCP;
	ipv4_h.saddr = src_ip;
	ipv4_h.daddr = dest_ip;
	ipv4_h.tot_len += sizeof(struct tcphdr);
	ipv4_h.check = IphdrGetChecksum((__u16 *) &ipv4_h,sizeof(struct iphdr) + sizeof(struct tcphdr));
	// make tcp header.

	tcp_h = PrepareEmptyTcphdr();
	// set src port number random
	if (window_size != 0)
		tcp_h.window = htons(window_size);

	tcp_h.source = htons(src_port);
	tcp_h.dest = htons(dest_port);

	tcp_h.seq = htonl(seq);
	tcp_h.ack_seq = htonl(req_seq+1);

	// ***For kSynFlooding TCP request, ACK seq should not be provided
	// tcp_h = TcphdrSetAckSeq(tcp_h,35623);
	tcp_h.ack = 1;
	tcp_h = TcphdrGetChecksum(ipv4_h, tcp_h, NULL, 0);

	packet = AssembleIphdrWithData(ipv4_h, &tcp_h, sizeof(tcp_h));
	SendPacket(sock, ipv4_h, packet, dest_port);
	free(packet);

	*(seq_copy) = seq;
	*(ack_copy) = req_seq + 1;


	return sock;
}

void TcpSocketSendData(int sock,
													__u32 src_ip,
													__u32 dest_ip,
													int src_port,
													int dest_port,
													char *data,
													int data_size,
													int seq,
													int ack,
													__u16 window_size)
{
	struct iphdr ipv4_h;
	ipv4_h = PrepareEmptyIphdr();
	ipv4_h.protocol = IPPROTO_TCP;
	ipv4_h.saddr = src_ip;
	ipv4_h.daddr = dest_ip;


	// make tcp header.
	struct tcphdr tcp_h;

	tcp_h = PrepareEmptyTcphdr();
		if (window_size != 0)
			tcp_h.window = htons(window_size);

		// set src port number random

		tcp_h.source = htons(src_port);
		tcp_h.dest = htons(dest_port);
		tcp_h.seq = htonl(seq);
		tcp_h.ack_seq = htonl(ack);
		seq++;
		// ***For kSynFlooding TCP request, ACK seq should not be provided
		// tcp_h = TcphdrSetAckSeq(tcp_h,35623);
		tcp_h.psh=1;
		tcp_h.ack=1;




	ipv4_h.tot_len += sizeof(struct tcphdr) + data_size;
	ipv4_h.check = IphdrGetChecksum((__u16 *) &ipv4_h,
			sizeof(struct iphdr) + sizeof(struct tcphdr)+data_size);

	tcp_h = TcphdrGetChecksum(ipv4_h, tcp_h, data, data_size);

	char *tcp_with_data = malloc(sizeof(struct tcphdr) + data_size);
	memcpy(tcp_with_data, &tcp_h, sizeof(tcp_h));
	memcpy(tcp_with_data + sizeof(tcp_h), data, data_size);


	char *packet = AssembleIphdrWithData(ipv4_h, tcp_with_data,
			sizeof(struct tcphdr) + data_size);
	free(tcp_with_data);

	SendPacket(sock, ipv4_h, packet, dest_port);
	free(packet);
	return;
}

void TckSocketSendDataWithoutAck(int sock,
																 __u32 src_ip,
																 __u32 dest_ip,
																 int src_port,
																 int dest_port,
																 char *data,
																 int data_size,
																 int seq,
																 int ack,
																 __u16 window_size)
{
	struct iphdr ipv4_h;
	ipv4_h = PrepareEmptyIphdr();
	ipv4_h.protocol = IPPROTO_TCP;
	ipv4_h.saddr = src_ip;
	ipv4_h.daddr = dest_ip;

	// make tcp header.
	struct tcphdr tcp_h;
	tcp_h = PrepareEmptyTcphdr();
	if (window_size != 0)
		tcp_h.window = htons(window_size);
	// set src port number random
	tcp_h.source = htons(src_port);
	tcp_h.dest = htons(dest_port);
	tcp_h.seq = htonl(seq);
	tcp_h.ack_seq = htonl(ack);

	// ***For kSynFlooding TCP request, ACK seq should not be provided
	// tcp_h = TcphdrSetAckSeq(tcp_h,35623);
	//tcp_h.ack = 1;
	tcp_h.psh = 1;
	// tcp_h = TcphdrSetAckFlag(tcp_h);

	ipv4_h.tot_len += sizeof(struct tcphdr) + data_size;
	ipv4_h.check = IphdrGetChecksum((__u16 *) &ipv4_h,
			sizeof(struct iphdr) + sizeof(struct tcphdr)+data_size);
	tcp_h = TcphdrGetChecksum(ipv4_h, tcp_h, data, data_size);

	char *tcp_with_data = malloc(sizeof(struct tcphdr) + data_size);
	memcpy(tcp_with_data, &tcp_h, sizeof(tcp_h));
	memcpy(tcp_with_data + sizeof(tcp_h), data, data_size);

	char *packet = AssembleIphdrWithData(ipv4_h, tcp_with_data,
			sizeof(struct tcphdr) + data_size);
	free(tcp_with_data);

	SendPacket(sock, ipv4_h, packet, dest_port);
	free(packet);
	return;
}
