/* GPLv2 (c) Airbus */
/**
 * 		intr.c
 * File giving basic functions to add and manage handlers and syscalls.
 * 
*/
#include <intr.h>

extern info_t *info;
extern void idt_trampoline();
extern void handler_IT_timer();
static int_desc_t IDT[IDT_NR_DESC];

/**
 * handler_sys_counter()
 * Handler of the syscall that is responsible for displaying the counter value.
 * Note: The "real" syscall would have simply been a wrapper for the printf function,
 * but handling its arguments in a syscall would have taken more time.
*/
void handler_sys_counter()
{
   // Retrieval of current PGD
	cr3_reg_t cr3 = {.raw = get_cr3()};

   // switch to kernel PGD
   pde32_t * kern_pgd = (pde32_t * )PGD_PROCS_BASE;
   set_cr3((uint32_t)kern_pgd);

   // Display of the current value
   uint32_t *counter = (uint32_t *) ((long unsigned int) SHARED_INT_ADDR_KRN);
   debug("counter value : %d\n", *counter);
   set_cr3((uint32_t) cr3.raw);
}

/**
 * handler_sys_terminate()
 * Handler halting the kernel.
 * QEMU stays idle.
*/
void handler_sys_terminate()  {
   debug("Halting QEMU ...\n");
   halt();
}

// syscalls table
static uint32_t syscall_table[NR_SYS_CALLS];

/**
 * associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address)
 * Associates the handler to the given syscall value. 
 * Parameters:
 *    - syscall_value, the value of the syscall redirecting to the handler
 *    - handler_address, the address of the function meant to be the handler
*/
void associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address)
{
   syscall_table[syscall_value] = handler_address;
}

/**
 * init_syscall_table()
 * Initialize to null all the syscall table. 
*/
void init_syscall_table()
{
   for (int i = 0; i < NR_SYS_CALLS; i++)
   {
      syscall_table[i] = 0;
   }
}

/**
 * syscall_isr()
 * Function responsible for handling the context saving before calling a handler.void syscall_isr()
*/
void syscall_isr()
{
   asm volatile(
       "leave ; pusha        \n"
       "mov %esp, %eax      \n"
       "call syscall_finder \n"
       "popa ; iret");
}

/**
 * Function responsible for calling the appropriate handler for a syscall
 * syscall_finder(int_ctx_t *ctx)
 * int_ctx_t *ctx -> used to retrieve the syscall value in EAX
*/ 
 void syscall_finder(int_ctx_t *ctx)
 {
   uint32_t syscall_value = ctx->gpr.eax.raw;
   // debug("==================\nSYSCALL eax = 0x%x\n==================\n", syscall_value);
   if (syscall_value < NR_SYS_CALLS)
   {
      uint32_t handler_address = syscall_table[syscall_value];
      if (handler_address != 0)
      {
         asm volatile("mov %0, %%eax\n"
                      "call *%%eax\n"
                      :
                      : "r"(handler_address));
      }
      else
      {
         debug("No handler for syscall 0x%x\n", syscall_value);
      }
   }
   else
   {
      debug("No handler for syscall 0x%x\n", syscall_value);
   }
}

/**
 * intr_init()
 * Initialize the interruption table and the associated register intr.
*/
void intr_init()
{
   idt_reg_t idtr;
   offset_t isr;
   size_t i;

   isr = (offset_t)idt_trampoline;

   /* re-use default grub GDT code descriptor */
   for (i = 0; i < IDT_NR_DESC; i++, isr += IDT_ISR_ALGN)
      int_desc(&IDT[i], gdt_krn_seg_sel(1), isr);

   idtr.desc = IDT;
   idtr.limit = sizeof(IDT) - 1;

   int_desc_t *tab_int_desc = idtr.desc;
   // Gestion de l'interruption irq0
   int_desc(&tab_int_desc[32], gdt_krn_seg_sel(1), (offset_t)&handler_IT_timer);
   // Gestion de l'interruption irq80
   int_desc(&tab_int_desc[0x80], gdt_krn_seg_sel(1), (offset_t)&syscall_isr);
   tab_int_desc[0x80].dpl = 3;
   set_idtr(idtr);
}

void __regparm__(1) intr_hdlr(int_ctx_t *ctx)
{
   debug("\nIDT event\n"
         " . int    #%d\n"
         " . error  0x%x\n"
         " . cs:eip 0x%x:0x%x\n"
         " . ss:esp 0x%x:0x%x\n"
         " . eflags 0x%x\n"
         "\n- GPR\n"
         "eax     : 0x%x\n"
         "ecx     : 0x%x\n"
         "edx     : 0x%x\n"
         "ebx     : 0x%x\n"
         "esp     : 0x%x\n"
         "ebp     : 0x%x\n"
         "esi     : 0x%x\n"
         "edi     : 0x%x\n",
         ctx->nr.raw, ctx->err.raw,
         ctx->cs.raw, ctx->eip.raw,
         ctx->ss.raw, ctx->esp.raw,
         ctx->eflags.raw, ctx->gpr.eax.raw,
         ctx->gpr.ecx.raw, ctx->gpr.edx.raw,
         ctx->gpr.ebx.raw, ctx->gpr.esp.raw,
         ctx->gpr.ebp.raw, ctx->gpr.esi.raw, ctx->gpr.edi.raw);

   uint8_t vector = ctx->nr.blow;

   if (vector < NR_EXCP)
      excp_hdlr(ctx);
   else
      debug("ignore IRQ %d\n", vector);
}
