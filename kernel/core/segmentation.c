/**
 * 		segmentation.c
 * File defining functions used for the segmentation.
 *  
*/

#include <segmentation.h>

seg_desc_t GDT[8];
tss_t TSS_list[1 + MAX_PROCS]; // 0 for kernel, 1 for process 1, 2 for process 2, etc...

/**
 * init_gdt()
 * Initialize the GDT and associated register GDTR as well as
 * all control registers.
*/
void init_gdt()
{
   gdt_reg_t gdtr;

   GDT[0].raw = 0ULL;

   c0_dsc(&GDT[c0_idx]);
   d0_dsc(&GDT[d0_idx]);
   c3_dsc(&GDT[c3_idx]);
   d3_dsc(&GDT[d3_idx]);

   gdtr.desc = GDT;
   gdtr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr);

   set_cs(c0_sel);

   set_ss(d0_sel);
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);

}

/**
 * init_tss()
 * Initialize the (barebones) TSS for the processses with esp and ss and adds it in the GDT.
*/
void init_tss()
{
   // TODO: Should we initialize the TSS for the kernel or not?

   // set basic TSS structure for ring 0 switching
   TSS_list[1].s0.esp = get_ebp();
   TSS_list[1].s0.ss = d0_sel;
   TSS_list[2].s0.esp = get_ebp();
   TSS_list[2].s0.ss = d0_sel;

   tss_dsc(&GDT[ts_krn_idx], (offset_t)&(TSS_list[0]));
   tss_dsc(&GDT[ts_proc1_idx], (offset_t)&(TSS_list[1]));
   tss_dsc(&GDT[ts_proc2_idx], (offset_t)&(TSS_list[2]));
}

/**
 * call_ring_3_bak()
 * Allows to call code from ring3.
 * Parameter:
 *    - void *ring3_code, the address of the code in ring 3.
 * As a norm, switching to ring 3 will be done using a new process associated with the given code.
*/
void call_ring_3_bak(void *ring3_code)
{
   // TODO: create a process list, process 0 will be the kernel, the next available process
   // will be associated to the provided ring3 code.
  
   // while(1) { debug("TODO: Stack not retrieved from process 1.\n"); }
   uint32_t ustack = PG_4M_SIZE * 4 + PG_4M_SIZE; // 20 Mb, soit fin de la pagination user associée au processus 1

   // Test: Change GDT to the first process GDT
   pde32_t *pgd_proc_1 = (pde32_t *)(PGD_PROCS_BASE + PAGE_SIZE);
   set_cr3((uint32_t) pgd_proc_1);
   //while(1){};
   // Set the selectors and the TSS for ring 3
   set_ds(d3_sel);
   set_es(d3_sel);
   set_fs(d3_sel);
   set_gs(d3_sel);
   set_tr(ts_proc1_sel);
   asm volatile(
       "push %0 \n" // ss
       "push %1 \n" // esp pour du ring 3 !
       "pushf   \n" // eflags
       "push %2 \n" // cs
       "push %3 \n" // eip
       "iret" ::
           "i"(d3_sel),
       "m"(ustack),
       "i"(c3_sel),
       "r"(ring3_code));
}
