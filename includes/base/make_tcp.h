#ifndef MAKE_TCP

#define MAKE_TCP
#define __MINIMUM_RESPONSE_WAIT_TIME__ 0.1

#ifndef TCP_PSEUDO_HEADER
#define TCP_PSEUDO_HEADER

/**
 * @brief tcp pseudo header
 * @date 2021/04/03
 */
struct tcp_pseudo_header {
	__u32 source_address; /**  src ipv4 address(__u32) */
	__u32 dest_address;   /** dest ipv4 address(__u32) */
	__u8 placeholder;     /** placeholder (always 0) */
	__u8 protocol;        /** protocol (always IPPROTO_TCP) */
	__u16 tcp_length;     /** total length of packet (include data size) */
};
#endif // ifndef TCP_PSEUDO_HEADER

/*!
 * @brief
 * get empty tcphdr
 * @date		2021/04/03
 * @return		struct tcphdr	
 * @reval		empty tcphdr
 */
struct tcphdr prepare_empty_tcp(void);

/*!
 * @brief
 * set tcphdr's src port
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @param[in]	src_port	source port value for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_src_port(struct tcphdr tcph, __u16 src_port);

/*!
 * @brief
 * set tcphdr's dest port
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @param[in]	dest_port	destination port value for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_dest_port(struct tcphdr tcph, __u16 dest_port);

/*!
 * @brief
 * set tcphdr's sequence number
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @param[in]	seq			sequence number value for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_seq(struct tcphdr tcph, __u32 seq);

/*!
 * @brief
 * set tcphdr's ack sequence number
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @param[in]	ack_seq		ack sequence number value for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_ack_seq(struct tcphdr tcph, __u32 ack_seq);

/*!
 * @brief
 * set tcphdr's syn flag as true
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_syn_flag(struct tcphdr tcph);

/*!
 * @brief
 * set tcphdr's ack flag as 1
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_ack_flag(struct tcphdr tcph);

/*!
 * @brief
 * set tcphdr's psh flag as 1
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_psh_flag(struct tcphdr tcph);

/*!
 * @brief
 * set tcphdr's window size
 * @date		2021/04/03
 * @param[in]	tcph		struct tcphdr for update
 * @param[in]	window_size	window size value for update
 * @return		struct tcphdr	
 * @reval		updated tcphdr
 */
struct tcphdr tcp_set_window_size(struct tcphdr tcph, __u16 window_size);

/*!
 * @brief
 * update checksum value for tch header
 * @date		2021/04/03
 * @param[in]	ipv4h			ipv4 header
 * @param[in]	tcph			tcp header
 * @param[in]	data			data
 * @param[in]	datasize		data size	
 * @return		struct tcphdr	
 * @reval		updated tcp header
 */
struct tcphdr tcp_get_checksum(struct iphdr ipv4h,
                               struct tcphdr tcph,
                               void *data,
                               int datasize);

/*!
 * @brief
 * make tcp connection (3-way handshake)
 *  1. send SYN
 *  2. receive SYN+ACK
 *  3. send ACK
 * @date		2021/04/03
 * @param[in]	src_ip			source ipv4 address
 * @param[in]	dest_ip			destination ipv4 address
 * @param[out]	src_port_copy	random generated source port number
 * @param[in]	dest_port		destination port number
 * @param[out]	seq_copy		random generated sequence number
 * @param[out]	ack_copy		random generated request number + 1
 * @return		int
 * @reval		socket descriptor
 */
int tcp_make_connection(__u32 src_ip,
                        __u32 dest_ip,
                        int *src_port_copy,
                        int dest_port,
                        int *seq_copy,
                        int *ack_copy,
                        __u16 window_size);

/*!
 * @brief
 * after tcp-connection, send data with ACK requesting
 * @date		2021/04/03
 * @param[in]	src_ip			source ipv4 address
 * @param[in]	dest_ip			destination ipv4 address
 * @param[in]	src_port		source port number
 * @param[in]	dest_port		destination port number
 * @param[in]	data			data for send
 * @param[in]	data_size		sizeof(data)
 * @param[out]	seq				sequence number
 * @param[out]	ack				ack sequence number
 * @return		void
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

/*!
 * @brief
 * after tcp-connection, send data without ACK requesting
 * @date		2021/04/03
 * @param[in]	src_ip			source ipv4 address
 * @param[in]	dest_ip			destination ipv4 address
 * @param[in]	src_port		source port number
 * @param[in]	dest_port		destination port number
 * @param[in]	data			data for send
 * @param[in]	data_size		sizeof(data)
 * @param[out]	seq				sequence number
 * @param[out]	ack				ack sequence number
 * @return		void
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
