#ifndef BODY_BUFFERING

#define BODY_BUFFERING

/// http request method message size in body buffering
#define __BODY_BUFFERING_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print body buffering usage
 * @date		2021/02/20
 * @return		void
 */
void body_buffering_print_usage(void);

/*!
 * @brief
 * body buffering generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_body_buffering(void *data);

/*!
 * @brief
 * body buffering time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *body_buffering_time_check(void *data);

/*!
 * @brief
 * body buffering main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void body_buffering_main(char *argv[]);

#endif // ifndef BODY_BUFFERING
