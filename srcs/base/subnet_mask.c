#include "header.h"
#include "base/subnet_mask.h"

__u32 GetAddressIntegerValue(__uc *str)
{
  __u32 res = 0;
  __uc tmp_str[IPV4_STRLEN];
  memset(tmp_str, '\0', IPV4_STRLEN);
  snprintf(tmp_str, IPV4_STRLEN, "%s", str);
  char *next_ptr;
  char *ret_ptr = strtok_r(tmp_str, ".", &next_ptr);
  for (int i = 3; i >= 0; i--) {
    __u32 val = atoi(ret_ptr);
    res |= val << (BYTE_LEN * i);
    ret_ptr = strtok_r(NULL, ".", &next_ptr);
  }
  return res;
}

void GetAddressStr(__u32 val, __uc str[IPV4_STRLEN])
{
  __u32 split[4] = { 0, 0, 0, 0 };
  for (int i = 3; i >= 0; i--) {
    split[i] = (val >> (BYTE_LEN * i)) & BYTE_MAX_VAL;
  }
  snprintf(str, IPV4_STRLEN, "%u.%u.%u.%u",
      split[3], split[2], split[1], split[0]);
  return;
}

int MaskingNextIpAddress(__uc *ipv4, __uc now[IPV4_STRLEN], __u32 mask)
{
  __u32 now_addr;
  if (!strlen(now)) {   // first call
    __u32 ipv4_addr = GetAddressIntegerValue(ipv4);
    now_addr = ipv4_addr;
    __u32 max_val = BIT_32_MAX_VAL;


    now_addr = (__u32) now_addr & (max_val << (32 - mask));

    if ((now_addr & 0xff) == 0x00)
            now_addr += 1;

    GetAddressStr(now_addr, now);
    return -1;
  }
  now_addr = GetAddressIntegerValue(now);
  __u32 max_val = (__u32) pow(2, (32 - mask)) - 1;
  if (((now_addr & max_val) == max_val) ||
       (((now_addr + 1) & 0xff) == 0xff)) {  // finish
    __u32 ipv4_addr = GetAddressIntegerValue(ipv4);
    now_addr = ipv4_addr;

    __u32 max_val = BIT_32_MAX_VAL;
    now_addr = (__u32) now_addr & (max_val << (32 - mask));

    if ((now_addr & 0xff) == 0x00)
      now_addr += 1;

    GetAddressStr(now_addr, now);
    return 1;
  }
  now_addr++;
  GetAddressStr(now_addr, now);
  return 0;
}

__u32 GetMaskFromIpv4Format(__uc *ipv4)
{
  __u32 mask = 32;
  __uc *char_ptr = ipv4 + 7;
  int i = 0;
  while (char_ptr[i] != '/') {
    i++;
    if (i > strlen(ipv4))
      return 32;
  }
  __uc mask_str[4];
  i++;
  snprintf(mask_str, sizeof(mask_str), "%s", char_ptr + i);
  mask = atoi(mask_str);
  return mask;
}

void GetIpAddressFromIpv4Format(__uc *ipv4, __uc *now)
{
  __uc *char_ptr = ipv4;
  int i = 0;
  while (char_ptr[i] != '/') {
    i++;
    if (i > strlen(ipv4)){
      snprintf(now, IPV4_STRLEN, "%s", ipv4);
      return;
    }
  }
  memcpy(now, ipv4, sizeof(__uc) * i);
  now[i] = '\0';
  return;
}

int ArgvToInputArguments(char *argv[], InputArguments *input)
{
  __uc *src = argv[0];
  __uc *dest = argv[1];
  __uc *port = argv[2];
  GetIpAddressFromIpv4Format(src, input->src);
  GetIpAddressFromIpv4Format(dest, input->dest);
  input->src_mask = GetMaskFromIpv4Format(src);
  input->dest_mask = GetMaskFromIpv4Format(dest);

  char *next_ptr;
  char *ret_ptr = strtok_r(port, "-", &next_ptr);
  input->port_start = atoi(ret_ptr);
  ret_ptr = strtok_r(NULL, "-", &next_ptr);
  if (ret_ptr) {
    input->port_end = atoi(ret_ptr);
  } else {
    input->port_end = input->port_start;
  }
  return 0;
}

int GetMaskingArguments(InputArguments *input, MaskingArguments *now)
{
  // last dest_ipv4 or first(blank)
  if (MaskingNextIpAddress(input->dest, now->dest, input->dest_mask)) {
    int res;
    // last src_ipv4
    if ((res = MaskingNextIpAddress(input->src,
            now->src,
            input->src_mask)) > 0) {
      (now->port)++;
      if (now->port > input->port_end) {
        now->port = input->port_start;
      }
    } else if (res < 0) {
      now->port = input->port_start;
    }
  }
  return 0;
}
