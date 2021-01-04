
#define __MAX_RECV_MSG_LENGTH__ 4096

struct recv
{
	int socket;
	unsigned char msg[__MAX_RECV_MSG_LENGTH__];
	__u32 msg_length;

	__u32 seq_num;
	__u32 ack_seq_num;
};

struct recv get_response(int socket);
void packet_dismantle(struct recv recvd,struct iphdr *ip, void *data);
void free_recv(struct recv recvd);
