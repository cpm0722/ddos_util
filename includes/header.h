#ifndef kHeadBufferingER_H

#define kHeadBufferingER_H

/// maximum value of unsigned integer
#define __UINT_MAXIMUM__ (1L << (31)) - 1
/// ipv4 string's length
#define IPV4_STRLEN 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "base/asm_types.h"

/**
 * @brief attack types
 * @date 2021/04/12
 */
typedef enum 
{
	kNoneType, kSynFlooding, kUdpFlooding, kIcmpFlooding, kConnectionFlooding, kGetFlooding, kHeadBuffering, kBodyBuffering, kResponseBuffering, kHashDos
} AttackType;

/**
 * @brief boolean
 * @date 2021/04/12
 */
typedef enum
{
	false, true
} bool;

/**
 * @brief ip, port, subnet masking values
 * @date 2021/04/12
 */
typedef struct
{
	unsigned char src[IPV4_STRLEN];   /** source ipv4 string, format: XXX.XXX.XXX.XXX */
	unsigned char dest[IPV4_STRLEN];  /** destination ipv4 string, format: XXX.XXX.XXX.XXX */
	__u32 src_mask;                   /** source ipv4 mask, range: 0~32, default: 32 */
	__u32 dest_mask;                  /** destination ipv4 mask, range: 0~32, default: 32 */
	__u32 port_start;                 /** destination port start, range: 0~65535 */
	__u32 port_end;                   /** destination port end, range: 0~65535 */
} InputArguments;

/**
 * @brief now values(ip, port) in subnet masking 
 * @date 2021/04/12
 */
typedef struct
{
	unsigned char src[IPV4_STRLEN];   /** now source ipv4 string in subnet masking, format: XXX.XXX.XXX.XXX */
	unsigned char dest[IPV4_STRLEN];  /** now destination ipv4 string in subnet masking, format: XXX.XXX.XXX.XXX */
	__u32 port;                       /** now destination port in subnet masking, range: 0~65535 */
} MaskingArguments;

#endif // ifndef kHeadBufferingER_H
