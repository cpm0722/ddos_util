#include "../header.h"
#include "../base/make_ipv4.h"
#include "../base/receiver.h"

struct recv get_response(int socket_fd) {

	int one = 1;
	struct timeval tv;
	socklen_t len;
	int bytes;

	struct recv recvd;;


	unsigned char buffer[__MAX_RECV_MSG_LENGTH__];
	struct sockaddr_in cliaddr;
	struct iphdr *iph;

	int result=0;
	char source_add[50];
	char expected_source_add[50];
	len = sizeof(struct sockaddr_in);

	tv.tv_sec = 30;
	tv.tv_usec=0;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &one, sizeof(one));
	setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO,(char*)&tv,sizeof(tv));

	bytes = recvfrom(socket_fd,buffer, 1500, MSG_WAITALL,(struct sockaddr *)&cliaddr, &len);

	recvd.msg_length = bytes;

	memset(recvd.msg,0,bytes);
	memcpy(recvd.msg+0,buffer+0,bytes);

	return recvd;

}
void packet_dismantle(struct recv recvd,struct iphdr *ip, void *data)
{
	//memcpy(ip,recvd.msg,20);
	memcpy(data,recvd.msg+20,recvd.msg_length - 20);

}


void free_recv(struct recv recvd) {

}

