#ifndef kSynFlooding_FLOOD

#define kSynFlooding_FLOOD

/*!
 * @brief
 * print syn flooding usage
 * @date		2021/03/08
 * @return		void
 */
void SynFloodPrintUsage(void);

/*!
 * @brief
 * syn flooding generate thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *GenerateSynFlood(void *data);

/*!
 * @brief
 * syn flooding time check thread
 * @date		2021/03/08
 * @param[in]	data	thread input, default: NULL
 * @return		void *
 * @retval		NULL: always
 */
void *SynFloodTimeCheck(void *data);

/*!
 * @brief
 * syn flooding main function
 * @date		2021/03/08
 * @param[in]	argv	arguments
 * @return		void
 */
void SynFloodMain(char *argv[]);

#endif // ifndef kSynFlooding_FLOOD
