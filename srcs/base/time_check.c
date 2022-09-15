#include "header.h"
#include "base/time_check.h"

void TimeCheck(pthread_cond_t *cond_p,
               struct timespec *before_time_p,
               struct timespec *now_time_p,
               uint32_t *produced_in_sec_p)
{
  clock_gettime(CLOCK_MONOTONIC, now_time_p);
  long double elapsed_time = (now_time_p->tv_sec - before_time_p->tv_sec) +
    ((now_time_p->tv_nsec - before_time_p->tv_nsec) / NANO_PER_SEC);
  if (elapsed_time >= 1.0) {
    printf("%u generated in sec \n", *produced_in_sec_p);

    *produced_in_sec_p = 0;
    clock_gettime(CLOCK_MONOTONIC, before_time_p);
    pthread_cond_signal(cond_p);
  }
  return;
}
