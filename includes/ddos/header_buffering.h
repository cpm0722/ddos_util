#ifndef HEADER_BUFFERING

#define HEADER_BUFFERING

/// http request method message size in header buffering
#define __HEADER_BUFFERING_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print header buffering usage
 * @date		2021/02/20
 * @return		void
 */
void header_buffering_print_usage();

/*!
 * @brief
 * header buffering generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_header_buffering(void *data);

/*!
 * @brief
 * header buffering time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *header_buffering_time_check(void *data);

/*!
 * @brief
 * header buffering main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void header_buffering_main(char *argv[]);

#endif // ifndef HEADER_BUFFERING
