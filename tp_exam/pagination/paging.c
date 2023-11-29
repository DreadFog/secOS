#include <pagination/paging.h>

void init_paging(void) {
    // retrieval of CR3
    cr3_reg_t cr3 = {.raw = get_cr3()};
    pde32_t **page_directory_list = (pde32_t **) PGD_PROCS_BASE;
    // Identity mapping of the kernel space
    pde32_t *pgd_kernel = page_directory_list[0];
    pte32_t *ptb_kernel = (pte32_t *) PGD_PROCS_BASE + ONE_MB; // 5 Mb

    set_cr3((uint32_t) pgd_kernel);
    for(int i=0;i<1024;i++) {
	 	pg_set_entry(&ptb_kernel[i], PG_KRN|PG_RW, i);
	}
	memset((void*)pgd_kernel, 0, PAGE_SIZE);
	pg_set_entry(&pgd_kernel[0], PG_KRN|PG_RW, page_nr(ptb_kernel));
}