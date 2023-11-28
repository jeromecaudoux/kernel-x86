#include "time.h"
#include "pit.h"

// duration in second
void sleep(uint32_t duration)
{
  uint32_t	start;

  start = pit_get_ticks();
  duration *= 100;

  while (1) {
    if (pit_get_ticks() - start > duration)
      return;
  }
}
