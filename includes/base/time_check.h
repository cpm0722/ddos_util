#ifndef INCLUDES_BASE_TIME_CHECK_H_

#define INCLUDES_BASE_TIME_CHECK_H_

#define NANO_PER_SEC 1000000000.0

/*!
 * @brief
 * if elapsed time is more than 1 sec, update produced_in_sec and before_time, send condition signal to all waiting threads
 * @date      2021/04/12
 * @param[in]    cond_p        thread condition for synchronizing
 * @param[in,out]  before_time_p    timespec for before call
 * @param[in,out]  now_time_p      timespec for now call
 * @param[out]    produced_in_sec_p  number of packets which produced in before time ~ now time, it will be initialized to 0
 * @return      void
 */
void TimeCheck(pthread_cond_t *cond_p,
        struct timespec *before_time_p,
        struct timespec *now_time_p,
        uint32_t *produced_in_sec_p);

#endif  // INCLUDES_BASE_TIME_CHECK_H_
