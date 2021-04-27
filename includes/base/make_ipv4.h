#ifndef MAKE_IPV4

#define MAKE_IPV4


void prepare_pid(int *pid_list, int num);

/*!
 * @brief
 * get empty iphdr
 * @date		2021/04/03
 * @return		struct iphdr
 * @retval		empty iphdr	
 */
struct iphdr prepare_empty_ipv4(void);

/*!
 * @brief
 * set iphdr's protocol
 * @date		2021/04/03
 * @param[in]	ip_head		struct iphdr for update
 * @param[in]	protocol	protocol value for update
 * @return		struct iphdr
 * @reval		updated iphdr
 */
struct iphdr ipv4_set_protocol(struct iphdr ip_head, __u8 protocol);

/*!
 * @brief
 * set iphdr's dest address
 * @date		2021/04/03
 * @param[in]	ip_head		struct iphdr for update
 * @param[in]	daddr		destination address value for update
 * @return		struct iphdr
 * @reval		updated iphdr
 */
struct iphdr ipv4_set_daddr(struct iphdr ip_head, __u32 daddr);

/*!
 * @brief
 * set iphdr's src address
 * @date		2021/04/03
 * @param[in]	ip_head		struct iphdr for update
 * @param[in]	saddr		source address value for update
 * @return		struct iphdr
 * @reval		updated iphdr
 */
struct iphdr ipv4_set_saddr(struct iphdr ip_head, __u32 saddr);

/*!
 * @brief
 * update iphdr's size(add data_size)
 * @date		2021/04/03
 * @param[in]	ip_head		struct iphdr for update
 * @param[in]	data_size	data size value for update
 * @return		struct iphdr
 * @reval		updated iphdr
 */
struct iphdr ipv4_add_size(struct iphdr ip_head, __u32 data_size);

/*!
 * @brief
 * create new packet using malloc(). the packet is assemble of iphdr and data.
 * @date		2021/04/03
 * @param[in]	ip_head		struct iphdr for update
 * @param[in]	data		data size value for update
 * @param[in]	data_size	data size value for update
 * @return		char *
 * @reval		mallocated packet (assemble of iphdr and data)
 */
char *packet_assemble(struct iphdr ip_head, void *data, __u32 data_size);

/*!
 * @brief
 * create ipv4 raw socket
 * @date		2021/04/03
 * @param[in]	protocol	protocol for raw socket
 * @return		int	
 * @reval		socket descriptor
 */
int make_socket(int protocol);

/*!
 * @brief
 * send packet to ip_head's dest address + port
 * @date		2021/04/03
 * @param[in]	sock		socket descriptor for send
 * @param[in]	ip_head		destination ip header for send
 * @param[in]	packet		packet data for send
 * @param[in]	port		destination port number for send
 * @return		void
 */
void send_packet(int sock, struct iphdr ip_head, char *packet, int port);

/*!
 * @brief
 * iphdr checksum value calculate
 * @date		2021/04/03
 * @param[in]	ptr			iphdr's address for calcuate checksum
 * @param[in]	nbytes		sizeof(iphdr)
 * @return		__u16
 * @retval		checksum value
 */
__u16 in_cksum(unsigned short *ptr, int nbytes);

#endif // ifndef MAKE_IPV4
