#ifndef GET_FLOOD

#define GET_FLOOD

#define __GET_REQUEST_MSG_SIZE__ 100

/**
	@fn void get_flood_print_usage(void);
	@brief function: print get_flood usage
	@date 2021/02/20
	@param void
	@return void
*/
void get_flood_print_usage(void);

/**
	@fn void *generate_get_flood(void *data);
	@brief function: get_flood generate thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *generate_get_flood(void *data);

/**
	@fn void *get_flood_time_check(void *data);
	@brief function: get_flood time check thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *get_flood_time_check(void *data);

/**
	@fn void *get_flood_main(void *data);
	@brief function: get_flood main function
	@date 2021/02/20
	@param char *argv[]: arguments
	@return void
*/
void get_flood_main(char *argv[]);

#endif // ifndef GET_FLOOD
