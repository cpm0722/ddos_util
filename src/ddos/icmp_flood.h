#ifndef ICMP_FLOOD

#define ICMP_FLOOD

/**
	@fn void icmp_flood_print_usage(void);
	@brief function: print icmp_flooding usage
	@date 2021/02/20
	@param void
	@return void
*/
void icmp_flood_print_usage(void);

/**
	@fn void *generate_icmp_request(void *);
	@brief function: icmp_flooding generate thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *generate_icmp_flood(void *data);

/**
	@fn void *icmp_time_check(void *);
	@brief function: icmp_flooding time check thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *icmp_flood_time_check(void *data);

/**
	@fn void *icmp_flood_main(char *argv[]);
	@brief function: icmp_flooding main function
	@date 2021/02/20
	@param argv char *argv[]: arguments
	@return void
*/
void icmp_flood_main(char *argv[]);

#endif // ifndef ICMP_FLOOD
