#include <tests.h>

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
    // default_configuration();
    // call_ring_3(userland_test_call_ring_0);
}

/*
Tests syscalls
*/

void userland_test_syscall() {
	// debug("userland_test_syscall\n");
	// asm volatile("mov %eax, %cr0"); // crashes, as we are in ring 3
	// debug("Before syscall\n");
	asm volatile ("int $0x80"::"S"(1)); // Test syscall 1
	// debug("After syscall\n");
	// asm volatile("mov %eax, %cr0"); //back to ring 3, so it crashes
	while(1){};
}

void test_syscall_function() {
	// asm volatile("mov %eax, %cr0"); / works, as we are in ring 0
	debug("test_syscall_function\n");
	debug("I am executed in ring 0\n");
	print_processes();
}

// void test_syscall(void)
// {
// 	debug("====================================\n");
// 	debug("Test syscall\n");
// 	debug("====================================\n");
//     default_configuration();
//     associate_syscall_handler(1, (uint32_t)handler_sys_counter);
//     associate_syscall_handler(2, (uint32_t)handler_sys_terminate);
// 	add_process("user1", 0, user1);
// 	add_process("user2", 0, user2);
// 	call_ring_3_pid_1();
// }