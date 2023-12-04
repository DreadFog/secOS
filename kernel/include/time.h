#ifndef __TIME_H__
#define __TIME_H__
#include <unistd.h>
#include <inttypes.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <stdio.h>
#include <n7OS/console.h>
#include <n7OS/proc.h>
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#endif
typedef struct {
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
} time_t;
void init_timer(uint32_t clock_freq);
void handler_timer();