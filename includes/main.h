#ifndef MAIN_H

#define MAIN_H

/*!
 * @brief
 * read stdin g_input to with printing a prompt character('$') 
 * @date		2021/04/12
 * @return		void
 */
void get_input(void);

/*!
 * @brief
 * tokenize(seperator ' ') g_input to g_tokens
 * @date		2021/04/12
 * @return		void
 */
void make_tokens(void);

/*!
 * @brief
 * read argv[] and handle option (-r, -c, -t, --help),
 * decide program's mode (command mode / prompt mode)
 * @date		2021/04/12
 * @param[in]	argv	main's argv
 * @param[in]	argc	main's argc
 * @return		bool
 * @retval		1(true): is command mode
 * @retval		0(false): is prompt mode
 */
bool check_options(char *argv[], int argc);

/*!
 * @brief
 * in command mode, convert argv to g_tokens 
 * @date		2021/04/12
 * @param[in]	argv	main's argv
 * @param[in]	argc	main's argc
 * @return		attack_type
 * @retval		1~10: correct atack type (attack_type's enum value)
 * @retval		0: NONE (error occured)
 */
attack_type argv_to_tokens(char *argv[], int argc);

/*!
 * @brief
 * print attack types and read stdin for choosing
 * @date		2021/04/12
 * @return		int
 * @retval		1~10: correct atack type (attack_type's enum value)
 * @retval		0: NONE (error occured)
 */
attack_type type_choose_attack(void);

/*!
 * @brief
 * print DDoS Util logo
 * @date		2021/04/12
 * @return		void
 */
void print_logo(void);

/*!
 * @brief
 * print man page when --help option or argument error
 * @date		2021/04/12
 * @return		void
 */
void print_usage(char *argv[]);

/*!
 * @brief
 * program's main function
 * @date		2021/04/12
 * @param[in]	argv	argument vectors
 * @param[in]	argc	argument count
 * @return		int
 * @retval		0	normal exit
 * @retval		1	error
 */
int main(int argc, char *argv[]);

#endif //ifndef MAIN_H
