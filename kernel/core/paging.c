#include <paging.h>

void init_paging() {
    // retrieval of CR3
    cr3_reg_t cr3 = {.raw = get_cr3()};
    //pde32_t (*page_directory_list)[NB_PGD_ENTRIES] = (pde32_t (*)[NB_PGD_ENTRIES]) PGD_PROCS_BASE;// technically 1024 PGDs with each NB_PGD_ENTRIES entries
    pde32_t **page_directory_list = (pde32_t **) PGD_PROCS_BASE; 

    // Initialize the kernel PGD and PTBs
    pde32_t *pgd_kernel = page_directory_list[0];
    pte32_t **ptb_kernel_list = (pte32_t **) PGD_PROCS_BASE + ONE_MB; // 5 Mb
	memset((void*)pgd_kernel, 0, PAGE_SIZE);
    set_cr3((uint32_t) pgd_kernel);

    for(int j = 0; j < NB_PGD_ENTRIES; j++) {
        for(int i=0;i<NB_PTB_ENTRIES;i++) {
	 	    pg_set_entry((ptb_kernel_list + j*PAGE_SIZE)[j], PG_KRN|PG_RW, i);
	    }
	    pg_set_entry(&pgd_kernel[j], PG_KRN|PG_RW, page_nr(ptb_kernel_list[j]));
    }

    pde32_t *curr_proc_pgd;
    pte32_t **curr_proc_ptb_list;

    // initialize the two processes PGDs and PTBs
    for (int k = 0; k < MAX_PROCS; k++) { 
        //initialize the PGD and the related PTBs
        curr_proc_pgd = (pde32_t *) (PGD_PROCS_BASE +(k+1)*PAGE_SIZE); // first PDE was for kernel
        curr_proc_ptb_list = (pte32_t **) PGD_PROCS_BASE + (k+1)*ONE_MB;
        memset((void*)curr_proc_pgd, 0, PAGE_SIZE);

        // PGD and PTB of each process is initialized in ring 3
        for(int j = 0; j < NB_PGD_ENTRIES; j++) {
            for(int i=0;i<NB_PTB_ENTRIES;i++) {
	 	        pg_set_entry((curr_proc_ptb_list + j*PAGE_SIZE)[j], PG_USR|PG_RW, i);
	        }
	        pg_set_entry(&curr_proc_pgd[j], PG_USR|PG_RW, page_nr(curr_proc_ptb_list[j]));
        }
    }
    set_cr3(cr3);
}
