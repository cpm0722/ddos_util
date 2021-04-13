#ifndef RESPONSE_BUFFERING

#define RESPONSE_BUFFERING

/*!
 * @brief
 * print response buffering usage
 * @date		2021/03/15
 * @return		void
 */
void response_buffering_print_usage(void);

/*!
 * @brief
 * response buffering generate thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_response_buffering(void *data);

/*!
 * @brief
 * response buffering time check thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *response_buffering_time_check(void *data);

/*!
 * @brief
 * response buffering main function
 * @date		2021/03/15
 * @param[in]	argv	arguments
 * @return		void
 */
void response_buffering_main(char *argv[]);

#endif // ifndef RESPONSE_BUFFERING
