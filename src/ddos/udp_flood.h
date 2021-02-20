#ifndef UDP_FLOOD

#define UDP_FLOOD

#ifndef UDPHDR
#define UDPHDR
/**
	@struct udphdr
	@brief udp header
	@date 2021/02/13
*/
struct udphdr
{
	u_short src_port;   /// source port (0)
	u_short dest_port;  /// destination port
	u_short len;        /// data length
	u_short checksum;   /// checksum (0)
	char data[1024];    /// data (maximum length: 1024)
};
#endif // ifndef UDP_HDR

/**
	@fn void udp_flood_print_usage(void);
	@brief function: print udp_flooding usage
	@date 2021/02/13
	@param void
	@return void
*/
void udp_flood_print_usage(void);

/**
	@fn void *generate_udp_request(void *);
	@brief function: udp_flooding generate thread
	@date 2021/02/13
	@param void *data NULL
	@return void * NULL
*/
void *generate_udp_flood(void *data);

/**
	@fn void *udp_time_check(void *);
	@brief function: udp_flooding time check thread
	@date 2021/02/13
	@param void *data NULL
	@return void * NULL
*/
void *udp_flood_time_check(void *data);

/**
	@fn void *udp_flood_main(char *argv[]);
	@brief function: udp_flooding main function
	@date 2021/02/13
	@param argv char *argv[]: arguments
	@return void
*/
void udp_flood_main(char *argv[]);

#endif // ifndef UDP_FLOOD
