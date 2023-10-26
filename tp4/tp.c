/* GPLv2 (c) Airbus */
#include <debug.h>
#include <cr.h>
#include <pagemem.h>
#include <segmem.h>

void tp()
{
	debug("tp\n");
	cr3_reg_t cr3 = (cr3_reg_t) ((raw32_t) get_cr(3));
	
	printf("CR3: 0x%x\n", cr3.addr);
	uint32_t cr0 = get_cr0();
  printf("CR0: 0x%x\n", cr0);
  pde32_t pde[PDE32_PER_PD];
  pte32_t pte[PTE32_PER_PT];
  uint32_t wanted_addr = 0x600000;
  pde[pd32_idx(wanted_addr)].addr = pte;
  pte[pt32_idx(wanted_addr)].addr = wanted_addr;

 cr3.addr = (uint32_t) pde;
  set_cr3(cr3);
  /* cr0 |= 1 << 31;
  set_cr0(cr0); */
}
