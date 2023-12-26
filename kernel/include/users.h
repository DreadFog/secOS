#include <types.h>
#include <paging.h>

/**
 * user1() represents the main function of process 1.
 * We associates the (physical) memory section .user1.
*/
void __attribute__((section(".user1"))) user1();

/**
 * user2() represents the main function of process 2.
 * We associates the (physical) memory section  .user2.
*/
void __attribute__((section(".user2"))) user2();