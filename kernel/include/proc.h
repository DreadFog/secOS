#ifndef __PROC_H__
#define __PROC_H__
#include <paging.h>

typedef uint32_t pid_t;

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
    uint32_t *sp; // stack pointer
    PROCESS_STATE state;
    pid_t pid;
    pid_t ppid;
    uint8_t is_available:1;
} process_t;

void init_process_table(void *root_program);
process_t * add_process(const char *name, pid_t ppid, void *function);
void remove_process_by_pid(pid_t pid);
process_t *get_process_by_pid(pid_t pid);
int exec_fork(const char *name, void *function);
void print_processes();
void scheduler();
void stop_current_process();
int get_current_process_id();

#endif