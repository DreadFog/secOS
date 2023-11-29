/* GPLv2 (c) Airbus */
// Debug
#include <debug.h>

// Pagination
#include <pagination/paging.h>

// Interruptions
#include "interruptions/interruption.h"

// TODO: ajouter la section dans le linker
void __attribute__((section(".user"))) user1() {
	uint32_t *cpt = SHARED_MEM_ADDR;
	while (1) { (*cpt)++; }
}

void __attribute__((section(".user"))) user2() {
	while (1) {}
}

void userland() {
	// TODO
}

void tp() {
	// TODO
	while (1) {}
}
