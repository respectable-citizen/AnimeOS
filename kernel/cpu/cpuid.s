.section .text

.globl check_cpuid_supported

check_cpuid_supported:
	mov $0x0, %al
	cpuid
	mov $0x1, %al
	ret
