#include <users.h>

void user1() {
	uint32_t *cpt = (uint32_t *) ((long unsigned int) SHARED_INT_PROC1_ADDR);
	while (1) {
        (*cpt)++;
        if (*cpt == 2) {
            asm volatile("int $0x80\t\n"::"S"(0));;
        }
    }
}

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