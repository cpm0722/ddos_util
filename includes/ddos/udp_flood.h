#ifndef INCLUDES_DDOS_UDP_FLOOD_H_

#define INCLUDES_DDOS_UDP_FLOOD_H_

#ifndef UDPHDR
#define UDPHDR
#define UDP_DATA_MAX 1024
/**
 * @brief udp header
 * @date 2021/02/13
 */
struct udphdr
{
  u_short src_port;          /** source port (0) */
  u_short dest_port;         /** destination port */
  u_short len;               /** data length */
  u_short checksum;          /** checksum (0) */
  char *data;  /** data (maximum length: 1024) */
};
#endif  // ifndef UDPHDR

/*!
 * @brief
 * print udp flooding usage
 * @date    2021/02/13
 * @return    void
 */
void UdpFloodPrintUsage(void);

/*!
 * @brief
 * udp flooding generate thread
 * @date    2021/02/13
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *GenerateUdpFlood(void *data);

/*!
 * @brief
 * udp flooding time check thread
 * @date    2021/02/13
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *UdpFloodTimeCheck(void *data);

/*!
 * @brief
 * udp flooding main function
 * @date    2021/02/13
 * @param[in]  argv  arguments
 * @return    void
 */
void UdpFloodMain(char *argv[]);

#endif  // INCLUDES_DDOS_UDP_FLOOD_H_
