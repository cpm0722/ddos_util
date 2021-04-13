#ifndef ICMP_FLOOD

#define ICMP_FLOOD

/*!
 * @brief
 * print icmp flooding usage
 * @date		2021/02/20
 * @return		void
 */
void icmp_flood_print_usage(void);

/*!
 * @brief
 * icmp flooding generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_icmp_flood(void *data);

/*!
 * @brief
 * icmp flooding time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *icmp_flood_time_check(void *data);

/*!
 * @brief
 * icmp flooding main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void icmp_flood_main(char *argv[]);

#endif // ifndef ICMP_FLOOD
