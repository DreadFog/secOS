/**
 * 		time.c
 * File intializing and defining the switchover between processes
 * using the clock/time as a basis.
 *  
*/

#include <time.h>
#include <intr.h>
#include <debug.h>

extern void handler_IT_timer();

uint32_t curr_time = 0; // Current time in milliseconds
uint32_t f_osc = 0x1234BD; // 1.19 MHz
uint32_t const clock_freq = 1000;
time_t time; // Current time represented as hours/minutes/seconds
int const millis_per_sec = clock_freq;
int const millis_per_min = 60 * millis_per_sec;
int const millis_per_hour = 60 * millis_per_min;

/**
 * init_timer()
 * (Re)initialize the timer, using the PIC. 
*/
void init_timer()
{
    curr_time = 0;
    time.seconds = 0;
    time.minutes = 0;
    time.hours = 0;
    uint32_t f = f_osc / clock_freq;
    // PIC configuration
    outb(0x34, 0x43);
    // freq saving
    outb(f & 0xFF, 0x40);
    outb(f >> 8, 0x40);
    // activating the timer IT
    outb(inb(0x21) & 0xfe, 0x21);
}

/**
 * update_time()
 * Converts current time curr_time to "beautified" time.
*/
void update_time()
{
    time.hours = curr_time / millis_per_hour;
    time.minutes = (curr_time % millis_per_hour) / millis_per_min;
    time.seconds = (curr_time % millis_per_min) / millis_per_sec;
}

/**
 * handler_timer()
 * Function that gets called every millisecond (at clock_freq).
 * It is responsible for updating the time and calling the scheduling
 * function when it is deemed appropriate.
*/
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
        debug("%dh:%dm:%ds\n", time.hours, time.minutes, time.seconds);
        // TODO: (fix and) call scheduler();
        //debug("scheduler should be called now\n");
    }
}
