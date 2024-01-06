/* GPLv2 (c) Airbus */

/**
 * 		tp.c
 * Main file of the 32bits kernel "SecOS".
 *  
*/

// Debug
#include <tests.h>

/** 
 * default_configuration()
 * Initialize to default values all system tables
 * as well as the timer irq0 for switching process.
 * Are initialized: the GDT, the TSS, the page table,
 * the syscall table, the process table
 * 
*/
void default_configuration() {
	init_gdt();
	init_tss();
    init_paging();
	init_syscall_table();
	init_process_table(NULL);
	init_timer();
	force_interrupts_on();
}


/** 
 * tp()
 * Main code of the kernel.
*/
void tp() {
	debug("====================================\n");
	debug("                 TP                 \n");
	debug("====================================\n");
    default_configuration();
    associate_syscall_handler(0, (uint32_t)handler_sys_counter);
    // associate_syscall_handler(0, (uint32_t)handler_sys_terminate); // we can add other syscalls if needed (with different syscall values)
	add_process("user1", 0, user1);
	add_process("user2", 0, user2);
	call_ring_3_pid_1();
	while (1) {
		debug("Kernel process!\n");
	}
}
