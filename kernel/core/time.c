#include <time.h>
#include <intr.h>
#include <debug.h>
extern void handler_IT_timer();

uint32_t curr_time = 0;
uint32_t f_osc = 0x1234BD; // 1.19 MHz
uint32_t f = 1000;
time_t time;
int millis_per_sec;
int millis_per_min;
int millis_per_hour;
int millis_per_day;

void init_timer(uint32_t clock_freq)
{
    millis_per_sec = clock_freq;
    millis_per_min = 60 * millis_per_sec;
    millis_per_hour = 60 * millis_per_min;
    curr_time = 0;
    time.seconds = 0;
    time.minutes = 0;
    time.hours = 0;
    f = f_osc / clock_freq;
    // PIC configuration
    outb(0x34, 0x43);
    // freq saving
    outb(f & 0xFF, 0x40);
    outb(f >> 8, 0x40);
    // activating the timer IT
    outb(inb(0x21) & 0xfe, 0x21);
    // init_irq_entry(0x20, (uint32_t)handler_IT_timer); // taken care of during init_intr()
}

void update_time()
{
    time.hours = curr_time / millis_per_hour;
    time.minutes = (curr_time % millis_per_hour) / millis_per_min;
    time.seconds = (curr_time % millis_per_min) / millis_per_sec;
    debug("%d:%d:%d\n", time.hours, time.minutes, time.seconds);
}

void handler_timer()
{
    // ack IT
    outb(0x20, 0x20);
    // inc time
    curr_time++;
    update_time();
    // call the scheduler every second
    if (curr_time % millis_per_sec == 0)
    {
        // scheduler();
        debug("scheduler\n");
    }
}
