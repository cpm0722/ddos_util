#ifndef HASH_DOS

#define HASH_DOS

/**
	@fn void hash_dos_print_usage(void);
	@brief function: print hash_dos usage
	@date 2021/03/15
	@param void
	@return void
*/
void hash_dos_print_usage(void);

/**
	@fn void *generate_hash_dos(void *);
	@brief function: hash_dos generate thread
	@date 2021/03/15
	@param void *data NULL
	@return void * NULL
*/
void *generate_hash_dos(void *data);

/**
	@fn void *hash_dos_check(void *);
	@brief function: hash_dos time check thread
	@date 2021/03/15
	@param void *data NULL
	@return void * NULL
*/
void *hash_dos_time_check(void *data);

/**
	@fn void *hash_dos_main(char *argv[]);
	@brief function: hash_dos main function
	@date 2021/03/15
	@param argv char *argv[]: arguments
	@return void
*/
void hash_dos_main(char *argv[]);

#endif // ifndef HASH_DOS
