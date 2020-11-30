struct udphdr {
	u_short src_port;
	u_short dest_port;
	u_short len;
	u_short check;
	char data[1024];
};

void udp_flood_print_usage(void);
void* generate_udp_request(void *);
void *udp_time_check(void *);
void udp_flood_run(char *[]);
