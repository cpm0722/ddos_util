#ifndef kHashDos_DOS

#define kHashDos_DOS

/*!
 * @brief
 * print hash dos usage
 * @date		2021/03/15
 * @return		void
 */
void HashDosPrintUsage(void);

/*!
 * @brief
 * hash dos generate thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateHashDos(void *data);

/*!
 * @brief
 * hash dos time check thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *HashDosTimeCheck(void *data);

/*!
 * @brief
 * hash dos main function
 * @date		2021/03/15
 * @param[in]	argv	arguments
 * @return		void
 */
void HashDosMain(char *argv[]);

#endif // ifndef kHashDos_DOS
