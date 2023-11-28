#include	"kwrite.h"
#include	"uart.h"

void kwrite(kwrite_output output, const void *buf, size_t count)
{
	switch (output) {
		case KWRITE_UART:
			uart_send_buffer(buf, count);
			return;
	}
	// todo : invalid output
}
