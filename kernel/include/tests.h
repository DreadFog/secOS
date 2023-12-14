#include <debug.h>  
#include <intr.h>
#include <segmentation.h>
#include <paging.h>
#include <proc.h>
#include <time.h>
#include <asm.h>
#include <users.h>
/*
    * tests.h
    *
    *  Unit tests of different functionalities of the kernel.
*/

/*
    * test_call_ring0_from_ring3
    *   Description:
    *       Tests the ability to call a ring0 function from ring3.
    *   Expected behaviour:
    *      The kernel should crash, as ring0 code is reserved to the kernel.
*/
void test_call_ring0_from_ring3(void);

/*
    * test_syscall
    *   Description:
    *       Tests the ability to perform a syscall from ring3.
    *   Expected behaviour:
    *      The kernel should not crash, and the syscall should be executed.
*/
void test_syscall(void);