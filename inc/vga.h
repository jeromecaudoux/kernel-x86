#ifndef HEADER_VGA
#define HEADER_VGA

#define	VIDEO_MEM_START_OFFSET	(0xb8000 + 0xc0000000)
//#define	VIDEO_MEM_START_OFFSET	(0xb8000)
#define MAX_ROWS_COUNT		25
#define MAX_COLS_COUNT		80

void	print_char_on_screen(unsigned int row, unsigned int col,
			     char c, unsigned short color);
void	printk_on_screen(unsigned int row, unsigned int col,
			 const char *string);
void	clear_screen(void);

#endif
