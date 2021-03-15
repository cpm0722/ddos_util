#ifndef RESPONSE_BUFFERING

#define RESPONSE_BUFFERING

/**
	@fn void response_buffering_print_usage(void);
	@brief function: print response_buffering usage
	@date 2021/03/15
	@param void
	@return void
*/
void response_buffering_print_usage(void);

/**
	@fn void *generate_response_buffering(void *);
	@brief function: response_buffering generate thread
	@date 2021/03/15
	@param void *data NULL
	@return void * NULL
*/
void *generate_response_buffering(void *data);

/**
	@fn void *response_buffering_time_check(void *);
	@brief function: response_buffering time check thread
	@date 2021/03/15
	@param void *data NULL
	@return void * NULL
*/
void *response_buffering_time_check(void *data);

/**
	@fn void *response_buffering_main(char *argv[]);
	@brief function: response_buffering main function
	@date 2021/03/15
	@param argv char *argv[]: arguments
	@return void
*/
void response_buffering_main(char *argv[]);

#endif // ifndef RESPONSE_BUFFERING
