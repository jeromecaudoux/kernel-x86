#include "types.h"
#include "vga.h"

void print_char_on_screen(unsigned int row, unsigned int col,
			  char c, unsigned short color)
{
  char *video_mem = (void *)VIDEO_MEM_START_OFFSET;

  video_mem[(row * 80 + col) * 2] = c;
  video_mem[(row * 80 + col) * 2 + 1] = color;
}

void printk_on_screen(unsigned int row, unsigned int col, const char *string)
{
  int i = 0;

  while (string[i] != NULL) {
    print_char_on_screen(row, col + i, string[i], 0x14);
    ++i;
  }
}

void clear_screen(void)
{
  int col = 0;
  int row = 0;

  while (row < MAX_ROWS_COUNT) {
    col = 0;
    while (col < MAX_COLS_COUNT) {
      print_char_on_screen(row, col, ' ', 0x0);
      ++col;
    }
    ++row;
  }
}
