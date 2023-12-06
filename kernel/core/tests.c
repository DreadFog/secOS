#include <debug.h>  
#include <intr.h>
#include <segmentation.h>
#include <paging.h>

// counter print syscall
void sys_counter(uint32_t *counter) {
  	// counter est une @virtuelle ring 3
	// asm volatile("int $0x80\t\n");
	counter = counter;
}

void __attribute__((section(".user"))) user1() {
	/* uint32_t *cpt = (uint32_t*)SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_1
	while (1) { (*cpt)++; } */
	debug("user1\n");
}

void __attribute__((section(".user"))) user2() {
	/* uint32_t *cpt = (uint32_t*)SHARED_MEM_ADDR; // TODO : SHARED_MEM_ADDR_USER_2
	while (1) {
		sys_counter(cpt);
	}
   */
  debug("user2\n");
}

void default_configuration() {
	init_gdt();
    init_paging();
	init_syscall_table();
}

void some_kernel_function() {
    debug("some_kernel_function\n");
}

void userland_test_call_ring_0() {
	// asm volatile("mov %eax, %cr0"); // crashes, as we are in ring 3
	debug("Before ring 0 call\n");
    some_kernel_function();
	debug("After ring 0 call, should not appear!\n");
	// asm volatile("mov %eax, %cr0"); //back to ring 3, so it crashes
	while(1){};
}

void test_call_ring0_from_ring3(void)
{
    default_configuration();
    call_ring_3(userland_test_call_ring_0);
}

/*
Tests syscalls
*/

void userland_test_syscall() {
	debug("userland_test_syscall\n");
	// asm volatile("mov %eax, %cr0"); // crashes, as we are in ring 3
	// debug("Before syscall\n");
	asm volatile ("int $0x80"::"S"(1)); // Test syscall 1
	// debug("After syscall\n");
	// asm volatile("mov %eax, %cr0"); //back to ring 3, so it crashes
	while(1){};
}

void test_syscall_function() {
	// asm volatile("mov %eax, %cr0"); / works, as we are in ring 0
	//debug("test_syscall_function\n");
	// debug("I am executed in ring 0\n");
}

void test_syscall(void)
{
	debug("====================================\n");
	debug("Test syscall\n");
	debug("====================================\n");
    default_configuration();
    associate_syscall_handler(1, (uint32_t)test_syscall_function);
	// debug by retrieving the current RIP and the address of userland_test_syscall
	debug("userland_test_syscall = 0x%x\n", (unsigned int)userland_test_syscall);
	call_ring_3(userland_test_syscall);
}