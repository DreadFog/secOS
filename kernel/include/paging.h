#ifndef PAGING_H_
#define PAGING_H_

#include <cr.h>
#include <pagemem.h>
#include <string.h>
#include <debug.h>
#define PGD_PROCS_BASE 0x3d0900 // adresse 4 MB, début de la table de PGD
#define ONE_MB 0xf4240
#define SHARED_MEM_ADDR 0xf42400 // @ Physique du counter à 16 Mb
#define NB_PGD_ENTRIES 1024
#define NB_PTB_ENTRIES 1024
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