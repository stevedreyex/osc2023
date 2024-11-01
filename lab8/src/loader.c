#include "loader.h"
#include "interrupt.h"
#include "thread.h"

static void *program_loc;
/**************************************************************************
 * This function will run the program at the specific location.
 * And this function sould handle following tasks.
 * 1. Save kernel status.
 * 2. EL1 -> EL0.
 * 3. Start user program.
 * ***********************************************************************/
int run_program(void *loc) {
  core_timer_enable();
  mini_uart_interrupt_enable();
  asm volatile("mov x1,	0x0;\r\n" // Enable CPU interrupt
               "msr spsr_el1, 	x1;\r\n"
               "mov x1,	0x40000;\r\n" // Set user stack to 0x60000
               "msr sp_el0,	x1;\r\n"
               "mov x1,	%[loc];\r\n"
               "msr elr_el1,	x1;\r\n" // Set the target address
               "mov x0,	%[loc];\r\n"     // For recalculat offset
               "eret"
               :
               : [loc] "r"(loc) // input location
  );

  // Note: at this stage, this function will not return.
  return 0;
}

/**********************************************************************
 * Setup the private function location which will run at EL0
 *********************************************************************/
int setup_program_loc(void *loc) {
  program_loc = loc;
  return 0;
}

/**********************************************************************
 * The implement of syscall EXEC
 * which will setup sp_el0 and the elr_el1
 *********************************************************************/
void sys_run_program(void) {
  core_timer_enable();
  Thread *t = get_current();
  void *sp_el0 = (void *)t->sp_el0;
  asm volatile("mov x2,	0x0;\r\n" // Enable CPU interrupt
               "msr spsr_el1, 	x2;\r\n"
               "msr sp_el0,	%[sp];\r\n"
               "mov x1,	%[loc];\r\n"
               "msr elr_el1,	%[loc];\r\n" // Set the target address
               "eret"
               :
               : [loc] "r"(program_loc) // input location
                 ,
                 [sp] "r"(sp_el0));
  return;
}

void *getProgramLo() { return program_loc; }
