#ifndef INCLUDES_BASE_MAKE_IPV4_H_

#define INCLUDES_BASE_MAKE_IPV4_H_


void PreparePid(int *pid_list, int num);

/**
  @fn struct iphdr PrepareEmptyIphdr(void);
  @brief function: get empty iphdr
  @date 2021/04/03
  @param void
  @return struct iphdr
*/
struct iphdr PrepareEmptyIphdr(void);

/**
  @fn struct iphdr IphdrSetProtocol(struct iphdr ip_head, unsigned char protocol)
  @brief function: set iphdr's protocol
  @date 2021/04/03
  @param struct iphdr ip_head, unsigned char protocol: protocol value
  @return struct iphdr
*/
struct iphdr IphdrSetProtocol(struct iphdr ip_head, unsigned char protocol);

/**
  @fn struct iphdr IphdrSetDestAddr(struct iphdr ip_head, uint32_t daddr)
  @brief function: set iphdr's dest address
  @date 2021/04/03
  @param struct iphdr ip_head, uint32_t daddr: dest address value
  @return struct iphdr
*/
struct iphdr IphdrSetDestAddr(struct iphdr ip_head, uint32_t daddr);

/**
  @fn struct iphdr IphdrSetSrcAddr(struct iphdr ip_head, uint32_t saddr)
  @brief function: set iphdr's src address
  @date 2021/04/03
  @param struct iphdr ip_head, uint32_t saddr: src address value
  @return struct iphdr
*/
struct iphdr IphdrSetSrcAddr(struct iphdr ip_head, uint32_t saddr);

/**
  @fn struct iphdr IphdrAddSize(struct iphdr ip_head, uint32_t data_size)
  @brief function: update iphdr's size(add data_size)
  @date 2021/04/03
  @param struct iphdr ip_head, uint32_t data_size
  @return struct iphdr
*/
struct iphdr IphdrAddSize(struct iphdr ip_head, uint32_t data_size);

/**
  @fn char *AssembleIphdrWithData(struct iphdr ip_head, void *data, uint32_t data_size)
  @brief function: assemble iphdr and data
  @date 2021/04/03
  @param struct iphdr ip_head, void *data, uint32_t data_size
  @return struct iphdr
*/
char *AssembleIphdrWithData(struct iphdr ip_head, void *data, uint32_t data_size);

/**
  @fn int MakeRawSocket(int protocol)
  @brief function: make ipv4 raw socket
  @date 2021/04/03
  @param int protocol
  @return int: socket
*/
int MakeRawSocket(int protocol);

/**
  @fn int SendPacket(int sock, struct iphdr ip_head, char *packet, int port)
  @brief function: send packet to ip_head's dest address + port
  @date 2021/04/03
  @param int sock, struct iphdr ip_head: dest's iphdr, char *packet: packet for send, int port: dest port
  @return void
*/
void SendPacket(int sock, struct iphdr ip_head, char *packet, int port);

/**
  @fn uint16_t IphdrGetChecksum(unsigned short *ptr, int nbytes)
  @brief function: iphdr checksum value calculate
  @date 2021/04/03
  @param unsigned short *ptr: iphdr ptr, int nbytes: sizeof(iphdr)
  @return uint16_t: checksum value
*/
uint16_t IphdrGetChecksum(uint16_t *ptr, int nbytes);

#endif  // INCLUDES_BASE_MAKE_IPV4_H_
