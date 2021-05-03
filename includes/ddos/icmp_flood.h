#ifndef ICMP_FLOOD_H_

#define ICMP_FLOOD_H_

/*!
 * @brief
 * print icmp flooding usage
 * @date		2021/02/20
 * @return		void
 */
void IcmpFloodPrintUsage(void);

/*!
 * @brief
 * icmp flooding generate thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateIcmpFlood(void *data);

/*!
 * @brief
 * icmp flooding time check thread
 * @date		2021/02/20
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *IcmpFloodTimeCheck(void *data);

/*!
 * @brief
 * icmp flooding main function
 * @date		2021/02/20
 * @param[in]	argv	arguments
 * @return		void
 */
void IcmpFloodMain(char *argv[]);

#endif // ifndef ICMP_FLOOD_H_
