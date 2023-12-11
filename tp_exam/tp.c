/* GPLv2 (c) Airbus */
// Debug
#include <tests.h>

void tp() {
	// ajout du timer, fonctionne
	// init_timer(1000);
	// force_interrupts_on();
	test_syscall();
	// test_call_ring0_from_ring3();
	while (1) {}
}
