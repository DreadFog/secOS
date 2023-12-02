/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
extern info_t *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;
extern uint32_t mbh;
// extern uint32_t stack;

void tp()
{
   debug("kernel mem [0x%p - 0x%p]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);
   /* debug("mbh mem [0x%p - 0x%p]\n", &mbh, &stack);
   debug("mbh mem [0x%p - 0x%p]\n", &stack, &__kernel_start__); */

   multiboot_memory_map_t *entry = (multiboot_memory_map_t *)info->mbi->mmap_addr;
   while ((uint32_t)entry < (info->mbi->mmap_addr + info->mbi->mmap_length))
   {
      // print "[start - end] type" for each entry
      debug("[0x%x - 0x%x] %d\n", (unsigned int)entry->addr, (unsigned int)(entry->addr + entry->len - 1), entry->type);
      entry++;
   }

   int *ptr_in_available_mem;
   ptr_in_available_mem = (int *)0x0;
   debug("Available mem (0x0): before: 0x%x ", *ptr_in_available_mem); // read
   *ptr_in_available_mem = 0xaaaaaaaa;                                 // write
   debug("after: 0x%x\n", *ptr_in_available_mem);                      // check

   int *ptr_in_reserved_mem;
   ptr_in_reserved_mem = (int *)0xf0000;
   debug("Reserved mem (at: 0xf0000):  before: 0x%x ", *ptr_in_reserved_mem); // read
   *ptr_in_reserved_mem = 0xaaaaaaaa;                                         // write
   debug("after: 0x%x\n", *ptr_in_reserved_mem);                              // check

   // Écriture dans une zone réservée empêchée par la table globale de descripteurs qui a déjà été initialisée.
}
