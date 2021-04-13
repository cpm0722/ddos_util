#ifndef CONN_FLOOD

#define CONN_FLOOD

/*!
 * @brief
 * print connection flooding usage
 * @date		2021/03/08
 * @return		void
 */
void conn_flood_print_usage(void);

/*!
 * @brief
 * connection flooding generate thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_conn_flood(void *data);

/*!
 * @brief
 * connection flooding time check thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *conn_flood_time_check(void *data);

/*!
 * @brief
 * connection flooding main function
 * @date		2021/03/08
 * @param[in]	argv	arguments
 * @return		void
 */
void conn_flood_main(char *argv[]);

#endif
