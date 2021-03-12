.section .text

.extern kernel_main
.globl entry_point

entry_point:
	mov $stack_top, %rsp
	jmp kernel_main

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:
