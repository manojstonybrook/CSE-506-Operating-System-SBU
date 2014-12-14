/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>
#include <kern/macro.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushq $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushq $0;							\
	pushq $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
   Find the PUSHA,POPA macros in kern/macro.h. Since amd64 doesn't support
   pusha,popa so you will find these macros handy.
 */
  TRAPHANDLER_NOEC(jos64_divide, 0)
  TRAPHANDLER_NOEC(jos64_debug, 1)
  TRAPHANDLER_NOEC(jos64_nmi, 2)
  TRAPHANDLER_NOEC(jos64_breakpoint, 3)
  TRAPHANDLER_NOEC(jos64_overflow, 4)
  TRAPHANDLER_NOEC(jos64_boundcheck, 5)
  TRAPHANDLER_NOEC(jos64_ill_opcode, 6)
  TRAPHANDLER_NOEC(jos64_device, 7)

  TRAPHANDLER(jos64_doublefault, 8)
  TRAPHANDLER(jos64_tss_segment, 10)
  TRAPHANDLER(jos64_stack, 11)
  TRAPHANDLER(jos64_stackexception, 12)
  TRAPHANDLER(jos64_protectionfault, 13)
  TRAPHANDLER(jos64_pagefault, 14)

  TRAPHANDLER_NOEC(jos64_fp_error, 16)
  TRAPHANDLER_NOEC(jos64_align, 17)
  TRAPHANDLER_NOEC(jos64_mcheck, 18)
  TRAPHANDLER_NOEC(jos64_simd_error, 19)
  TRAPHANDLER_NOEC(jos64_systemcall, 48)
  
  TRAPHANDLER_NOEC(jos64_irq32, IRQ_OFFSET)
  TRAPHANDLER_NOEC(jos64_irq33, IRQ_OFFSET+1)
  TRAPHANDLER_NOEC(jos64_irq34, IRQ_OFFSET+2)
  TRAPHANDLER_NOEC(jos64_irq35, IRQ_OFFSET+3)
  TRAPHANDLER_NOEC(jos64_irq36, IRQ_OFFSET+4)
  TRAPHANDLER_NOEC(jos64_irq37, IRQ_OFFSET+5)
  TRAPHANDLER_NOEC(jos64_irq38, IRQ_OFFSET+6)
  TRAPHANDLER_NOEC(jos64_irq39, IRQ_OFFSET+7)
  TRAPHANDLER_NOEC(jos64_irq40, IRQ_OFFSET+8)
  TRAPHANDLER_NOEC(jos64_irq41, IRQ_OFFSET+9)
  TRAPHANDLER_NOEC(jos64_irq42, IRQ_OFFSET+10)
  TRAPHANDLER_NOEC(jos64_irq43, IRQ_OFFSET+11)
  TRAPHANDLER_NOEC(jos64_irq44, IRQ_OFFSET+12)
  TRAPHANDLER_NOEC(jos64_irq45, IRQ_OFFSET+13)
  TRAPHANDLER_NOEC(jos64_irq46, IRQ_OFFSET+14)
  TRAPHANDLER_NOEC(jos64_irq47, IRQ_OFFSET+15)

  
  	 
/*
 * Lab 3: Your code here for _alltraps
 *
 * Hint: Be sure to review the x64 calling convention from lab1
 *       for how to pass a pointer to the trapframe.
 */


 _alltraps:
        movq $0, %r8
        movw %ds,%r8
        pushq %r8
        movw %es, %r8
        pushq %r8
        PUSHA
        mov %rsp, %rdi
        movq $0, %r8
        movw $(GD_KD), %r8
        movw %r8, %es
        movw %r8, %ds
        call trap
        POPA_
	mov $0, %rax
	popq %rax
	movw %rax, %es
	popq %rax
	movw %rax, %ds
	addq $8, %rsp
    	iret 
