// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

extern void _pgfault_upcall(void);
//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{

	void *addr = (void *) utf->utf_fault_va;
        uint32_t err = utf->utf_err;
        int r;

        // Check that the faulting access was (1) a write, and (2) to a
        // copy-on-write page.  If not, panic.
        // Hint:
        //   Use the read-only page table mappings at vpt
        //   (see <inc/memlayout.h>).

        // LAB 4: Your code here.
	void *va;
	va = ROUNDDOWN(addr, PGSIZE);
	int pageno = PPN(va);
        int pte = uvpt[pageno];

        if (!(err & FEC_WR))
             panic("Faulting access is not write ");


	if(!(pte & PTE_COW))
	  panic("Page entry is not on COW page");

        // Allocate a new page, map it at a temporary location (PFTEMP),
        // copy the data from the old page to the new page, then move the new
        // page to the old page's address.
        // Hint:
        //   You should make three system calls.
        //   No need to explicitly delete the old page's mapping.

        // LAB 4: Your code here.
	envid_t id = sys_getenvid();
        if((sys_page_alloc(id, (void *)PFTEMP, PTE_P|PTE_U|PTE_W)) < 0)
                panic("sys_page_alloc failed lib/fork.c:50");

        memcpy(PFTEMP, va, PGSIZE);

        if ((sys_page_map(id, (void *)PFTEMP, id, (void *)va, PTE_P|PTE_U|PTE_W)) < 0)
              panic(" page map failed lib/fork.c");

	if (sys_page_unmap(id, (void *)PFTEMP) < 0)
              panic("sys_page_unmap failed lib/fork.c");
	

}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	
	uint64_t va;
        int r;
        int pte = uvpt[pn];
        int perm = pte & 0xfff;
        va = ROUNDDOWN(pn * PGSIZE, PGSIZE);
	int err;
	int perm1 = perm;
        if((perm & PTE_W) || (perm & PTE_COW)) 
	{
	   perm = perm | PTE_COW;
 	   perm = perm & (~PTE_W);	
	}

 	if(sys_page_map(thisenv->env_id, (void *) va, envid, (void *) va, perm) < 0)
           panic("page map failed in duplicate");
	
	if((perm1 & PTE_W) || (perm1 & PTE_COW))
	{
	  err = sys_page_map(thisenv->env_id, (void *)va, thisenv->env_id, (void *)va, perm);
	  if(err < 0) 
	    panic("Own page map failed %e", err); 
	}
	  return 0;	   	
/*        }
	else
	{
	  if (sys_page_map(thisenv->env_id, (void *) va, envid, (void *) va, perm) < 0)
               panic("page map failed in duplicate");

	 return 0;
	}
*/	
	
        // if(sys_page_map(thisenv->env_id, (void *)va, envid, (void *)va, perm) < 0)
	  //  panic("mapping failed without COW");

	//return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{

	envid_t child_id;        
        //int r;
	uint8_t *addr;

        set_pgfault_handler(pgfault);
        child_id = sys_exofork();

        if(child_id < 0)
             panic("child not created lib/fork.c");

        if(child_id == 0)
	 {
            thisenv = &envs[ENVX(sys_getenvid())];
            return 0;
         }

	extern unsigned char end[];
        for (addr = (uint8_t *)UTEXT; addr < end/*USTACKTOP-PGSIZE*/; addr += PGSIZE) 
	{
              // if(!(uvpml4e[VPML4E(addr)] & PTE_P))
		//  continue;
		
		//if(!(uvpde[VPDPE(addr)] & PTE_P))
	          // continue;
	
                if(!(uvpd[VPD(addr)] & PTE_P))
		  continue;
		
		if(!(uvpt[VPN(addr)] & PTE_P))
		  continue; 
                       
		duppage(child_id, PPN(addr)); 
        }

	if((sys_page_alloc(child_id, (void *) (UXSTACKTOP - PGSIZE), PTE_P|PTE_U|PTE_W)) < 0)
            panic("allocation in exception stack");	

        if(duppage(child_id, PPN(USTACKTOP-PGSIZE)) < 0)
	  panic("not able to duplicate stack");

        if (sys_env_set_pgfault_upcall(child_id, _pgfault_upcall) < 0)
          panic("not able to set page fault upcall");

        if ((sys_env_set_status(child_id, ENV_RUNNABLE)) < 0)
          panic("Not able to make environment runnable");

        return child_id;

}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
