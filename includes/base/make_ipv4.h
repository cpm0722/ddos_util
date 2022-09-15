#ifndef INCLUDES_BASE_MAKE_IPV4_H_

#define INCLUDES_BASE_MAKE_IPV4_H_

/**
  @fn void PreparePid(int *pid_list ,int num);
  @brief function: get list of pid from forked processes
  @date 2021/04/03
  @param pid_list, num(process)
  @return void
*/
void PreparePid(int *pid_list, int num);

/**
  @fn void PrepareEmptyIphdr(struct iphdr *ip_head);
  @brief function: get default(empty) ip header struct
  @date 2021/04/03
  @param out: struct iphdr *ip_head
  @return void
*/
void PrepareEmptyIphdr(struct iphdr *ip_head);

/**
  @fn void IphdrSetProtocol(struct iphdr *ip_head, unsigned char protocol)
  @brief function: set iphdr's protocol
  @date 2021/04/03
  @param out: struct iphdr *ip_head, unsigned char protocol: protocol value
  @return void
*/
void IphdrSetProtocol(struct iphdr *ip_head, unsigned char protocol);

/**
  @fn void iphdr IphdrSetDestAddr(struct iphdr *ip_head, uint32_t daddr)
  @brief function: set iphdr's dest address
  @date 2021/04/03
  @param out: struct iphdr *ip_head, uint32_t daddr: dest address value
  @return void 
*/
void IphdrSetDestAddr(struct iphdr *ip_head, uint32_t daddr);

/**
  @fn void IphdrSetSrcAddr(struct iphdr *ip_head, uint32_t saddr)
  @brief function: set iphdr's src address
  @date 2021/04/03
  @param out: struct iphdr *ip_head, uint32_t saddr: src address value
  @return void
*/
void IphdrSetSrcAddr(struct iphdr *ip_head, uint32_t saddr);

/**
  @fn void IphdrAddSize(struct iphdr *ip_head, uint32_t data_size)
  @brief function: update iphdr's size(add data_size)
  @date 2021/04/03
  @param out: struct iphdr *ip_head, uint32_t data_size
  @return void
*/
void IphdrAddSize(struct iphdr *ip_head, uint32_t data_size);

/**
  @fn void AssembleIphdrWithData(char *packet, struct iphdr *ip_head, void *data, uint32_t data_size)
  @brief function: assemble iphdr and data
  @date 2021/04/03
  @param out: char *packet, in: struct iphdr *ip_head, void *data, uint32_t data_size
  @return void
*/
void AssembleIphdrWithData(char *packet, struct iphdr *ip_head, void *data, uint32_t data_size);

/**
  @fn int MakeRawSocket(int protocol)
  @brief function: make ipv4 raw socket
  @date 2021/04/03
  @param int protocol
  @return int: socket
*/
int MakeRawSocket(int protocol);

/**
  @fn void SendPacket(int sock, char *packet, uint32_t daddr, uint16_t tot_len, int port)
  @brief function: send packet to ip_head's dest address + port
  @date 2021/04/03
  @param int sock, char *packet: packet for send, uint32_t : dest ip, uint16_t tot_len : length of packet, int port: dest port
  @return void
*/
void SendPacket(int sock, char *packet, uint32_t daddr, uint16_t tot_len, int port);

/**
  @fn uint16_t IphdrGetChecksum(uint16_t* ip_head, int nbytes)
  @brief function: iphdr checksum value calculate
  @date 2021/04/03
  @param uint16_t *ip_head: iphdr, int nbytes: sizeof(iphdr)
  @return uint16_t: checksum value
*/
uint16_t IphdrGetChecksum(uint16_t* ip_head, int nbytes);

#endif  // INCLUDES_BASE_MAKE_IPV4_H_
