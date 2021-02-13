#ifndef UDP_FLOOD

#define UDP_FLOOD

#ifndef UDPHDR
#define UDPHDR
/**
	@struct udphdr
	@brief udp header
	@date 2021/02/13
	@author 김한수(cpm0722@gmail.com)
*/
struct udphdr {
	u_short src_port;   /// source port (0)
	u_short dest_port;  /// destination port
	u_short len;        /// data length
	u_short checksum;   /// checksum (0)
	char data[1024];    /// data (maximum length: 1024)
};
#endif

/**
	@fn void udp_flood_print_usage(void);
	@brief function: print udp usage
	@date 2021/02/13
	@author 김한수(cpm0722@gmail.com)
	@param void
	@return void
*/
void udp_flood_print_usage(void);
/**
	@fn void *generate_udp_request(void *);
	@brief function: udp request thread
	@date 2021/02/13
	@author 김한수(cpm0722@gmail.com)
	@param void * NULL
	@return void * data NULL
*/
void *generate_udp_request(void *);
/**
	@fn void *udp_time_check(void *);
	@brief function: udp time check thread
	@date 2021/02/13
	@author 김한수(cpm0722@gmail.com)
	@param void * NULL
	@return void * data NULL
*/
void *udp_time_check(void *);
/**
	@fn void *udp_flood_main(char *argv[]);
	@brief function: udp main function
	@date 2021/02/13
	@author 김한수(cpm0722@gmail.com)
	@param void
	@return argv char *[]: arguments
*/
void udp_flood_main(char *argv[]);

#endif
