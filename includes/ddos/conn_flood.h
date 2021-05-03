#ifndef CONN_FLOOD_H_

#define CONN_FLOOD_H_

/*!
 * @brief
 * print connection flooding usage
 * @date		2021/03/08
 * @return		void
 */
void ConnectionFloodPrintUsage(void);

/*!
 * @brief
 * connection flooding generate thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateConnFlood(void *data);

/*!
 * @brief
 * connection flooding time check thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *ConnectionFloodTimeCheck(void *data);

/*!
 * @brief
 * connection flooding main function
 * @date		2021/03/08
 * @param[in]	argv	arguments
 * @return		void
 */
void ConnFloodMain(char *argv[]);

#endif // CONN_FLOOD_H_
