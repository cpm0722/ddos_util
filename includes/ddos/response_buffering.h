#ifndef RESPONSE_BUFFERING_H_

#define RESPONSE_BUFFERING_H_

/*!
 * @brief
 * print response buffering usage
 * @date		2021/03/15
 * @return		void
 */
void ResponseBufferingPrintUsage(void);

/*!
 * @brief
 * response buffering generate thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateResponseBuffering(void *data);

/*!
 * @brief
 * response buffering time check thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *ResponseBufferingTimeCheck(void *data);

/*!
 * @brief
 * response buffering main function
 * @date		2021/03/15
 * @param[in]	argv	arguments
 * @return		void
 */
void ResponseBufferingMain(char *argv[]);

#endif // ifndef RESPONSE_BUFFERING_H_
