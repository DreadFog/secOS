/* GPLv2 (c) Airbus */
// Debug
#include <tests.h>

void default_configuration() {
	init_gdt();
	init_tss();
    init_paging();
	init_syscall_table();
	init_process_table(NULL);
	// init_timer(1000);
	// force_interrupts_on();
}

void tp() {
	debug("====================================\n");
	debug("                 TP                 \n");
	debug("====================================\n");
    default_configuration();
    associate_syscall_handler(0, (uint32_t)handler_sys_terminate);
    associate_syscall_handler(1, (uint32_t)handler_sys_counter);
	add_process("user1", 0, user1);
	add_process("user2", 0, user2);
	call_ring_3_pid_1();
	while (1) {
		debug("Kernel process!\n");
	}
}
