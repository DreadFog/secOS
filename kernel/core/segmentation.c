#include <segmentation.h>
#define c0_idx 1
#define d0_idx 2
#define c3_idx 3
#define d3_idx 4
#define ts_idx 5

#define c0_sel gdt_krn_seg_sel(c0_idx)
#define d0_sel gdt_krn_seg_sel(d0_idx)
#define c3_sel gdt_usr_seg_sel(c3_idx)
#define d3_sel gdt_usr_seg_sel(d3_idx)
#define ts_sel gdt_krn_seg_sel(ts_idx)

seg_desc_t GDT[6];
tss_t TSS;

#define gdt_flat_dsc(_dSc_, _pVl_, _tYp_) \
   ({                                     \
      (_dSc_)->raw = 0;                   \
      (_dSc_)->limit_1 = 0xffff;          \
      (_dSc_)->limit_2 = 0xf;             \
      (_dSc_)->type = _tYp_;              \
      (_dSc_)->dpl = _pVl_;               \
      (_dSc_)->d = 1;                     \
      (_dSc_)->g = 1;                     \
      (_dSc_)->s = 1;                     \
      (_dSc_)->p = 1;                     \
   })

#define tss_dsc(_dSc_, _tSs_)                  \
   ({                                          \
      raw32_t addr = {.raw = _tSs_};           \
      (_dSc_)->raw = sizeof(tss_t);            \
      (_dSc_)->base_1 = addr.wlow;             \
      (_dSc_)->base_2 = addr._whigh.blow;      \
      (_dSc_)->base_3 = addr._whigh.bhigh;     \
      (_dSc_)->type = SEG_DESC_SYS_TSS_AVL_32; \
      (_dSc_)->p = 1;                          \
   })

#define c0_dsc(_d) gdt_flat_dsc(_d, 0, SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d, 0, SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d, 3, SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d, 3, SEG_DESC_DATA_RW)

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

void call_ring_3(void *ring3_code)
{
   // norm: switching to ring 3 will be done using a new process associated with the given code
   /*
   TODO: create a process list, process 0 will be the kernel, the next available process
   will be associated to the provided ring3 code.
   */

   // while(1) { debug("TODO: Stack not retrieved from process 1.\n"); }
   uint32_t ustack = 0x600000;

   // Set the selectors and the TSS for ring 3
   set_ds(d3_sel);
   set_es(d3_sel);
   set_fs(d3_sel);
   set_gs(d3_sel);
   TSS.s0.esp = get_ebp();
   TSS.s0.ss = d0_sel;
   tss_dsc(&GDT[ts_idx], (offset_t)&TSS);
   set_tr(ts_sel);
   // Test: Change GDT to the first process GDT
   /* uint32_t gdt_proc_1 = PGD_PROCS_BASE + PAGE_SIZE;
   set_cr3(gdt_proc_1); */
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