#ifndef PAGING_H_
#define PAGING_H_

#include <cr.h>
#include <pagemem.h>
#include <string.h>
#include <debug.h>
#define PGD_PROCS_BASE PG_4M_SIZE // adresse 4 MB, début de la table de PGD 1 << 25
#define PROCS_STACK_HEAP_BASE PGD_PROCS_BASE * 2 // 8 Mb
#define KERNEL_STACK_BASE PGD_PROCS_BASE * 3 // 12 Mb
#define ONE_MB PG_4M_SIZE / 4 // 1 << 23
#define SHARED_MEM_ADDR PROCS_STACK_HEAP_BASE // 12 Mb
#define PROCS_PHY_MEM_BASE PG_4M_SIZE * 4 // 16 Mb, 20 Mb, etc...
#define PROCS_PHY_MEM_SIZE PG_4M_SIZE
#define MAX_PROCS 2

/*
Function that will initialize paging in the OS.
    - PGD pre-allocation for the kernel + MAX_PROCS user processes (more processes would simply require more memory allocated to the kernel)
    - PTB pre-allocation for the kernel + MAX_PROCS user processes
    - identity-mapping for the kernel + processes
    - Reservation of a shared-memory chunk that will not be identity-mapped
*/
void init_paging();
#endif