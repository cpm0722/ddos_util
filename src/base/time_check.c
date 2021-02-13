#include "../header.h"
#define NANO_PER_SEC 1000000000.0

void time_check(pthread_mutex_t *mutex_p,
							  pthread_cond_t *cond_p,
								struct timespec *before_time_p,
								struct timespec *now_time_p,
								unsigned long *produced_in_sec_p)
{
	// Get Time
	clock_gettime(CLOCK_MONOTONIC, now_time_p);
	long double elapsed_time = (now_time_p->tv_sec - before_time_p->tv_sec) + 
		((now_time_p->tv_nsec - before_time_p->tv_nsec) / NANO_PER_SEC);
	if (elapsed_time >= 1.0) {
		*produced_in_sec_p = 0;
		clock_gettime(CLOCK_MONOTONIC, before_time_p);
		pthread_cond_signal(cond_p);
	}
	return;
}
