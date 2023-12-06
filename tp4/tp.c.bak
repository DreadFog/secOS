/* GPLv2 (c) Airbus */
#include <debug.h>
#include <cr.h>
#include <pagemem.h>
#include <segmem.h>

#define ten_msb(x) (x >> 22)
#define ten_midsb(x)  ((x << 10) >> 22)
#define twelve_lsb(x) (x & 0xFFFFFF)

void tp()
{
    debug("tp\n");
  // Q1
    uint32_t cr3 = {.raw = get_cr3()};
  
    printf("CR3: 0x%x\n", cr3.addr);

  // Q2
  // cf https://en.wikipedia.org/wiki/Control_register#Control_registers_in_Intel_x86_series
  uint32_t pde_addr = 0x600000;
  cr3 =  (cr3_reg_t) ((raw32_t) pde_addr);
  set_cr3(cr3);
    printf("CR3: 0x%x\n", get_cr3());
  
  // Q3
  // activation de la pagination
    uint32_t cr0 = get_cr0();
  cr0 |= 1 << 31;
  // set_cr0(cr0);   // plante car on n'a initialisé que le premier niveau d'indirection de la pagination?  

  // Q4
  uint32_t pte_addr =  0x601000;  // size(PGD = 0x1000)
  pte32_t *pte = (pte32_t*) pte_addr; // PDE at 0x600000

  // Q5
  /*
  En-têtes de programme :
                Type           Décalage Adr. vir.  Adr.phys.  T.Fich. T.Mém.  Fan Alignement
                LOAD           0x000094 0x00300000 0x00300000 0x0000c 0x0000c RWE 0x4
                LOAD           0x000000 0x00300010 0x00300010 0x00000 0x02000 RW  0x10
                LOAD           0x0000b0 0x00302010 0x00302010 0x02900 0x03530 RWE 0x20
  */
  pde32_t *pde = (pde32_t*) pde_addr; // PDE at 0x600000
  // on a une superbe PDE a l'adresse demandee
  uint32_t add0 = 0x00300000;
  uint32_t add1 = 0x00300010;
  uint32_t add2 = 0x00302010;

  pde[ten_msb(add0)].addr = pte_addr >> 10;
  pte[ten_midsb(add0)].addr = add0 >> 12;
  pte[ten_midsb(add1)].addr = add1 >> 12;
  pte[ten_midsb(add2)].addr = add2 >> 12;
  /*
  pte[ten_midsb(add0)].addr = add0 - twelve_lsb(add0);
  pte[ten_midsb(add1)].addr = add1 - twelve_lsb(add1);
  pte[ten_midsb(add2)].addr = add2 - twelve_lsb(add2);
  */

  // inch ?
  set_cr0(cr0);



/*
Notes:
PGT_d[t].(base+p) = a_phy

ex: 
pour ad = 0x00300123
d = 0 -> le max c'est 0xFFF
t = 0x300 -> le max c'est 0x3FF
p = 0x123 -> le max c'est 0xFFF

dont on utilise PGD[0] qui pointe sur une table des pages
à cette table des pages à l'index t, on met (0<<22)|(0x300<<12), càd (d<<10+12) | (t <<10)

penser à memset les zones à zéro c'est pas bête!

*/




}