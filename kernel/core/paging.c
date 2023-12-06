#include <paging.h>

void init_paging() {
    // Initialize the kernel PGD and PTBs
    pde32_t *pgd_kernel = (pde32_t *)PGD_PROCS_BASE;
    pte32_t *curr_ptb = (pte32_t *) PGD_PROCS_BASE + ONE_MB; // 5 Mb
	memset((void*)pgd_kernel, 0, PAGE_SIZE);
    set_cr3((uint32_t) pgd_kernel);

    int required_page_count = KERNEL_STACK_BASE / PAGE_SIZE;
    int required_ptb_count = required_page_count / PDE32_PER_PD + 1;
    debug("Required: %d\n", required_ptb_count);
    // while(1) {};
    int j = 0;
    while (j < required_ptb_count) {
        //remplissage de la table de page
        for(int i=0; i < (int) PTE32_PER_PT; i++) {
            debug("Curr max mapped address : %lx\n", (i + j*1024)*PAGE_SIZE);
	 	    pg_set_entry(&curr_ptb[i], PG_KRN|PG_RW, i + j*1024);
	    }
	    pg_set_entry(&pgd_kernel[j], PG_KRN|PG_RW, page_nr(curr_ptb));
        curr_ptb += PAGE_SIZE;
        j++;
    }

    // mapping for the processes
    // Problem: I think that PTBs and PGDs in ring 3 need 
    pde32_t *curr_proc_pgd;
    pte32_t *curr_proc_ptb;
    uint32_t curr_proc_phi_mem_base;
    required_page_count = PROCS_PHY_MEM_SIZE / PAGE_SIZE;
    required_ptb_count = required_page_count / PDE32_PER_PD + 1;
    // initialize the two processes PGDs and PTBs
    for (int k = 0; k < MAX_PROCS; k++) {
        curr_proc_phi_mem_base = PROCS_PHY_MEM_BASE + k*PROCS_PHY_MEM_SIZE;
        curr_proc_pgd = (pde32_t *) (PGD_PROCS_BASE +(k+1)*PAGE_SIZE); // first PDE was for kernel
        //initialize the PGD and the related PTBs
        memset((void*)curr_proc_pgd, 0, PAGE_SIZE);
        // PGD and PTB of each process is initialized in ring 3
        int j = 0;
        while(j < required_ptb_count) {
            curr_proc_ptb = (pte32_t *) (PGD_PROCS_BASE + (k+2)*ONE_MB * j*PAGE_SIZE); // PGDs, then kernel PTB, then processes PTBs hence k+2 * ONE_MB
            for(int i=0;i< (int) PTE32_PER_PT;i++) {
                debug("Curr max mapped address : %lx\n", curr_proc_phi_mem_base + (i + j*1024)*PAGE_SIZE);
	 	        pg_set_entry(&curr_proc_ptb[j], PG_USR|PG_RW, curr_proc_phi_mem_base / PAGE_SIZE + i + j*1024);
	        }
	        pg_set_entry(&curr_proc_pgd[j], PG_USR|PG_RW, page_nr(curr_proc_ptb));
            j++;
        }
    }
    // enable paging
	uint32_t cr0 = get_cr0();
	set_cr0(cr0|CR0_PG);
}
