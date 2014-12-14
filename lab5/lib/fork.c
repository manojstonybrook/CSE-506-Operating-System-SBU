
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
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).
	
	// LAB 4: Your code here.
	//cprintf("\nInside pgfault!!\n\n");
	uint64_t va;
	va = (uint64_t)ROUNDDOWN(addr, PGSIZE);
	int pageno = PPN(va);
        int pte = uvpt[pageno];
	pte = pte & 0xfff;
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
	//envid_t id = sys_getenvid();
        if((sys_page_alloc(0, (void *)PFTEMP, PTE_P|PTE_U|PTE_W)) < 0)
                panic("sys_page_alloc failed lib/fork.c:50");

        memcpy(PFTEMP, (void *)va, PGSIZE);

        if((sys_page_map(0, (void *)PFTEMP, 0, (void *)va, PTE_P|PTE_U|PTE_W)) < 0)
              panic(" page map failed lib/fork.c");

	if(sys_page_unmap(0, (void *)PFTEMP) < 0)
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
       // LAB 4: Your code here.
	uint64_t va;
        int r;
        pte_t pte = uvpt[pn];
        int perm = pte & 0xfff;
        int perms = pte & PTE_SYSCALL;

        va = pn * PGSIZE;

        if (perms & PTE_SHARE)
	{
            r  = sys_page_map(0, (void *)va, envid, (void *)va, perms);
	    return r;

	}
        if (pte & PTE_W || pte & PTE_COW) 
	{
	    perm = perm | PTE_COW;
	    perm = perm & ~PTE_W;
	}
            r = sys_page_map(0, (void *)va, envid, (void *) va, perm);
	    if(r < 0)
	      return r;
            r = sys_page_map(0, (void *)va, 0, (void *)va, perm);

	    return r;
        


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
//   Use vpd, vpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	envid_t child_id;        
        //int r;
	uint64_t addr;
	uint64_t PN;
        set_pgfault_handler(pgfault);
        child_id = sys_exofork();
        
	if(child_id < 0)
             panic("child not created lib/fork.c");

        if(child_id == 0)
	 {
            thisenv = &envs[ENVX(sys_getenvid())];
            return 0;
         }

        for (addr = UTEXT; addr < (USTACKTOP-PGSIZE); addr += PGSIZE) 
	{
		
               if(!(uvpml4e[VPML4E(addr)]))
		 continue;
		
		if(!(uvpde[VPDPE(addr)]))
	          continue;
	
                if(!(uvpd[VPD(addr)]))
		  continue;
		
		if(!(uvpt[VPN(addr)]))
		  continue; 
                       
		duppage(child_id, VPN(addr)); 
        }

	if((sys_page_alloc(child_id, (void *) (UXSTACKTOP - PGSIZE), PTE_P|PTE_U|PTE_W)) < 0)
            panic("allocation in exception stack");	

        if(duppage(child_id, VPN(USTACKTOP-PGSIZE)) < 0)
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
