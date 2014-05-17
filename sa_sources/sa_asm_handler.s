/*
 * sa_asm_handler_gcc.s
 *
 *  Created on: Aug 7, 2012
 *      Author: B28412
 */
 
 .LC0:
	.text
	.align	2
	.global	sa_interrupt_handler
	.type sa_interrupt_handler function
	
sa_interrupt_handler:
	mov r0, sp
	add r0, #0x18
	bl setupTracepoints
	movw lr, #0xFFF9
	movt lr, #0xFFFF
	bx lr
