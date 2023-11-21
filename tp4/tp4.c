/* GPLv2 (c) Airbus */
#include <debug.h>
#include <pagemem.h>
#include <string.h>
#include <cr.h>

void tp() {
	// Q1
	cr3_reg_t cr3 = {.raw = get_cr3()};
	debug("CR3 = 0x%x\n", (unsigned int) cr3.raw);
	// end Q1

	// Q2                              // 0x001 (10bits) -- 0x300 (10bits) -- 0x000 (12 bits)
	pde32_t *pgd = (pde32_t*)0x600000; // 00 0000 0001   -- 10 0000 0000   -- 0000 0000 0000
	set_cr3((uint32_t)pgd);
	// end Q2

	// Q3 
	// uint32_t cr0 = get_cr0();
	// set_cr0(cr0|CR0_PG);
	// encore un peu tôt d'activer la pagination à ce stade :)
	// notamment car le pgd est vide !
	// end Q3

	// Q4
	pte32_t *ptb = (pte32_t*)0x601000;
	// end Q4

	// Q5
	for(int i=0;i<1024;i++) {
	 	pg_set_entry(&ptb[i], PG_KRN|PG_RW, i);
	}
	memset((void*)pgd, 0, PAGE_SIZE);
	pg_set_entry(&pgd[0], PG_KRN|PG_RW, page_nr(ptb));
 	// uint32_t cr0 = get_cr0(); // enable paging
	// set_cr0(cr0|CR0_PG);
	// end Q5

	// Q6
	// debug("PTB[1] = %p\n", ptb[1].raw);
	// res Q6
	/* IDT event
	 . int    #14
	 . error  0x0
	 . cs:eip 0x8:0x304206
	 . ss:esp 0x0:0x303a52
	 . eflags 0x2

	- GPR
	eax     : 0x601004
	ecx     : 0x0
	edx     : 0x0
	ebx     : 0x60
	esp     : 0x301fac
	ebp     : 0x301fe8
	esi     : 0x2bfc2
	edi     : 0x2bfc3

	Exception: Page fault
	#PF details: p:0 wr:0 us:0 id:0 addr 0x601004
	cr0 = 0x80000011
	cr4 = 0x0

	-= Stack Trace =-
	0x30305e
	0x303020
	0x8c85
	0x72bf0000
	fatal exception !

	#PF car l'adresse virtuelle n'est pas mappée
	*/
	// solution:
	pte32_t *ptb2 = (pte32_t*)0x602000;
	for(int i=0;i<1024;i++) {
		pg_set_entry(&ptb2[i], PG_KRN|PG_RW, i+1024);
	}
	pg_set_entry(&pgd[1], PG_KRN|PG_RW, page_nr(ptb2));

	uint32_t cr0 = get_cr0(); // enable paging
	set_cr0(cr0|CR0_PG);
	// debug("PTB[1] = %p\n", ptb[1].raw);
	// end Q6

	// Q7
	pte32_t  *ptb3    = (pte32_t*)0x603000;
	uint32_t *target  = (uint32_t*)0xc0000000;
	int      pgd_idx = pd32_idx(target);
	int      ptb_idx = pt32_idx(target);
	debug("%d %d\n", pgd_idx, ptb_idx);
	/**/
	memset((void*)ptb3, 0, PAGE_SIZE);
	pg_set_entry(&ptb3[ptb_idx], PG_KRN|PG_RW, page_nr(pgd));
	pg_set_entry(&pgd[pgd_idx], PG_KRN|PG_RW, page_nr(ptb3));
	/**/
	debug("PGD[0] = 0x%x | target = 0x%x\n", (unsigned int) pgd[0].raw, (unsigned int) *target);

	/*uint32_t cr0 = get_cr0(); // enable paging
	set_cr0(cr0|CR0_PG);*/
	// end Q7

	// Q8
	char *v1 = (char*)0x700000; // 7 memoire partagee
	char *v2 = (char*)0x7ff000;
	ptb_idx = pt32_idx(v1);
	pg_set_entry(&ptb2[ptb_idx], PG_KRN|PG_RW, 2);
	ptb_idx = pt32_idx(v2);
	pg_set_entry(&ptb2[ptb_idx], PG_KRN|PG_RW, 2);
	debug("%p = %s | %p = %s\n", v1, v1, v2, v2);
	// uint32_t cr0 = get_cr0(); // enable paging
	// set_cr0(cr0|CR0_PG);
	// end Q8

	// Q9
    *target = 0; 
    //invalidate(target); // vidage des caches 
	// end Q9


}
