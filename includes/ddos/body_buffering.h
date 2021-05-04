#ifndef INCLUDES_DDOS_BODY_BUFFERING_H_

#define INCLUDES_DDOS_BODY_BUFFERING_H_

/*!
 * @brief
 * print body buffering usage
 * @date    2021/02/20
 * @return    void
 */
void BodyBufferingPrintUsage(void);

/*!
 * @brief
 * body buffering generate thread
 * @date    2021/02/20
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *GenerateBodyBuffering(void *data);

/*!
 * @brief
 * body buffering time check thread
 * @date    2021/02/20
 * @param[in]  data  thread input, default: NULL
 * @return    void *
 * @retval    NULL: always
 */
void *BodyBufferingTimeCheck(void *data);

/*!
 * @brief
 * body buffering main function
 * @date    2021/02/20
 * @param[in]  argv  arguments
 * @return    void
 */
void BodyBufferingMain(char *argv[]);

#endif  // INCLUDES_DDOS_BODY_BUFFERING_H_
