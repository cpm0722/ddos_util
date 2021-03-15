#ifndef CONN_FLOOD

#define CONN_FLOOD

/**
	@fn void conn_flood_print_usage(void);
	@brief function: print conn_flooding usage
	@date 2021/03/08
	@param void
	@return void
*/
void conn_flood_print_usage(void);

/**
	@fn void *generate_conn_request(void *);
	@brief function: conn_flooding generate thread
	@date 2021/03/08
	@param void *data NULL
	@return void * NULL
*/
void *generate_conn_flood(void *data);

/**
	@fn void *conn_time_check(void *);
	@brief function: conn_flooding time check thread
	@date 2021/03/08
	@param void *data NULL
	@return void * NULL
*/
void *conn_flood_time_check(void *data);

/**
	@fn void *conn_flood_main(char *argv[]);
	@brief function: conn_flooding main function
	@date 2021/03/08
	@param argv char *argv[]: arguments
	@return void
*/
void conn_flood_main(char *argv[]);

#endif
