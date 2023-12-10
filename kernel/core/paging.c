#include <paging.h>

void init_paging()
{
    // Initialize the kernel PGD and PTBs
    pde32_t *pgd_kernel = (pde32_t *)PGD_PROCS_BASE;
    pte32_t *curr_ptb = (pte32_t *)PGD_PROCS_BASE + ONE_MB; // 5 Mb
    memset((void *)pgd_kernel, 0, PAGE_SIZE);
    set_cr3((uint32_t)pgd_kernel);

    unsigned long required_page_count = KERNEL_STACK_BASE / PAGE_SIZE;
    unsigned long kernel_ptb_count = required_page_count / PDE32_PER_PD + 1;
    // debug("Required: %d\n", kernel_ptb_count);
    // while(1) {};
    unsigned long j = 0;
    while (j < kernel_ptb_count)
    {
        // remplissage de la table de page
        for (int i = 0; i < (int)PTE32_PER_PT; i++)
        {
            //debug("Curr max mapped address : %lx\n", (i + j*1024)*PAGE_SIZE);
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
    uint32_t curr_proc_phi_mem_base;
    required_page_count = PROCS_PHY_MEM_SIZE / PAGE_SIZE;
    unsigned long required_ptb_count = required_page_count / PDE32_PER_PD + 1;
    // initialize the two processes PGDs and PTBs
    for (int k = 0; k < MAX_PROCS; k++)
    {
        curr_proc_phi_mem_base = PROCS_PHY_MEM_BASE + k * PROCS_PHY_MEM_SIZE;
        curr_proc_pgd = (pde32_t *)(PGD_PROCS_BASE + (k + 1) * PAGE_SIZE); // first PGD was for kernel
        curr_proc_ptb = (pte32_t *)(PGD_PROCS_BASE + (k + 2) * ONE_MB);    // PGDs, then kernel PTB, then processes PTBs hence k+2 * ONE_MB

        // initialize the PGD and the related PTBs
        memset((void *)curr_proc_pgd, 0, PAGE_SIZE);

        // kernel pages
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

        // User pages: 16-20 Mb for process 1, 20-24 for process 2 are mapped to 12-16 Mb virt addresses.
        while (j < kernel_ptb_count + required_ptb_count)
        {
            for (int i = 0; i < (int)PTE32_PER_PT; i++)
            {
                debug("Curr max mapped address : %lx\n", curr_proc_phi_mem_base + (i + j*1024)*PAGE_SIZE);
                pg_set_entry(&curr_proc_ptb[i], PG_USR | PG_RW, curr_proc_phi_mem_base / PAGE_SIZE + i + j * 1024);
            }
            pg_set_entry(&curr_proc_pgd[j + kernel_ptb_count], PG_USR | PG_RW, page_nr(curr_proc_ptb));
            j++;
            curr_proc_ptb += PAGE_SIZE;
        }
    }
    // enable paging
    uint32_t cr0 = get_cr0();
    set_cr0(cr0 | CR0_PG);
}
