#ifndef UDP_FLOOD

#define UDP_FLOOD

#ifndef UDPHDR
#define UDPHDR
struct udphdr {
	u_short src_port;
	u_short dest_port;
	u_short len;
	u_short check;
	char data[1024];
};
#endif

void udp_flood_print_usage(void);
void* generate_udp_request(void *);
void *udp_time_check(void *);
void udp_flood_run(char *[]);

#endif
