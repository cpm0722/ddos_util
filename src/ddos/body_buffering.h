#ifndef BODY_BUFFERING

#define BODY_BUFFERING

#define __BODY_BUFFERING_REQUEST_MSG_SIZE__ 100

/**
	@fn void body_buffering_print_usage(void);
	@brief function: print body_buffering usage
	@date 2021/02/20
	@param void
	@return void
*/
void body_buffering_print_usage(void);

/**
	@fn void *generate_body_buffering(void *data);
	@brief function: body_buffering generate thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void* generate_body_buffering(void *data);

/**
	@fn void *body_buffering_time_check(void *data);
	@brief function: body_buffering time check thread
	@date 2021/02/20
	@param void *data NULL
	@return void * NULL
*/
void* body_buffering_time_check(void *data);

/**
	@fn void *body(void *data);
	@brief function: body_buffering main function
	@date 2021/02/20
	@param char *argv[]: arguments
	@return void
*/
void body_buffering_main(char *argv[]);

#endif // ifndef BODY_BUFFERING
