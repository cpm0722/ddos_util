#ifndef SYN_FLOOD

#define SYN_FLOOD

/**
	@fn void syn_flood_print_usage(void);
	@brief function: print syn_flooding usage
	@date 2021/03/08
	@param void
	@return void
*/
void syn_flood_print_usage(void);

/**
	@fn void *generate_syn_flood(void *);
	@brief function: syn_flooding generate thread
	@date 2021/03/18
	@param void *data NULL
	@return void * NULL
*/
void *generate_syn_flood(void *data);

/**
	@fn void *syn_time_check(void *);
	@brief function: syn_flooding time check thread
	@date 2021/03/08
	@param void *data NULL
	@return void * NULL
*/
void *syn_flood_time_check(void *data);

/**
	@fn void *syn_flood_main(char *argv[]);
	@brief function: syn_flooding main function
	@date 2021/03/08
	@param argv char *argv[]: arguments
	@return void
*/
void syn_flood_main(char *argv[]);

#endif // ifndef SYN_FLOOD
