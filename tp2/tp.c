/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <intr.h>

void bp_handler()
{
	//uint32_t val;
	asm volatile("pusha \t\n"); // est censé fix le problème de sauvegarde de contexte
	/* asm volatile("mov 4(%%ebp), %0" : "=r"(val));
	printf("%x\n\n\n\n", val); */
	/*
		val contient l'@ de retour après interruption : cette @ provient de la sauvegarde de contexte qui est effectuée pendant l'interruption.
	*/
	debug("Handled BP interruption!\n");
	// problème: cette fonction finit par un ret qui est exécuté alors qu'on n'a pas call la fonction
	asm volatile("iret \t\n");
}

void bp_trigger()
{
	debug("Triggering BP...\n");
	asm volatile("int3 \t\n");
}

void tp()
{
	// TODO print idtr

	idt_reg_t idtr;
	get_idtr(idtr);
	int_desc_t *tab_int_desc = idtr.desc;
	int_desc(&tab_int_desc[3], gdt_krn_seg_sel(1), (offset_t)&bp_handler);

	// TODO call bp_trigger
	/* 	asm volatile("int3 \t\n");
		debug("oqei_ghqoil\n"); */
	bp_trigger();
}