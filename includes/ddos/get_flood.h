#ifndef kGetFlooding_FLOOD

#define kGetFlooding_FLOOD

/// http request method message size in get flooding
#define __kGetFlooding_REQUEST_MSG_SIZE__ 100

/*!
 * @brief
 * print get flooding usage
 * @date		2021/02/20
 * @return		void
 */
void GetFloodPrintUsage(void);

/*!
 * @brief
 * get flooding generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateGetFlood(void *data);

/*!
 * @brief
 * get flooding time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GetFloodTimeCheck(void *data);

/*!
 * @brief
 * get flooding main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void GetFloodMain(char *argv[]);

#endif // ifndef kGetFlooding_FLOOD
