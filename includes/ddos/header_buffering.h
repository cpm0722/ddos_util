#ifndef kHeadBufferingER_BUFFERING

#define kHeadBufferingER_BUFFERING

/// http request method message size in header buffering
#define __kHeadBufferingER_BUFFERING_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print header buffering usage
 * @date		2021/02/20
 * @return		void
 */
void HeaderBufferingPrintUsage();

/*!
 * @brief
 * header buffering generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateHeaderBuffering(void *data);

/*!
 * @brief
 * header buffering time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *HeaderBufferingTimeCheck(void *data);

/*!
 * @brief
 * header buffering main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void HeaderBufferingMain(char *argv[]);

#endif // ifndef kHeadBufferingER_BUFFERING
