/* GPLv2 (c) Airbus */
// Debug
#include <debug.h>
#include <intr.h>
#include <time.h>
#include <asm.h>
#include <segmentation.h>
// Pagination
#include <paging.h>

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
	asm volatile ("int $0x80"::"S"(1)); // Test syscall 1
	debug("After syscall\n");
	while(1){};
}

void tp() {
	// TODO
	init_paging();
	init_gdt();
	// ajout du timer, fonctionne
	/*init_timer(1000);
	force_interrupts_on();*/
	call_ring_3(userland);
	while (1) {}
}
