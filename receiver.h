
#define __MAX_RECV_MSG_LENGTH__ 4096

struct recv
{
	int socket;
	unsigned char *msg;
	__u32 msg_length;

};

struct recv get_response(int socket);
void packet_dismantle(struct recv recvd,struct iphdr *ip, void *data);
void free_recv(struct recv recvd);
