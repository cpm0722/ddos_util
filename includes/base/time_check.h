#ifndef TIME_CHECK

#define TIME_CHECK

#define NANO_PER_SEC 1000000000.0

/**
	@fn void time_check(pthread_cond_t *cond_p, struct timespec *before_time_p, struct timespec *now_time_p, unsigned long *produced_in_sec_p)
	@brief function: if elapsed time is more than 1 sec, update produced_in_sec and before_time, send condition signal to all waiting threads
	@date 2021/04/03
	@param pthread_cond_t *cond_p, struct timespec *before_time_p, struct timespec *now_time_p, unsigned long *produced_in_sec_p
	@return void
*/
void time_check(pthread_cond_t *cond_p,
								struct timespec *before_time_p,
								struct timespec *now_time_p,
								unsigned long *produced_in_sec_p);

#endif // ifndef TIME_CHECK
