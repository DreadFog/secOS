#include <segmem.h>


// Publicly accessible functions

/*
init_gdt()
Initialise des descripteurs de code et de données en
ring 0 et en ring 3.
*/
void init_gdt();

/*
Function to switch to ring 3:
- creation of a process
- association of the given address to the rip of the process
- usage of the newly created process stack
*/
void call_ring_3(void *ring3_code);