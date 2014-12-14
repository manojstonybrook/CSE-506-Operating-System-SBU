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
#include <kern/trap.h>

#include <kern/pmap.h>
#define CMDBUF_SIZE	180	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "display the rbp, rip and arguments", mon_backtrace },
        { "backtrace_lab1", "bactrce from current place", backtrace_lab1 },
        { "showmapping", "shows physical page mapping for virtal address give address range in hexadecimal", showmapping},
        {"change_permission", "give address and permission both in hexadecimal", change_permission},
        //{ "backtrace_lab1", "", backtrace_lab1 },
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
       	if(info->rip_fn_narg)
          cprintf("%16.0x %16.0x %16.0x\n", rdi, rsi, rdx);      
        else 
          cprintf("\n");
       	
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
backtrace_lab1(int argc, char **argv, struct Trapframe *tf)
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

uint64_t str_to_int(char *s)
{
 uint64_t i=0, offset=1;
 uint64_t res=0;

 while(s[i]!='\0')
    i++;
   i--;
  //cprintf("length %d\n", i);
  while(i >= 2)
   {
    if('0' <= s[i] && s[i]<= '9')
      res = res + ((int)(s[i] - '0'))*offset;
    else if('a' <= s[i] && s[i]<= 'f')
      res = res + ((int)(s[i]-'a') + 10) * offset;
    else if('A' <= s[i] && s[i]<= 'F')
      res = res + ((int)(s[i]-'a') + 10) * offset;
      
    offset *= 16;    
    //cprintf("data %d %d\n", res, i);
    i--;
   }
 //
 return res;
}

void check_permission(uintptr_t perm)
{
  if(perm & PTE_P)
   cprintf(" PTE_P");

  if(perm & PTE_W)
   cprintf(" PTE_W");

  if(perm & PTE_U)
   cprintf(" PTE_U");
  
  if(perm & PTE_PWT)
   cprintf(" PTE_PWT");

 if(perm & PTE_PCD)
   cprintf(" PTE_PCD");

 if(perm & PTE_A)
   cprintf(" PTE_A");

if(perm & PTE_D)
   cprintf(" PTE_D");

if(perm & PTE_PS)
   cprintf(" PTE_PS");

if(perm & PTE_MBZ)
   cprintf(" PTE_PBZ");

 cprintf("\n");

}



int
showmapping(int argc, char **argv, struct Trapframe *tf)
{
   if(argc < 3)
   {
    cprintf("\ngive the start and end range\n");
    return 0;
   }
   uintptr_t i, perm=0;
 
   uintptr_t start = str_to_int(argv[1]);
   uintptr_t end = str_to_int(argv[2]);
   
   cprintf("\n virtual address        |   physical address   |  permissions \n");
   for(i = start; i < end; i = i + PGSIZE)
   {
     pte_t *pte = pml4e_walk(boot_pml4e, (void *) i, 0); 
     perm = *pte & 0xfff;
     if(pte!=NULL)
      {
       cprintf("\n %16.0x           %16.0x          ", i, *pte & ~0xfff);
       check_permission(perm);
  
      }
     else
       cprintf("\n %16.0x           Invalid          ", i);
     
    }
    
  return 0;
}

int
change_permission(int argc, char **argv, struct Trapframe *tf)
{
 
  if(argc < 3)
   {
    cprintf("\ngive the address and permission\n");
    return 0;
   }
   uintptr_t i;
 
   uintptr_t start = str_to_int(argv[1]);
   uintptr_t perm = str_to_int(argv[2]);
  
     pte_t *pte = pml4e_walk(boot_pml4e, (void *) start, 0); 
     if(pte!=NULL)
     {
     cprintf("\n Before Permission %16.0x", *pte);
     *pte = *pte & ~0xfff;
     *pte = *pte | perm;
     cprintf("\n After Permission %16.0x\n", *pte);
     }
     else
      cprintf("\npage not exist\n");   
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

	if (tf != NULL)
	    print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
