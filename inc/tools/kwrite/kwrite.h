#ifndef KWRITE_H_
# define KWRITE_H_

# include "types.h"

typedef enum e_kwrite_output {
	KWRITE_UART
} kwrite_output;

void kwrite(kwrite_output fd, const void *buf, size_t count);

#endif /* !KWRITE_H_ */
