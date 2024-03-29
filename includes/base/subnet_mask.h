#ifndef INCLUDES_BASE_SUBNET_MASK_H_

#define INCLUDES_BASE_SUBNET_MASK_H_

/// 32bit maximum value
#define BIT_32_MAX_VAL 0xffffffff
/// 1byte maximum value
#define BYTE_MAX_VAL 0xff
/// bit per byte
#define BYTE_LEN 8

/*!
 * @brief
 * change ipv4 format: string(XXX.XXX.XXX.XXX) -> uint32_t
 * @date    2021/04/03
 * @param[in]  str    ipv4 string(XXX.XXX.XXX.XXX)
 * @return    uint32_t
 * @retval    ipv4 32bit value
 */
uint32_t GetAddressIntegerValue(char *str);

/*!
 * @brief
 * change ipv4 format: __32 -> string(XXX.XXX.XXX.XXX)
 * @date    2021/04/03
 * @param[in]  val    ipv4 32bit value
 * @param[out]  str    ipv4 string(XXX.XXX.XXX.XXX)
 * @return    void
 */
void GetAddressStr(uint32_t val, char str[IPV4_STRLEN]);

/*!
 * @brief
 * update now ipv4 to next ipv4 with subnet masking value
 * @date    2021/04/03
 * @param[in]  ipv4  initial input of ipv4 string(XXX.XXX.XXX.XXX)
 * @param[out]  now    now ipv4 string(XXX.XXX.XXX.XXX), it will be updated to next ipv4.
 * @param[in]  mask  initial input of subnet masking value(0~32)
 * @return    int
 * @retval    -1: now parameter was empty string
 * @retval    1: there is no next ipv4 address in subnet masking
 * @reval    0: else
 */
int MaskingNextIpAddress(char *ipv4, char now[IPV4_STRLEN], uint32_t mask);

/*!
 * @brief
 * get subnet masking value from masking ipv4 string(XXX.XXX.XXX.XXX/XX)
 * @date    2021/04/03
 * @param[in]  ipv4  initial input of ipv4/mask string(XXX.XXX.XXX.XXX/XX)
 * @return    uint32_t
 * @retval    subnet masking value
 */
uint32_t GetMaskFromIpv4Format(char *ipv4);

/*!
 * @brief
 * get ipv4 str(XXX.XXX.XXX.XXX) from masking ipv4 str(XXX.XXX.XXX.XXX/XX)
 * @date    2021/04/03
 * @param[in]  ipv4  initial input of ipv4/mask string(XXX.XXX.XXX.XXX/XX)
 * @param[out]  now    now ipv4 string(XXX.XXX.XXX.XXX)
 * @return    void
 */
void GetIpAddressFromIpv4Format(char *ipv4, char *now);

/*!
 * @brief
 * split argv to arguments(ipv4, masking, port)
 * @date    2021/04/03
 * @param[in]  argv  argument vectors from main(in command mode) or stdin(in prompt mode)
 * @param[out]  input  InputArguments structure
 * @return    int
 * @retval    always 0 (normal exit)
 */
int ArgvToInputArguments(char *argv[], InputArguments *input);

/*!
 * @brief
 * get next MaskingArguments(ipv4, masking, port) using InputArguments
 * @date      2021/04/03
 * @param[in/out]  input  InputArguments structure
 * @param[out]    now    MaskingArgument structure
 * @return      int
 * @retval      always 0 (normal exit)
 */
int GetMaskingArguments(InputArguments *input, MaskingArguments *now);

#endif  // INCLUDES_BASE_SUBNET_MASK_H_
