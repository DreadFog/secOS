#include <users.h>

void user1() {
	uint32_t *cpt = (uint32_t *) ((long unsigned int) SHARED_INT_PROC1_ADDR);
	while (1) {
        (*cpt)++;
        asm volatile("int $0x80\t\n"::"S"(1));
        if (*cpt == 2) {
            while (1) {}
        }
    }
}

void user2() {
	/* uint32_t *cpt = (uint32_t*)SHARED_INT_PROC2_ADDR;
	while (1) {
		sys_counter();
	}
   */
    // counter print syscall
    // void sys_counter() {
    //     asm volatile("int $0x80\t\n"::"S"(1));
    // }
    debug("user2\n");
    while(1){};
}