#include <cr.h>
#include <pagemem.h>
#include <string.h>
#include <debug.h>
#define PGD_PROCS_BASE 0x3d0900 // adresse 4 MB, début de la table de PGD
#define ONE_MB 0xf4240
#define SHARED_MEM_ADDR 0x0 // TODO

/*
Function that will initialize paging in the OS.
    - PGD pre-allocation for the kernel + 2 user processes (more processes would simply require more memory allocated to the kernel)
    - PTB pre-allocation for the kernel + 2 user processes
    - identity-mapping for the kernel + processes
    - Reservation of a shared-memory chunk that will not be identity-mapped
*/
void init_paging();