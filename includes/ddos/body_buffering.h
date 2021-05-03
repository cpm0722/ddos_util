#ifndef kBodyBuffering_BUFFERING

#define kBodyBuffering_BUFFERING

/// http request method message size in body buffering
#define __kBodyBuffering_BUFFERING_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print body buffering usage
 * @date		2021/02/20
 * @return		void
 */
void BodyBufferingPrintUsage(void);

/*!
 * @brief
 * body buffering generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateBodyBuffering(void *data);

/*!
 * @brief
 * body buffering time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *BodyBufferingTimeCheck(void *data);

/*!
 * @brief
 * body buffering main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void BodyBufferingMain(char *argv[]);

#endif // ifndef kBodyBuffering_BUFFERING
