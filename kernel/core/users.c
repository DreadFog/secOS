/**
 * 		users.c
 * File defining the code of the 2 processes used to
 * reprent users/processes using the kernel.
 *  
*/

#include <users.h>

/**
 * user1()
 * Represents the main function of process 1.
 * Increments a counter in shared space memory.
*/
void user1() {
	uint32_t *cpt = (uint32_t *) ((long unsigned int) SHARED_INT_PROC1_ADDR);
	while (1) {
        (*cpt)++;
        asm volatile("int $0x80\t\n"::"S"(1));
    }
}

/**
 * user2() 
 * Represents the main function of process 2.
 * Reads and prints (syscall) the counter in shared space memory.
*/
void user2() {
    // counter print syscall
    debug("user2\n");
    void sys_counter() {
        asm volatile("int $0x80\t\n"::"S"(1));
    }
	//uint32_t *cpt = (uint32_t *) ((long unsigned int) SHARED_INT_PROC2_ADDR);
	while (1) {
		sys_counter();
	}
}
