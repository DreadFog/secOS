/**
 * 		paging.c
 * Functions used for paging, for now only used to initialize the paging.
 * 
*/

#include <paging.h>

/**
 * init_paging()
 * Initialize the paging and memory translation: kernel and processus PGD & PTBs.
 * The paging is so that we have:
 *  - PGD pre-allocation for the kernel + MAX_PROCS user processes
        (more processes would simply require more memory allocated to the kernel)
 *  - PTB pre-allocation for the kernel + MAX_PROCS user processes
 *  - identity-mapping for the kernel + processes
 *  - Reservation of a shared-memory chunk that will not be identity-mapped
*/
void init_paging()
{
    // Initialize the kernel PGD and PTBs
    pde32_t *pgd_kernel = (pde32_t *)PGD_PROCS_BASE;
    pte32_t *curr_ptb = (pte32_t *)(PGD_PROCS_BASE + ONE_MB); // 5 Mb
    memset((void *)pgd_kernel, 0, PAGE_SIZE);
    set_cr3((uint32_t)pgd_kernel);

    // Note: we also identity map the shared memory for the kernel,
    // as it will have to print the value of the shared int

    unsigned long required_page_count = (KERNEL_STACK_BASE + PG_4M_SIZE) / PAGE_SIZE; // 4Mb for the shared memory
    unsigned long kernel_ptb_count = required_page_count / PDE32_PER_PD;
    unsigned long j = 0;
    while (j < kernel_ptb_count)
    {
        // Page table filling
        for (int i = 0; i < (int)PTE32_PER_PT; i++)
        {
            pg_set_entry(&curr_ptb[i], PG_KRN | PG_RW, i + j * 1024);
        }
        pg_set_entry(&pgd_kernel[j], PG_KRN | PG_RW, page_nr(curr_ptb));
        curr_ptb += PAGE_SIZE;
        j++;
    }

    // mapping for the processes
    // Processes will still have kernel pages memory-mapped in identity mapping
    // Then, 12 to 16 Mb (virtual address) will be mapped to their respective data
    pde32_t *curr_proc_pgd;
    pte32_t *curr_proc_ptb;
    required_page_count = (PROCS_PHY_MEM_SIZE*MAX_PROCS) / PAGE_SIZE;
    unsigned long twelve_first_mbs = (KERNEL_STACK_BASE) / PAGE_SIZE; // 4M for the shared memory
    kernel_ptb_count = twelve_first_mbs / PDE32_PER_PD;
    unsigned long required_ptb_count = required_page_count / PDE32_PER_PD;

    // initialize the two processes PGDs and PTBs
    for (int k = 0; k < MAX_PROCS; k++)
    {
        curr_proc_pgd = (pde32_t *)(PGD_PROCS_BASE + (k + 1) * PAGE_SIZE); // first PGD was for kernel
        curr_proc_ptb = (pte32_t *)(PGD_PROCS_BASE + (k + 2) * ONE_MB);    // PGDs, then kernel PTB, then processes PTBs hence k+2 * ONE_MB

        // initialize the PGD and the related PTBs
        memset((void *)curr_proc_pgd, 0, PAGE_SIZE);

        // kernel pages de 0 à 12
        j = 0;
        while (j < kernel_ptb_count)
        {
            for (int i = 0; i < (int)PTE32_PER_PT; i++)
            {
                pg_set_entry(&curr_proc_ptb[i], PG_KRN | PG_RW, i + j * 1024);
            }
            pg_set_entry(&curr_proc_pgd[j], PG_KRN | PG_RW, page_nr(curr_proc_ptb));
            curr_proc_ptb += PAGE_SIZE;
            j++;
        }
        
        // shared memory: only one entry in the page table
        uint32_t shared_int_addr = SHARED_INT_ADDR_KRN + (k+1)*ONE_MB; // that way, proc1 will have 14 Mb, and proc2 15 Mb
        pg_set_entry(&curr_proc_pgd[j], PG_USR | PG_RW, page_nr(curr_proc_ptb));
        int shared_mem_ptb_idx = pt32_idx(shared_int_addr);
        uint32_t int_krn_addr = ((uint32_t) SHARED_INT_ADDR_KRN) / ((uint32_t) PAGE_SIZE);
        pg_set_entry(&curr_proc_ptb[shared_mem_ptb_idx], PG_USR | PG_RW, int_krn_addr);
        curr_proc_ptb += PAGE_SIZE;
        j++;

        // User pages: proc1 16-20Mb, proc2 20-24Mb
        // Only shared memory is not identity mapped
        while (j < kernel_ptb_count + required_ptb_count)
        {
            for (int i = 0; i < (int)PTE32_PER_PT; i++)
            {
                // debug("Curr max mapped address : %lx\n", (i + j*1024)*PAGE_SIZE);
                pg_set_entry(&curr_proc_ptb[i], PG_USR | PG_RW, i + j * 1024);
            }
            pg_set_entry(&curr_proc_pgd[j], PG_USR | PG_RW, page_nr(curr_proc_ptb));
            curr_proc_ptb += PAGE_SIZE;
            j++;
        }
    }

    // enable paging
    uint32_t cr0 = get_cr0();
    set_cr0(cr0 | CR0_PG);
}
