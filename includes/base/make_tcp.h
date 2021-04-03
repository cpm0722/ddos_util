#ifndef MAKE_TCP

#define MAKE_TCP
#define __MINIMUM_RESPONSE_WAIT_TIME__ 0.1

#ifndef TCP_PSEUDO_HEADER
#define TCP_PSEUDO_HEADER
/**
	@struct tcp_pseudo_header
	@brief tcp pseudo header
	@date 2021/04/03
*/
struct tcp_pseudo_header {
	__u32 source_address; ///  src ipv4 address(__u32)
	__u32 dest_address;   /// dest ipv4 address(__u32)
	__u8 placeholder;     /// placeholder (always 0)
	__u8 protocol;        /// protocol (always IPPROTO_TCP)
	__u16 tcp_length;     /// total length of packet (include data size)
};
#endif // ifndef TCP_PSEUDO_HEADER

/**
	@fn struct tcphdr prepare_empty_tcp(void)
	@brief function: get empty tcphdr
	@date 2021/04/03
	@param void
	@return struct tchpdr
*/
struct tcphdr prepare_empty_tcp(void);

/**
	@fn struct tcphdr tcp_set_src_port(struct tcphdr tcph, __u16 src_port)
	@brief function: set tcphdr's src port
	@date 2021/04/03
	@param struct tcphdr tcph, __u16 src_port
	@return struct tcphdr
*/
struct tcphdr tcp_set_src_port(struct tcphdr tcph, __u16 src_port);

/**
	@fn struct tcphdr tcp_set_dest_port(struct tcphdr tcph, __u16 src_port)
	@brief function: set tcphdr's src port
	@date 2021/04/03
	@param struct tcphdr tcph, __u16 src_port
	@return struct tcphdr
*/
struct tcphdr tcp_set_dest_port(struct tcphdr tcph, __u16 dest_port);

/**
	@fn struct tcphdr tcp_set_seq(struct tcphdr tcph, __u16 dest_port)
	@brief function: set tcphdr's dest port
	@date 2021/04/03
	@param struct tcphdr tcph, __u16 dest_port
	@return struct tcphdr
*/
struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq);

/**
	@fn struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq)
	@brief function: set tcphdr's ack sequence
	@date 2021/04/03
	@param struct tcphdr tcph, __u32 ack sequence
	@return struct tcphdr
*/
struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq);

/**
	@fn struct tcphdr tcp_set_syn_flag(struct tcphdr tcph)
	@brief function: set tcphdr's syn flag as 1
	@date 2021/04/03
	@param struct tcphdr tcph
	@return struct tcphdr
*/
struct tcphdr tcp_set_syn_flag(struct tcphdr tcph);

/**
	@fn struct tcphdr tcp_set_ack_flag(struct tcphdr tcph, __u16 src_port)
	@brief function: set tcphdr's ack flag as 1
	@date 2021/04/03
	@param struct tcphdr tcph
	@return struct tcphdr
*/
struct tcphdr tcp_set_ack_flag(struct tcphdr tcph);

/**
	@fn struct tcphdr tcp_set_psh_flag(struct tcphdr tcph)
	@brief function: set tcphdr's psh flag as 1
	@date 2021/04/03
	@param struct tcphdr tcph
	@return struct tcphdr
*/
struct tcphdr tcp_set_psh_flag(struct tcphdr tcph);

/**
	@fn struct tcphdr tcp_set_window_size(struct tcphdr tcph, __u16 window_size)
	@brief function: set tcphdr's window size
	@date 2021/04/03
	@param struct tcphdr tcph, __u16 window_size
	@return struct tcphdr
*/
struct tcphdr tcp_set_window_size(struct tcphdr tcph, __u16 window_size);

/**
	@fn struct tcphdr tcp_get_checksum(struct iphdr ipv4h, struct tcphdr tcph, void *data, int datasize)
	@brief function: tcphdr checksum value calculate with iphdr, data
	@date 2021/04/03
	@param struct iphdr ipv4h, struct tcphdr tcph, void *data, int datasize
	@return struct tcphdr
*/
struct tcphdr tcp_get_checksum(struct iphdr ipv4h,
															 struct tcphdr tcph,
															 void *data,
															 int datasize);

/**
	@fn int tcp_make_connection(__u32 src_ip, __u32 dest_ip, int *src_port_copy, int dest_port, int *seq_copy, int *ack_copy, __u16 window_size)
	@brief function: make tcp connection (3-way handshake, 1. send SYN, 2. receive SYN+ACK, 3. send ACK)
	@date 2021/04/03
	@param __u32 src_ip, __u32 dest_ip, int *src_port_copy, int dest_port, int *seq_copy, int *ack_copy, __u16 window_size
	@return int: socket
*/
int tcp_make_connection(__u32 src_ip,
												__u32 dest_ip,
												int *src_port_copy,
												int dest_port,
												int *seq_copy,
												int *ack_copy,
												 __u16 window_size);

/**
	@fn void tcp_socket_send_data(int sock, __u32 src_ip, __u32 dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, __u16 window_size)
	@brief function: after tcp-connection, send data with ACK requesting
	@date 2021/04/03
	@param int sock, __u32 src_ip, __u32 dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, __u16 window_size
	@return void
*/
void tcp_socket_send_data(int sock,
													__u32 src_ip,
													__u32 dest_ip,
													int src_port,
													int dest_port,
													char *data,
													int data_size,
													int seq,
													int ack,
													 __u16 window_size);

/**
	@fn void tcp_socket_send_data_no_ack(int sock, __u32 src_ip, __u32 dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, __u16 window_size)
	@brief function: after tcp-connection, send data without ACK requesting
	@date 2021/04/03
	@param int sock, __u32 src_ip, __u32 dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, __u16 window_size
	@return void
*/
void tcp_socket_send_data_no_ack(int sock,
																 __u32 src_ip,
																 __u32 dest_ip,
																 int src_port,
																 int dest_port,
																 char *data,
																 int data_size,
																 int seq,
																 int ack,
																 __u16 window_size);

#endif // ifndef MAKE_TCP
