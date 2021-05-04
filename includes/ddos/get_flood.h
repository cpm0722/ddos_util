#ifndef INCLUDES_DDOS_GET_FLOOD_H_

#define INCLUDES_DDOS_GET_FLOOD_H_

/*!
 * @brief
 * print get flooding usage
 * @date    2021/02/20
 * @return    void
 */
void GetFloodPrintUsage(void);

/*!
 * @brief
 * get flooding generate thread
 * @date    2021/02/20
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *GenerateGetFlood(void *data);

/*!
 * @brief
 * get flooding time check thread
 * @date    2021/02/20
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *GetFloodTimeCheck(void *data);

/*!
 * @brief
 * get flooding main function
 * @date    2021/02/20
 * @param[in]  argv  arguments
 * @return    void
 */
void GetFloodMain(char *argv[]);

#endif  // INCLUDES_DDOS_GET_FLOOD_H_
