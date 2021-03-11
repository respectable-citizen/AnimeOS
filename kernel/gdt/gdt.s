.text

.extern gdt_descriptor
.globl load_gdt

load_gdt:
	#Load GDT
	lgdt (%rdi)
	
	movq %rsp, %rax
	pushq $0x10      #Stack segment is at the data segment
	pushq %rax       #Keep stack pointer the same
	pushfq           #Keep flags the same
	pushq $0x08      #Code segment at 0x08 in GDT

	#Set data segment registers
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	#Begin execution after iretq at return_to_kernel label
	movabs $return_to_kernel, %rax
	pushq %rax

	iretq

return_to_kernel:
	ret
