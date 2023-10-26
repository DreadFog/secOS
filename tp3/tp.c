/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
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
void userland()
{
   debug("\n\n\n\n\n\n\n\n\n\n\n\nCalled Userland\n\n\n\n\n\n\n\n\n\n\n");
   asm volatile("mov %eax, %cr0");
   debug("Should not be printed bc crash... \n");
   while (1)
   {
   }
   
   return;
}

void tp()
{

   seg_desc_t my_gdt[7];
   my_gdt[0].raw = 0ULL;
   my_gdt[1].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
   my_gdt[1].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
   my_gdt[1].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
   my_gdt[1].type = 11;        // Code,RX //:4;      /* segment type */
   my_gdt[1].s = 1;            //: 1;      /* descriptor type */
   my_gdt[1].dpl = 0;          // ring0    //:2;      /* descriptor privilege level */
   my_gdt[1].p = 1;            //: 1;      /* segment present flag */
   my_gdt[1].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
   my_gdt[1].avl = 1;          //: 1;      /* available for fun and profit */
   my_gdt[1].l = 0;            // 32bits     //:1;      /* longmode */
   my_gdt[1].d = 1;            //: 1;      /* default length, depend on seg type */
   my_gdt[1].g = 1;            //: 1;      /* granularity */
   my_gdt[1].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */

   my_gdt[2].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
   my_gdt[2].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
   my_gdt[2].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
   my_gdt[2].type = 3;         // data,RW //:4;      /* segment type */
   my_gdt[2].s = 1;            //: 1;      /* descriptor type */
   my_gdt[2].dpl = 0;          // ring0    //:2;      /* descriptor privilege level */
   my_gdt[2].p = 1;            //: 1;      /* segment present flag */
   my_gdt[2].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
   my_gdt[2].avl = 1;          //: 1;      /* available for fun and profit */
   my_gdt[2].l = 0;            // 32 bits   //:1;      /* longmode */
   my_gdt[2].d = 1;            //: 1;      /* default length, depend on seg type */
   my_gdt[2].g = 1;            //: 1;      /* granularity */
   my_gdt[2].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */

   my_gdt[3].limit_1 = 0x20;  //: 16;     /* bits 00-15 of the segment limit */
   my_gdt[3].base_1 = 0x0000; //: 16;     /* bits 00-15 of the base address */
   my_gdt[3].base_2 = 0x60;   //: 8;      /* bits 16-23 of the base address */
   my_gdt[3].type = 3;        // data,RW //:4;      /* segment type */
   my_gdt[3].s = 1;           //: 1;      /* descriptor type */
   my_gdt[3].dpl = 0;         // ring0    //:2;      /* descriptor privilege level */
   my_gdt[3].p = 1;           //: 1;      /* segment present flag */
   my_gdt[3].limit_2 = 0xf;   //: 4;      /* bits 16-19 of the segment limit */
   my_gdt[3].avl = 1;         //: 1;      /* available for fun and profit */
   my_gdt[3].l = 0;           // 32 bits   //:1;      /* longmode */
   my_gdt[3].d = 1;           //: 1;      /* default length, depend on seg type */
   my_gdt[3].g = 1;           //: 1;      /* granularity */
   my_gdt[3].base_3 = 0x00;   //: 8;      /* bits 24-31 of the base address */

   // code segment ring 3
   my_gdt[4].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
   my_gdt[4].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
   my_gdt[4].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
   my_gdt[4].type = 11;        // data,RX //:4;      /* segment type */
   my_gdt[4].s = 1;            //: 1;      /* descriptor type */
   my_gdt[4].dpl = 3;          // ring3    //:2;      /* descriptor privilege level */
   my_gdt[4].p = 1;            //: 1;      /* segment present flag */
   my_gdt[4].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
   my_gdt[4].avl = 1;          //: 1;      /* available for fun and profit */
   my_gdt[4].l = 0;            // 32 bits   //:1;      /* longmode */
   my_gdt[4].d = 1;            //: 1;      /* default length, depend on seg type */
   my_gdt[4].g = 1;            //: 1;      /* granularity */
   my_gdt[4].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */

   // data segment, ring 3
   my_gdt[5].limit_1 = 0xffff; //: 16;     /* bits 00-15 of the segment limit */
   my_gdt[5].base_1 = 0x0000;  //: 16;     /* bits 00-15 of the base address */
   my_gdt[5].base_2 = 0x00;    //: 8;      /* bits 16-23 of the base address */
   my_gdt[5].type = 3;         // data,RW //:4;      /* segment type */
   my_gdt[5].s = 1;            //: 1;      /* descriptor type */
   my_gdt[5].dpl = 3;          // ring3    //:2;      /* descriptor privilege level */
   my_gdt[5].p = 1;            //: 1;      /* segment present flag */
   my_gdt[5].limit_2 = 0xf;    //: 4;      /* bits 16-19 of the segment limit */
   my_gdt[5].avl = 1;          //: 1;      /* available for fun and profit */
   my_gdt[5].l = 0;            // 32 bits   //:1;      /* longmode */
   my_gdt[5].d = 1;            //: 1;      /* default length, depend on seg type */
   my_gdt[5].g = 1;            //: 1;      /* granularity */
   my_gdt[5].base_3 = 0x00;    //: 8;      /* bits 24-31 of the base address */
   gdt_reg_t my_gdtr;
   my_gdtr.addr = (long unsigned int)my_gdt;
   my_gdtr.limit = sizeof(my_gdt) - 1;
   set_gdtr(my_gdtr);
   tss_t TSS;
   TSS.s0.esp = get_ebp();
   TSS.s0.ss = gdt_usr_seg_sel(5);
   tss_dsc(&my_gdt[6], (offset_t)&TSS);
   set_tr(gdt_krn_seg_sel(6));
   asm volatile(
       "push %0    \n" // ss
       "push %%ebp \n" // esp
       "pushf      \n" // eflags
       "push %1    \n" // cs
       "push %2    \n" // eip
       // end Q2
       // Q3
       "iret" ::
           "i"(gdt_usr_seg_sel(5)),
       "i"(gdt_usr_seg_sel(4)),
       "r"(&userland));
}
