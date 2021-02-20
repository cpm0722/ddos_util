#ifndef HEADER_BUFFERING

#define HEADER_BUFFERING

#define __HEADER_BUFFERING_REQUEST_MSG_SIZE__ 100

/**
	@fn void header_buffering_print_usage(void);
	@brief function: print header_buffering usage
	@date 2021/02/20
	@param void
	@return void
*/
void header_buffering_print_usage();

/**
	@fn void *generate_header_buffering(void *data);
	@brief function: header_buffering generate thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *generate_header_buffering(void *data);

/**
	@fn void *header_buffering_time_check(void *data);
	@brief function: header_buffering time check thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void *header_buffering_time_check(void *data);

/**
	@fn void *header_buffering_main(void *data);
	@brief function: header_buffering main function
	@date 2021/02/20
	@param char *argv[]: arguments
	@return void
*/
void header_buffering_main(char *argv[]);

#endif // ifndef HEADER_BUFFERING
