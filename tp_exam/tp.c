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
// counter print syscall
void sys_counter(uint32_t *counter) {
  	// counter est une @virtuelle ring 3
	asm volatile("int $0x80\t\n");
	counter = counter;
}

void __attribute__((section(".user"))) user1() {
	uint32_t *cpt = (uint32_t*)SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_1
	while (1) { (*cpt)++; }
}

void __attribute__((section(".user"))) user2() {
	uint32_t *cpt = (uint32_t*)SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_2
	while (1) {
		sys_counter(cpt);
	}
}

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
