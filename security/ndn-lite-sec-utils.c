
#include "ndn-lite-sec-utils.h"

#include <stddef.h>

int
ndn_const_time_memcmp(const uint8_t* a, const uint8_t* b, uint32_t size)
{
  unsigned char result = 0;
  for (size_t i = 0; i < size; i++) {
    result |= a[i] ^ b[i];
  }
  return result; /* returns 0 if equal, nonzero otherwise */
}