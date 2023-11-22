/* GPLv2 (c) Airbus */
// Debug
#include <debug.h>

// Pagination
#include <cr.h>
#include <pagemem.h>

// Interruptions
#include <intr.h>
#include "interruptions/interruption.h"

// TODO: ajouter la section dans le linker
/*
void __attribute__((section(".user"))) user1() { while (1) {} }
void __attribute__((section(".user"))) user2() { while (1) {} }
*/


void irq0_handler()
{
	asm volatile("pusha \t\n");
	/* TODO: 
		- switch tâche 1 <-> tâche 2 
		- savoir si on a interrompu le noyau ou une tâche utilisateur
	*/
	// Rendre la main à tp()
	asm volatile("iret \t\n");
}

void userland() {
	// TODO
}

void tp() {
	// TODO

	// Gestion de l'interruption irq0
	idt_reg_t idtr;
	get_idtr(idtr);
	int_desc_t *tab_int_desc = idtr.desc;
	int_desc(&tab_int_desc[32], gdt_krn_seg_sel(1), (offset_t)&irq0_handler); // c'est vraiment gdt_krn_seg_sel(1) ?

	while (1) {}
}
