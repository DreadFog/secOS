#ifndef __TIME_H__
#define __TIME_H__
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#include <types.h>
#include <io.h>
#include <intr.h>
#endif

typedef struct {
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
} time_t;

/**
 * init_timer()
 * (Re)initialize the timer, using the PIC. 
*/
void init_timer();

void handler_timer();