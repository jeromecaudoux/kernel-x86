#include "string.h"
#include "types.h"
#include "kprintf.h"

void	memset(void *ptr, int value, size_t size)
{
  size_t cursor;
  char *buffer;

  cursor = 0;
  buffer = ptr;
  while (cursor < size) {
    buffer[cursor] = value;
    ++cursor;
  }
}

void *memcpy(void *dest, const void *src, size_t n)
{
  size_t cursor;
  char *to = (char *)dest;
  char *from = (char *)src;

  cursor = 0;
  while (cursor < n) {
    to[cursor] = from[cursor];
    ++cursor;
  }
  return dest;
}
