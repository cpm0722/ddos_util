#include "header.h"

int scan(char *hostip, int port){
	// file descriptor for the socket
	int fd;
	// tcp header length
	int tcphdr_len = sizeof(struct tcphdr);
	// buffers containing raw packets
	char send_buffer[PACKET_LENGTH], recv_buffer[PACKET_LENGTH];
	// tcp header segment of raw packet
	struct tcphdr *tcp = (struct tcphdr *) send_buffer;
	// address of the destination entry point
	struct sockaddr_in addr;
	// size of received packet
	int recvd_size;
	int one = 1, result;

	// clearing whole of the packet
	memset(send_buffer, 0, PACKET_LENGTH);
	memset(recv_buffer, 0, PACKET_LENGTH);
	// creating raw socket
	fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0){
		die("failed to create socket");
	}
	// address family
	addr.sin_family = AF_INET;
	// destination port
	addr.sin_port = htons(port);
	// destination ip
	addr.sin_addr.s_addr = inet_addr(hostip);

	// the tcp structure
	// source port
	tcp->source = htons(12345);
	// destination port
	tcp->dest = addr.sin_port;
	tcp->seq = htonl(1);
	tcp->doff = 5;
	tcp->syn = 1;
	tcp->window = htons(65535);

	if(sendto(fd, send_buffer, tcphdr_len, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		die("sendto failed");
	}

	recvd_size = recvfrom(fd, recv_buffer, PACKET_LENGTH, 0, 0, 0);
	if(recvd_size < 0){ // error occurred
		close(fd);
		die("failed to receive packet");
	}
	if(recvd_size == 0){ // timed out
		close(fd);
		return ST_FILTERED;
	}
	result = process_packet(recv_buffer, recvd_size);
	if(result == PCK_UNKNOWN){
		close(fd);
		return ST_UNKNOWN;
	}
	if(result == PCK_RST){
		close(fd);
		return ST_CLOSED;
	}

	close(fd);
	return ST_OPEN;
}

int main(void)
{
	return 0;
}
