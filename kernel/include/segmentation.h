#ifndef __SEGMENTATION_H__
#define __SEGMENTATION_H__
#include <segmem.h>
#include <debug.h>
#include <paging.h>
#define c0_idx 1
#define d0_idx 2
#define c3_idx 3
#define d3_idx 4
#define ts_krn_idx 5
#define ts_proc1_idx 6
#define ts_proc2_idx 7


#define c0_sel gdt_krn_seg_sel(c0_idx)
#define d0_sel gdt_krn_seg_sel(d0_idx)
#define c3_sel gdt_usr_seg_sel(c3_idx)
#define d3_sel gdt_usr_seg_sel(d3_idx)
#define ts_krn_sel gdt_krn_seg_sel(ts_krn_idx)
#define ts_proc1_sel gdt_usr_seg_sel(ts_proc1_idx)
#define ts_proc2_sel gdt_usr_seg_sel(ts_proc2_idx)

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

// Publicly accessible functions

/**
 * init_gdt()
 * Initialize the GDT and associated register GDTR as well as
 * all control registers.
*/
void init_gdt();

/**
 * init_tss()
 * Initialize the (barebones) TSS for the processses with esp and ss and adds it in the GDT.
*/
void init_tss();

#endif