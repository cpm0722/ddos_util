#ifndef HEADER_INCLUDED

#define HEADER_INCLUDED


#define __UINT_MAXIMUM__ (1L << (31)) - 1
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

typedef enum 
{
	NONE, SYN, UDP, ICMP, CONN, GET, HEAD, BODY, RESP, HASH, REF
} attack_type;

typedef enum
{
	false, true
} bool;

typedef struct
{
	unsigned char src[IPV4_STRLEN];
	unsigned char dest[IPV4_STRLEN];
	__u32 src_mask;
	__u32 dest_mask;
	__u32 port_start;
	__u32 port_end;
} InputArguments;

typedef struct
{
	unsigned char src[IPV4_STRLEN];
	unsigned char dest[IPV4_STRLEN];
	__u32 port;
} MaskingArguments;

#endif
