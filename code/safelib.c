#include <stdio.h>
#include <dlfcn.h>
#include "return_address.c"

char *strcpy(char *dest, const char *src)
{
  // compute length of input string
  int i = 0;
  while(src[i] != '\0')
  {
    i++;
  }

  // compute upper bound of destination's buffer size
  int upper_bound = i + int(dest);
  // bounds check
  if(upper_bound > (int)get_addr_of_ret_addr())
  {

  // call libc's memcpy()
    memcpy(dest, src, i);
  }
  // return
  return dest;
}