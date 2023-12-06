/* GPLv2 (c) Airbus */
// Debug
#include <debug.h>
#include <intr.h>
#include <time.h>
#include <asm.h>
#include <segmentation.h>
#include <paging.h>
#include <tests.h>
// #include <proc.h>

void userland() {
	// while(1) { debug("userland\n"); }
	// asm volatile("mov %eax, %cr0"); // crashes, as we are in ring 3
	debug("Before syscall\n");
	asm volatile ("int $0x80"::"S"(1)); // Test syscall 1
	debug("After syscall\n");
	// asm volatile("mov %eax, %cr0"); //back to ring 3, so it crashes
	while(1){};
}

void tp() {
	// ajout du timer, fonctionne
	// init_timer(1000);
	// force_interrupts_on();
	test_syscall();
	// test_call_ring0_from_ring3();
	while (1) {}
}
