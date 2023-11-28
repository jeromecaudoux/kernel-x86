#ifndef IRQ_H_
# define IRQ_H_

#include "types.h"

#define PIC1		0x20 /* IO base address for master PIC */
#define PIC2		0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PICS_BASE	0x20 /* Interrupts number from 0x00 to 0x08 */
#define PIC1_OFFSET	0x0 /* Interrupts number from 0x00 to 0x08 */
#define PIC2_OFFSET     0x8 /* Interrupts number from 0x08 to 0x0F */

#define ICW1_ICW4	0x01 /* ICW4 (not) needed */
#define ICW1_SINGLE	0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08 /* Level triggered (edge) mode */
#define ICW1_INIT	0x10 /* Initialization - required! */

#define ICW4_8086	0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08 /* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C /* Buffered mode/master */
#define ICW4_SFNM	0x10 /* Special fully nested (not) */

#define PIC_EOI		0x20 /* End-of-interrupt command code */

int	irq_init(uint8_t base);
void	irq_mask(int irq);
void	irq_unmask(int irq);
void	irq_send_eoi(unsigned char irq);

#endif /* !IRQ_H_ */
