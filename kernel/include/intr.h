/* GPLv2 (c) Airbus */
#ifndef __INTR_H__
#define __INTR_H__

#include <types.h>
#include <segmem.h>
#include <gpr.h>
#include <excp.h>
#include <debug.h>
#include <info.h>
#include <paging.h>
#include <asm.h>

#define IDT_NR_DESC                   256
#define IDT_ISR_ALGN                  16

#define BIOS_VIDEO_INTERRUPT          0x10
#define BIOS_DISK_INTERRUPT           0x13
#define BIOS_MISC_INTERRUPT           0x15
#define BIOS_KBD_INTERRUPT            0x16
#define BIOS_BOOT_INTERRUPT           0x19

/*
** BIOS services related to MISC_INTERRUPT
*/

/* AX values */
#define BIOS_GET_SMAP                 0xe820
#define BIOS_SMAP_ID                  0x534d4150
#define BIOS_SMAP_ERROR               0x86

#define BIOS_GET_EXT_MEM_32           0xe881
#define BIOS_GET_EXT_MEM              0xe801

#define BIOS_DISABLE_A20              0x2400
#define BIOS_ENABLE_A20               0x2401
#define BIOS_STATUS_A20               0x2402
#define BIOS_SUPPORT_A20              0x2403

/* AH values */

#define BIOS_GET_BIG_MEM              0x8a
#define BIOS_OLD_GET_EXT_MEM          0x88

typedef struct cpu_context
{
   raw32_t         nr;
   excp_err_code_t err;
   raw32_t         eip;
   raw32_t         cs;
   eflags_reg_t    eflags;

   /* if pvl level change */
   raw32_t         esp;
   raw32_t         ss;

} __attribute__((packed)) cpu_ctx_t;


typedef struct interrupt_context
{
   gpr_ctx_t gpr;
   cpu_ctx_t;

} __attribute__((packed)) int_ctx_t;

typedef void (*isr_t)(int_ctx_t*);

#define int_desc(_dsc_, _cs_, _isr_)                                    \
   ({                                                                   \
      raw32_t addr = {.raw = _isr_};                                    \
      (_dsc_)->raw      = addr.wlow;                                    \
      (_dsc_)->selector = _cs_;                                         \
      (_dsc_)->type     = SEG_DESC_SYS_INTR_GATE_32;                    \
      (_dsc_)->offset_2 = addr.whigh;                                   \
      (_dsc_)->p        = 1;                                            \
   })

/**
 * intr_init()
 * Initialize the interruption table and the associated register intr.
*/
void intr_init();

void intr_hdlr(int_ctx_t*) __regparm__(1);

// External function
void handler_IT_timer();

/**
 * handler_sys_counter()
 * Handler of the syscall that is responsible for displaying the counter value.
 * Note: The "real" syscall would have simply been a wrapper for the printf function,
 * but handling its arguments  would have taken more time.
*/
void handler_sys_counter();

/**
 * handler_sys_terminate()
 * Handler halting the kernel.
 * QEMU stays idle.
*/
void handler_sys_terminate();

// everything related to syscalls
#define NR_SYS_CALLS 2
/**
 * associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address)
 * Associates the handler to the given syscall value. 
 * Parameters:
 *    - syscall_value, the value of the syscall redirecting to the handler
 *    - handler_address, the address of the function meant to be the handler
*/
void associate_syscall_handler(uint32_t syscall_value, uint32_t handler_address);

/**
 * init_syscall_table()
 * Initialize to null all the syscall table. 
*/
void init_syscall_table();
#endif
