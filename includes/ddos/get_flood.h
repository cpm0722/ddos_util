#ifndef GET_FLOOD

#define GET_FLOOD

/// http request method message size in get flooding
#define __GET_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print get flooding usage
 * @date		2021/02/20
 * @return		void
 */
void get_flood_print_usage(void);

/*!
 * @brief
 * get flooding generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_get_flood(void *data);

/*!
 * @brief
 * get flooding time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *get_flood_time_check(void *data);

/*!
 * @brief
 * get flooding main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void get_flood_main(char *argv[]);

#endif // ifndef GET_FLOOD
