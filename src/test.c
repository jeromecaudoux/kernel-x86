#include	"test.h"
#include	"frame.h"
#include	"pages.h"
#include	"kprintf.h"
#include	"kmalloc/includes/kmalloc.h"
#include	"kmalloc/includes/free.h"

void test_kmalloc_sbrk()
{
    char *buffer = kmalloc(100);
    kprintf("buffer => %p\n", buffer);
    kprintf("kds => %p\n", sbrk(0));
    free(buffer);
    kprintf("kds => %p\n", sbrk(0));
}

void test_alloc_all_memory()
{
    size_t bytes_per_alloc = 10 << 20;
    kprintf("test_alloc_all_memory: %B\n", bytes_per_alloc);
    //mem_dump();
    size_t i = 0;
    while (1) {
        char *buffer2 = kmalloc(bytes_per_alloc);
        if (buffer2 != NULL) {
            i += (bytes_per_alloc);
        }
        else {
            kprintf("kmalloc failed after %B allocation. (%X)\n", i, i * bytes_per_alloc);
            break;
        }
    }
    kprintf("kds => %p\n", sbrk(0));
}

void test_alloc_frames()
{
    struct frame *frame = NULL;

    kprintf("frame => %p\n", frame);
    alloc_frames(2, &frame);
    kprintf("frame => %p\n", frame);
    map_pages(get_kpd_frame(), get_frames_context()->frames_meta_end_vaddr, frame, 2, PF_PRES | PF_RW);

    uint32_t *test = (uint32_t *) 0xC0801ffc; // at 1ffd it throw a page fault (uint32_t = 4bytes)
    *test = 42;
    kprintf("value => %p\n", *test);
}

void test_dump_screen()
{
    // dump screen
    char *video_mem = map_io(get_kpd_frame(), 0xB8000, 80 * 25 * 2);
    kprintf("videomem %p\n", video_mem);
    int y = 0;
    int x;
    while (y < 25) {
        x = 0;
        while (x < 80) {
            kprintf("%c", video_mem[(y * 80 + x) * 2]);
            ++x;
        }
        kprintf("\n");
        ++y;
    }
}
