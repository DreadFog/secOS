/* GPLv2 (c) Airbus */
#include <intr.h>
#include <debug.h>
#include <info.h>

extern info_t *info;
extern void idt_trampoline();
static int_desc_t IDT[IDT_NR_DESC];

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
   // TODO : c'est vraiment gdt_krn_seg_sel(1) ?
   // Gestion de l'interruption irq0
   int_desc(&tab_int_desc[32], gdt_krn_seg_sel(1), (offset_t)&irq0_handler);
   // Gestion de l'interruption irq80
   int_desc(&tab_int_desc[0x80], gdt_krn_seg_sel(1), (offset_t)&irq80_handler);
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

// Handler interruption horloge
void irq0_handler()
{
   asm volatile("pusha \t\n");
   /* TODO: (doit succéder aux process)
      - switch tâche 1 <-> tâche 2
      - savoir si on a interrompu le noyau ou une tâche utilisateur
   */
   // Rendre la main à tp()
   asm volatile("iret \t\n");
}

void irq80_handler()
{
   asm volatile("pusha \t\n");
   uint32_t *counter;
   asm volatile("mov 8(%%ebp), %0" : "=r"(counter));
   debug("counter value : %d", *counter);
   // Rendre la main à tp()
   asm volatile("iret \t\n");
}

/*
Function that retrieves the idrt and overwrites the current handler associated
with the given syscall_value
*/
void associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address)
{
   int_desc_t *dsc;
   idt_reg_t idtr;
   get_idtr(idtr);

   // configure the handler
   dsc = &idtr.desc[syscall_value];
   dsc->offset_1 = (uint16_t)((uint32_t)handler_address);
   dsc->offset_2 = (uint16_t)(((uint32_t)handler_address) >> 16);

   // allow call from ring 3
   dsc->dpl = 3;

}

/*
Dans tp5, ces deux fonctions sont utilisées. C'est l'adresse de syscall_isr qui est associée au 
syscall 48.
Faut-il l'associer à tous les syscalls puis faire de la disjonction de cas selon le contexte dans 
syscall_handler?Après tout, le `pusha` a empilé la valeur du syscall...
*/
void syscall_isr() {
   asm volatile (
      "leave ; pusha        \n"
      "mov %esp, %eax      \n"
      "call syscall_handler \n"
      "popa ; iret"
      );
}

void __regparm__(1) syscall_handler(int_ctx_t *ctx) {
   debug("SYSCALL eax = 0x%x\n", (unsigned int) ctx->gpr.eax.raw);
   // Q4
   debug("print syscall: %s", (char *)ctx->gpr.esi.raw);
   // end Q4
}
