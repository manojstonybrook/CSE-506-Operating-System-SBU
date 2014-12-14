// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/dwarf.h>
#include <kern/kdebug.h>
#include <kern/dwarf_api.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
        { "backtrace", "", backtrace },
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
       // Your code here.
        
        int rdi, rsi, rdx;
        
        asm __volatile("mov %%edx,%0" : "=r" (rdx)::);
        asm __volatile("mov %%esi,%0" : "=r" (rsi)::);
        asm __volatile("mov %%edi,%0" : "=r" (rdi)::);
        
       struct Ripdebuginfo *info = 0;
       int args;
       int *d, i;
       cprintf("Stack backtrace: \n");
       uint64_t *prev_rbp;
       uint64_t *data = (uint64_t *)read_rbp();
       uint64_t rbp, rip;
       
       read_rip(rip);
       rbp = read_rbp(); 
       cprintf(" rbp %16.0x rip %16.0x\n", rbp, rip);
       debuginfo_rip(rip, info);
        cprintf("      %s:%d: %s+%16.0x  args:%x ", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);
        prev_rbp = (uint64_t *)rbp;
        d = (int *)(*prev_rbp);
       	
        cprintf("%16.0x %16.0x %16.0x\n", rdi, rsi, rdx);      

       while(*prev_rbp)
       {
        
        rbp = (*prev_rbp);
        rip = (*(prev_rbp+1));
        cprintf(" rbp %16.0x rip %16.0x\n", rbp, rip);
        debuginfo_rip(rip, info);
        if(info->rip_fn_narg)
	  {
             cprintf("      %s:%d: %s+%16.0x  args:%x ", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);
             for(i = 1; i <= info->rip_fn_narg; i++) 
                 cprintf("%16.0x ", *(d-i));      
             cprintf("\n");

	  }
        else
          cprintf("      %s:%d: %s+%16.0x  args:%x\n", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);

        prev_rbp = (uint64_t *)rbp;
        d = (int *)(*prev_rbp);
       }
     
     return 0;
}


int
backtrace(int argc, char **argv, struct Trapframe *tf)
{
       // Your code here.
       // Your code here.
       struct Ripdebuginfo *info = 0;
       int args;
       int *d,i;
       cprintf("Stack backtrace: \n");
       uint64_t *prev_rbp;
       uint64_t rbp, rip;
       
       read_rip(rip);
       rbp = read_rbp();
 
       cprintf(" rbp %16.0x rip %16.0x\n", rbp, rip);

       debuginfo_rip(rip, info);
       cprintf("      %s:%d: %s+%16.0x  args:%x  ", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);

        prev_rbp = (uint64_t *)rbp;
       	d = (int *)(*prev_rbp);        
        // It is not a right way we are not writing this function arguments but previous one
        for(i = 1; i <= info->rip_fn_narg; i++) 
           cprintf("%16.0x ", *(d-i));      
        cprintf("\n");


       while(*prev_rbp)
       {
        
        rbp = (*prev_rbp);
        rip = (*(prev_rbp+1));
        cprintf(" rbp %16.0x rip %16.0x\n", rbp, rip);
        debuginfo_rip(rip, info);
        if(info->rip_fn_narg)
          {
           cprintf("      %s:%d: %s+%16.0x  args:%x ", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);
           
	   for(i = 1; i <= info->rip_fn_narg; i++) 
             cprintf("%16.0x ", *(d-i));      
           cprintf("\n");

          }
        else
          cprintf("      %s:%d: %s+%16.0x  args:%x \n", info->rip_file, info->rip_line, info->rip_fn_name, rip-info->rip_fn_addr, info->rip_fn_narg);

        prev_rbp = (uint64_t *)rbp;
        d = (int *)(*prev_rbp);
       }
     
     return 0;
}


/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");


	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
