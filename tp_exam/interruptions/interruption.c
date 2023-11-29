#include "interruptions/int_handlers.h"
#include "segmem.h"

void init_interruptions() {
	// Gestion de l'interruption irq0
	idt_reg_t idtr;
	get_idtr(idtr);
	int_desc_t *tab_int_desc = idtr.desc;
    // c'est vraiment gdt_krn_seg_sel(1) ?
	int_desc(&tab_int_desc[32], gdt_krn_seg_sel(1), (offset_t)&irq0_handler);
}
