#ifndef COMMAND_H
#define COMMAND_H

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (unsigned int *)0x3f10001c
#define PM_WDOG (unsigned int *)0x3f100024

/// Commands
int help(void);
int reboot(void);
int hello(void);
int invalid_command(const char *);
int lshw(void);
int ls(void);
int cat(void);
int run_loader(void);
int time_out(void);
int async_read(void);
int pmalloc_command(void);
int smalloc_command(void);
int pfree_command(void);
int exec(void);
int getpid_command(void);

struct command {
  const char *name;
  const char *help;
  int (*func)(void); // Funtion pointer of behavior
};

/// commands struct array which contain commands
extern struct command commands[];

#endif // COMMAND_H
