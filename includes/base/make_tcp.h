#ifndef INCLUDES_BASE_MAKE_TCP_H_

#define INCLUDES_BASE_MAKE_TCP_H_
#define __MINIMUM_kResponseBufferingONSE_WAIT_TIME__ 0.1

#ifndef TCP_PSEUDO_HEADER
#define TCP_PSEUDO_HEADER
/**
  @struct PseudoTcphdr
  @brief tcp pseudo header
  @date 2021/04/03
*/
struct PseudoTcphdr {
  uint32_t source_address;  ///  src ipv4 address(uint32_t)
  uint32_t dest_address;    /// dest ipv4 address(uint32_t)
  unsigned char placeholder;      /// placeholder (always 0)
  unsigned char protocol;         /// protocol (always IPPROTO_TCP)
  uint16_t tcp_length;      /// total length of packet (include data size)
};
#endif  // TCP_PSEUDO_HEADER

/**
  @fn struct tcphdr PrepareEmptyTcphdr(void)
  @brief function: get empty tcphdr
  @date 2021/04/03
  @param void
  @return struct tchpdr
*/
struct tcphdr PrepareEmptyTcphdr(void);

/**
  @fn struct tcphdr TcphdrSetSrcPort(struct tcphdr tcph, uint16_t src_port)
  @brief function: set tcphdr's src port
  @date 2021/04/03
  @param struct tcphdr tcph, uint16_t src_port
  @return struct tcphdr
*/
struct tcphdr TcphdrSetSrcPort(struct tcphdr tcph, uint16_t src_port);

/**
  @fn struct tcphdr TcphdrSetDestPort(struct tcphdr tcph, uint16_t src_port)
  @brief function: set tcphdr's src port
  @date 2021/04/03
  @param struct tcphdr tcph, uint16_t src_port
  @return struct tcphdr
*/
struct tcphdr TcphdrSetDestPort(struct tcphdr tcph, uint16_t dest_port);

/**
  @fn struct tcphdr TcphdrSetSeq(struct tcphdr tcph, uint16_t dest_port)
  @brief function: set tcphdr's dest port
  @date 2021/04/03
  @param struct tcphdr tcph, uint16_t dest_port
  @return struct tcphdr
*/
struct tcphdr TcphdrSetSeq(struct tcphdr tcph, uint32_t seq);

/**
  @fn struct tcphdr TcphdrSetAckSeq(struct tcphdr tcph, uint32_t ack_seq)
  @brief function: set tcphdr's ack sequence
  @date 2021/04/03
  @param struct tcphdr tcph, uint32_t ack sequence
  @return struct tcphdr
*/
struct tcphdr TcphdrSetAckSeq(struct tcphdr tcph, uint32_t ack_seq);

/**
  @fn struct tcphdr TcphdrSetSynFlag(struct tcphdr tcph)
  @brief function: set tcphdr's syn flag as 1
  @date 2021/04/03
  @param struct tcphdr tcph
  @return struct tcphdr
*/
struct tcphdr TcphdrSetSynFlag(struct tcphdr tcph);

/**
  @fn struct tcphdr TcphdrSetAckFlag(struct tcphdr tcph, uint16_t src_port)
  @brief function: set tcphdr's ack flag as 1
  @date 2021/04/03
  @param struct tcphdr tcph
  @return struct tcphdr
*/
struct tcphdr TcphdrSetAckFlag(struct tcphdr tcph);

/**
  @fn struct tcphdr TcphdrSetPshFlag(struct tcphdr tcph)
  @brief function: set tcphdr's psh flag as 1
  @date 2021/04/03
  @param struct tcphdr tcph
  @return struct tcphdr
*/
struct tcphdr TcphdrSetPshFlag(struct tcphdr tcph);

/**
  @fn struct tcphdr TcphdrSetWindowSize(struct tcphdr tcph, uint16_t window_size)
  @brief function: set tcphdr's window size
  @date 2021/04/03
  @param struct tcphdr tcph, uint16_t window_size
  @return struct tcphdr
*/
struct tcphdr TcphdrSetWindowSize(struct tcphdr tcph, uint16_t window_size);

/**
  @fn struct tcphdr TcphdrGetChecksum(struct iphdr ipv4h, struct tcphdr tcph, void *data, int datasize)
  @brief function: tcphdr checksum value calculate with iphdr, data
  @date 2021/04/03
  @param struct iphdr ipv4h, struct tcphdr tcph, void *data, int datasize
  @return struct tcphdr
*/
struct tcphdr TcphdrGetChecksum(struct iphdr ipv4h,
                               struct tcphdr tcph,
                               void *data,
                               int datasize);

/**
  @fn int MakeTcpConnection(uint32_t src_ip, uint32_t dest_ip, int *src_port_copy, int dest_port, int *seq_copy, int *ack_copy, uint16_t window_size)
  @brief function: make tcp connection (3-way handshake, 1. send kSynFlooding, 2. receive kSynFlooding+ACK, 3. send ACK)
  @date 2021/04/03
  @param uint32_t src_ip, uint32_t dest_ip, int *src_port_copy, int dest_port, int *seq_copy, int *ack_copy, uint16_t window_size
  @return int: socket
*/
void MakeTcpConnection(int sock,uint32_t src_ip,
                        uint32_t dest_ip,
                        int *src_port_copy,
                        int dest_port,
                        int *seq_copy,
                        int *ack_copy,
                         uint16_t window_size);

/**
  @fn void TcpSocketSendData(int sock, uint32_t src_ip, uint32_t dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, uint16_t window_size)
  @brief function: after tcp-connection, send data with ACK requesting
  @date 2021/04/03
  @param int sock, uint32_t src_ip, uint32_t dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, uint16_t window_size
  @return void
*/
void TcpSocketSendData(int sock,
                          uint32_t src_ip,
                          uint32_t dest_ip,
                          int src_port,
                          int dest_port,
                          char *data,
                          int data_size,
                          int seq,
                          int ack,
                           uint16_t window_size);

/**
  @fn void TckSocketSendDataWithoutAck(int sock, uint32_t src_ip, uint32_t dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, uint16_t window_size)
  @brief function: after tcp-connection, send data without ACK requesting
  @date 2021/04/03
  @param int sock, uint32_t src_ip, uint32_t dest_ip, int src_port, int dest_port, char *data, int data_size, int seq, int ack, uint16_t window_size
  @return void
*/
void TckSocketSendDataWithoutAck(int sock,
                                 uint32_t src_ip,
                                 uint32_t dest_ip,
                                 int src_port,
                                 int dest_port,
                                 char *data,
                                 int data_size,
                                 int seq,
                                 int ack,
                                 uint16_t window_size);

#endif  // INCLUDES_BASE_MAKE_TCP_H_
