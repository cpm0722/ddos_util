#ifndef HASH_DOS

#define HASH_DOS

/*!
 * @brief
 * print hash dos usage
 * @date		2021/03/15
 * @return		void
 */
void hash_dos_print_usage(void);

/*!
 * @brief
 * hash dos generate thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *generate_hash_dos(void *data);

/*!
 * @brief
 * hash dos time check thread
 * @date		2021/03/15
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *hash_dos_time_check(void *data);

/*!
 * @brief
 * hash dos main function
 * @date		2021/03/15
 * @param[in]	argv	arguments
 * @return		void
 */
void hash_dos_main(char *argv[]);

#endif // ifndef HASH_DOS
