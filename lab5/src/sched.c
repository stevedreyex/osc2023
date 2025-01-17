#include "sched.h"
#include "entry.h"
#include "printf.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

struct list_head runqueue[MAX_PRIO];

void preempt_disable(void)
{
    current->preempt_count++;
}

void preempt_enable(void)
{
    current->preempt_count--;
}

void _schedule(void)
{
    preempt_disable();
    
    int next, c;
    struct task_struct *p;
    while (1) {
        c = -1;
        next = 0;
        for (int i = 0;i < NR_TASKS;i++) {
            p = task[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        for (int i = 0;i < NR_TASKS;i++) {
            p = task[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }
    
    // printf("[_schedule] next scheduled - pid = %d\n", next);
    switch_to(task[next], next);

    preempt_enable();
}

void schedule(void) 
{
    current->counter = 0;
    _schedule();
}

void switch_to(struct task_struct *next, int index) 
{
    if (current == next)
        return;

    //printf("[switch_to] Tasks state before cotext switch:\n");
    //dumpTasksState();
    // printf("[switch_to] context switch! next scheduled - pid = %d\n", index);

    struct task_struct *prev = current;
    current = next;
    cpu_switch_to(prev, next);
}


void schedule_tail(void)
{
    preempt_enable();
}

void timer_tick()
{
    --current->counter;
}

void task_preemption()
{
    if (current->counter>0 || current->preempt_count >0) {
        return;
    }
    current->counter=0;

    disable_irq();
    _schedule();
    enable_irq();
}

void exit_process(void) {
    preempt_disable();
    current->state = TASK_ZOMBIE;

    // if user stack allocated, free stack.
    if (current->stack) { 
        kfree((void *)current->stack);
    }

    preempt_enable();
    schedule();
}

void kill_zombies(void) {
    // reclaim threads marked as DEAD
    //
    for (int i = 0;i < NR_TASKS;i++) {
        if (task[i]->state == TASK_ZOMBIE) {
            // kfree(task[i]);
            // task[i] = NULL;
        }
    }
}

void dumpTasksState() {
    printf("=========Tasks state=========\n");
    for (int i = 0;i < NR_TASKS;i++) {
        if(task[i] == NULL) continue;
        printf("Task %d: ", task[i]->pid);

        switch(task[i]->state) {
        case TASK_RUNNING:
            printf("RUNNING");
            break;
        case TASK_WAITING:
            printf("WAITING");
            break;
        case TASK_ZOMBIE: 
            printf("ZOMBIE");
            break;
        default:
            printf("Unknown State");
        }
        printf("\n");
    } 
}