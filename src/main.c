#include "main.h"
#include "vga.h"
#include "serial_port.h"
#include "segmentation.h"
#include "interrupts.h"
#include "kb.h"
#include "pit.h"
#include "irq.h"
#include "uart.h"
#include "pit.h"
#include "frame.h"
#include "pages.h"
#include "time.h"
#include "kprintf.h"
#include "kmalloc/includes/kmalloc.h"
#include "kmalloc/includes/free.h"
#include "test.h"

static t_kfs_context g_kfs_context;

static void do_printk(unsigned int row, unsigned int col, const char *string)
{
  printk_on_screen(row, col, string);
  uart_putstr(string);
}

void printk(const char *string)
{
  do_printk(0, 0, string);
}

static void print_hello_world(void)
{
  printk("\n");
  while (1)
  {
    printk("hello world !\n");
    sleep(1);
  }
}

void run_debug_test(void)
{
  // test_kmalloc_sbrk();
  //   test_alloc_all_memory();
  /* test_alloc_frames(); */
  /* test_dump_screen(); */
}

void main_new(void)
{
  finalize_pagination();
  if (g_kfs_context.magic_number != MAGIC_NUMBER)
  {
    printk("Invalid magic number");
    return;
  }

  init_flat_gdt();
  irq_init(PICS_BASE);
  kb_init();
  pit_init(100);
  uart_init(SERIAL_PORT);

  if (!uart_is_init())
  {
    printk("uart_init failed.");
  }

  interrupts_init();
  init_page_tables();
  init_frame_alloc(g_kfs_context.multiboot_info);

  run_debug_test();

  sleep(1);

  print_hello_world();
}

int main(unsigned int magic_number, multiboot_info_t *multiboot_info)
{
  t_kfs_context *context = (t_kfs_context *)((unsigned long)&g_kfs_context - (unsigned long)SYSTEM_ADDR_BASE);

  context->magic_number = magic_number;
  context->multiboot_info = (multiboot_info_t *)((unsigned long)SYSTEM_ADDR_BASE + (unsigned long)multiboot_info);

  init_page_directory();
  enable_paging();
  return 0;
}
