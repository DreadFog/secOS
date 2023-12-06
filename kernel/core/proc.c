#include <proc.h>
#include <debug.h>

process_t *processes[MAX_PROCS];
process_t storing_table[MAX_PROCS];
int current_process_id;
int current_process_index;
extern void ctx_sw(uint32_t *old_ctx, uint32_t *new_ctx);
extern void process_wrapper();

// Function to initialize the process table, is supposed to be called before any fork
void init_process_table(void *root_program)
{
    for (int i = 0; i < MAX_PROCS; i++)
    {
        processes[i] = NULL;
        storing_table[i].is_available = 1;
    }
    current_process_index = 0;
    current_process_id = 0;
    process_t *k = add_process("kernel", 0, NULL);
    k->state = RUNNING; // this one is currently running
    // retrieve the current stack top
    // TODO: retrieve the TSS address and handle it
    debug("TODO: retrieve the TSS address and handle it\n");
    processes[k->pid] = k;
}

void add_to_pointer_list(int pid) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i] == NULL) {
            processes[i] = &storing_table[pid];
            break;
        }
    }
}

/*
retrieve the first available pid in the table
or return -1 if there is no available pid
*/
int preempt_pid()
{
    for (int i = 0; i < MAX_PROCS; i++)
    {
        // find the first available process entry in the table
        if (storing_table[i].is_available)
        {
            storing_table[i].is_available = 0;
            return i;
        }
    }
    debug("No available pid\n");
    return -1;
}

process_t *add_process(const char *name, pid_t ppid, void *function)
{ // call the wrapper add_process and put to ebx the address of the function
    int pid = preempt_pid();
    if (pid == -1)
    {
        debug("No available pid\n");
        return NULL; // no available pid
    }
    process_t *process = &storing_table[pid];
    process->name = name;
    process->ppid = ppid;
    process->pid = pid;
    if (pid == 0) { // kernel
        process->stack = KERNEL_STACK_BASE;
    }
    else {
        process->stack = PROCS_STACK_HEAP_BASE + PAGE_SIZE * pid;
    }
    process->state = READY;
    return process;
}

void remove_process(process_t *process)
{
    processes[process->pid] = NULL;
    process->is_available = 1;
}

process_t *get_process_by_pid(pid_t pid)
{
    return processes[pid];
}

int exec_fork(const char *name, void *function)
{
    process_t *process = add_process(name, current_process_id, function);
    if (process == NULL)
    {
        return -1;
    }
    processes[process->pid] = process;
    return process->pid;
}

void print_processes()
{
    debug("\nPrinting processes\n");
    debug("==========\n");
    for (int i = 0; i < MAX_PROCS; i++)
    {
        if (processes[i] != NULL)
        {
            debug("Process %s, pid %d, ppid %d\n", processes[i]->name, processes[i]->pid, processes[i]->ppid);
            debug("Stack pointer %p\n", processes[i]->sp);
            debug("Registers: "); debug("TODO !\n");
            debug("==========\n");
        }
    }
}

void scheduler()
{ // scheduler that alternates between ready processes
    debug("Scheduler called while process of pid %d is active\n", current_process_id);
    process_t *current_process = processes[current_process_index];
    if (current_process != NULL)
    { // could be null when we call schedule just after the main process has been suspended
        current_process->state = READY;
    }
    // find the next process to run
    while (1)
    {
        current_process_index = (current_process_index + 1) % MAX_PROCS;
        if (processes[current_process_index] != NULL && processes[current_process_index]->state == READY)
        {
            processes[current_process_index]->state = RUNNING;
            current_process_id = processes[current_process_index]->pid;
            //ctx_sw(current_process->ctx, processes[current_process_index]->ctx);
            debug("Switching from process %d to process %d\n", current_process->pid, processes[current_process_index]->pid);
            debug("TODO\n");
            break;
        }
    }
    // if we still didn't find any process, we keep the current one
}

void stop_current_process()
{
    process_t *current_process = processes[current_process_index];
    remove_process(current_process);
    scheduler();
}

void block_current_process()
{
    process_t *current_process = processes[current_process_index];
    current_process->state = BLOCKED;
    scheduler();
}

void unblock_process(pid_t pid)
{
    storing_table[pid].state = READY;
    add_to_pointer_list(pid);   
}

int get_current_process_id()
{
    return current_process_id;
}