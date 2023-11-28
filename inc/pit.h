#ifndef PIT_H_
# define PIT_H_

#include "types.h"
#include "interrupts.h"

#define COUNTER_0	0x40 /* Base adress for the Counter 0 */
#define COUNTER_1	0x41 /* Base adress for the Counter 1 */
#define COUNTER_2	0x42 /* Base adress for the Counter 2 */
#define COUNTER_REG	0x43 /* Base adress for the Counter Register */

#define SC_0		0x00 /* Select counter 0 */
#define SC_1		0x40 /* Select counter 1 */
#define SC_2		0x80 /* Select counter 2 */
#define R_BC		0xc0 /* Read back command */

#define RW_CLC		0X00 /* Flag counter latch command */
#define RW_LBO		0X10 /* Read Write the least significant byte only */
#define RW_MBO		0X20 /* Read Write the most significant byte only */
#define RW_LMB		0X30 /* Read Write the least significant byte then the most significant byte */

#define MODE_0		0x00
#define MODE_1		0x02
#define MODE_2		0x40
#define MODE_3		0x60
#define MODE_4		0x08
#define MODE_5		0x0a

#define FREQUENCY	1193182

#define BCD		0x01 /* Binary Coded Decimal Counter */

void pit_handler(__attribute__((unused))struct regs *regs);
int pit_init(int freq);
uint32_t pit_get_ticks(void);

#endif /* !PIT_H_ */
