/* GPLv2 (c) Airbus */
// Debug
#include <debug.h>
#include <intr.h>

// Pagination
#include <paging.h>

// counter print syscall
void sys_counter(uint32_t *counter) {
  	// counter est une @virtuelle ring 3
	asm volatile("int $0x80\t\n");
	counter = counter;
}

void __attribute__((section(".user"))) user1() {
	uint32_t *cpt = SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_1
	while (1) { (*cpt)++; }
}

void __attribute__((section(".user"))) user2() {
	uint32_t *cpt = SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_2
	while (1) {
		sys_counter(cpt);
	}
}

void userland() {
	// TODO
}

void tp() {
	// TODO
	init_paging();
	while (1) {}
}
