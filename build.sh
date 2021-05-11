#!/bin/sh
CFLAGS="-I/usr/include/efi \
-I/usr/include/efi/x86_64 \
-I/usr/include/efi/protocol \
-fno-stack-protector \
-fpic \
-fshort-wchar \
-mno-red-zone \
-DHAVE_USE_MS_ABI"

#Compile bootloader
clang -I efi -I common -target x86_64-pc-win32-coff -fno-builtin -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone -c boot/boot.c
lld-link -subsystem:efi_application -nodefaultlib -dll -entry:efi_main boot.o -out:BOOTX64.efi

#Compile kernel
clang -I efi -I common -I kernel -I kernel/text_renderer -I kernel/lib -I kernel/memory_manager -I kernel/gdt -I kernel/interrupts -g -ffreestanding -fno-stack-protector -nostdlib -fuse-ld=lld -static -T kernel/kernel.lds \
				-Wall -Wextra -Werror -Wmissing-field-initializers -Wuninitialized \
				kernel/kernel.s \
				kernel/kernel.cpp \
				kernel/text_renderer/text_renderer.cpp \
				kernel/lib/memory.cpp \
				kernel/lib/math.cpp \
				kernel/utils.cpp \
				kernel/memory_manager/pmm.cpp \
				kernel/memory_manager/vmm.cpp \
				kernel/memory_manager/block32.cpp \
				kernel/memory_manager/heap.cpp \
				kernel/gdt/gdt.cpp \
				kernel/gdt/gdt.s \
				kernel/interrupts/interrupts.cpp \
				kernel/interrupts/isr.cpp \
				kernel/cpu/cpu.cpp \
				kernel/cpu/cpuid.s \
				kernel/cpu/cpuid.cpp \
				kernel/cpu/msr.cpp \
				-o kernel.elf
