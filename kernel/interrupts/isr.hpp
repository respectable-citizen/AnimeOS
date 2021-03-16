#ifndef ISR_H
#define ISR_H

#include <stdint.h>

/*
struct InterruptFrame {
	uint64_t	rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
	uint64_t	error_code, rip, cs, rflags, rsp, ss;
};
*/

__attribute((interrupt)) void isr8(void*);
__attribute((interrupt)) void isr14(void*);
__attribute((interrupt)) void isr128(void*);

#endif
