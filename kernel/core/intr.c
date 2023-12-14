/* GPLv2 (c) Airbus */
#include <intr.h>

extern info_t *info;
extern void idt_trampoline();
extern void handler_IT_timer();
static int_desc_t IDT[IDT_NR_DESC];

void handler_sys_counter()
{
   // récupération de la pgd courante
	cr3_reg_t cr3 = {.raw = get_cr3()};
   // switch vers la pgd kernel
   pde32_t * kern_pgd = (pde32_t * )PGD_PROCS_BASE;
   set_cr3((uint32_t)kern_pgd);
   uint32_t *counter = (uint32_t *) ((long unsigned int) SHARED_INT_ADDR_KRN);
   debug("counter value : %d\n", *counter);
   set_cr3((uint32_t) cr3.raw);
}

// syscalls
static uint32_t syscall_table[NR_SYS_CALLS];

void associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address)
{
   syscall_table[syscall_value] = handler_address;
}

void init_syscall_table()
{
   for (int i = 0; i < NR_SYS_CALLS; i++)
   {
      syscall_table[i] = 0;
   }
}

/*
Fonction récupérant la valeur du syscall et appelant la fonction associée
*/
void syscall_isr()
{
   asm volatile(
       "leave ; pusha        \n"
       "mov %esp, %eax      \n"
       "call syscall_finder \n"
       "popa ; iret");
}

/*
Appel système utilisé par le processus écrivant dans la console la valeur du compteur
*/
void __regparm__(1) syscall_finder(int_ctx_t *ctx)
{
   uint32_t syscall_value = ctx->gpr.eax.raw;
   debug("==================\nSYSCALL eax = 0x%x\n==================\n", syscall_value);
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
         ctx->nr.raw, ctx->err.raw, ctx->cs.raw, ctx->eip.raw, ctx->ss.raw, ctx->esp.raw, ctx->eflags.raw, ctx->gpr.eax.raw, ctx->gpr.ecx.raw, ctx->gpr.edx.raw, ctx->gpr.ebx.raw, ctx->gpr.esp.raw, ctx->gpr.ebp.raw, ctx->gpr.esi.raw, ctx->gpr.edi.raw);

   uint8_t vector = ctx->nr.blow;

   if (vector < NR_EXCP)
      excp_hdlr(ctx);
   else
      debug("ignore IRQ %d\n", vector);
}
