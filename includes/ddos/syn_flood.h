#ifndef SYN_FLOOD

#define SYN_FLOOD

/*!
 * @brief
 * print syn flooding usage
 * @date		2021/03/08
 * @return		void
 */
void syn_flood_print_usage(void);

/*!
 * @brief
 * syn flooding generate thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_syn_flood(void *data);

/*!
 * @brief
 * syn flooding time check thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *syn_flood_time_check(void *data);

/*!
 * @brief
 * syn flooding main function
 * @date		2021/03/08
 * @param[in]	argv	arguments
 * @return		void
 */
void syn_flood_main(char *argv[]);

#endif // ifndef SYN_FLOOD
