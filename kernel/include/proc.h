#ifndef __PROC_H__
#define __PROC_H__
#include <segmentation.h>
typedef uint32_t pid_t;

#define CTX_SIZE 5
typedef enum { EBX, ESP, EBP, ESI, EDI } registry_t;


typedef enum {
    RUNNING,
    READY,
    BLOCKED,
    SLEEPING,
} PROCESS_STATE;

typedef struct resource_t {
    void *resource;
    struct resource_t *next;
} RESOURCE;

typedef struct process_t {
    const char *name;
    uint32_t stack;
    uint32_t ctx[CTX_SIZE];
    uint32_t *sp; // stack pointer
    PROCESS_STATE state;
    pid_t pid;
    pid_t ppid;
    uint8_t is_available:1;
    pde32_t *pgd;
    void *entrypoint;
} process_t;

/**
 * init_process_table
 * Initialize the process table.
 * One should take care to call it before doing any fork.
 * Parameters:
 *  - root_program, the root adress of the program belonging to the process.
*/
void init_process_table(void *root_program);

/**
 * add_process
 * Call the wrapper add_process and put to ebx the address of the function
 * Parameters:
 *      - name, the name of the process to add
 *      - ppid, the parent pid of the process
 *      - function, the adress to the program of the process 
*/
process_t * add_process(const char *name, pid_t ppid, void *function);

/**
 * remove_process
 * Deletes the process from the process table
 * Parameter:
 *      - process, the process to remove
*/
void remove_process(process_t *process);

/**
 * get_process_by_pid
 * Fetchs the process associatid to a pid in the process table
 * Parameter:
 *       - pid, the pid of the process to fetch
*/
process_t *get_process_by_pid(pid_t pid);

/**
 * exec_fork
 * [Unused] function, would be used if we needed to fork processes;
*/
int exec_fork(const char *name, void *function);

/**
 * print_processes
 * Prints the processes context.
*/
void print_processes();

/**
 * scheduler
 * The scheduler alternates between ready processes in a
 * ring architecture.
*/
void scheduler();

/**
 * stop_current_process
 * Stops the current process
*/
void stop_current_process();

/**
 * get_current_process_id
 * Fetchs the id of the current process running.
*/
int get_current_process_id();


/**
 * Function to switch to ring 3:
 * - creation of a process
 * - association of the given address to the rip of the process
 * - usage of the newly created process stack
 * Switching to ring 3 will be done using a new process associated with the given code
*/
void call_ring_3_pid_1();

#endif