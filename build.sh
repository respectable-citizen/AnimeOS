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
#clang $CFLAGS -o boot.o boot/boot.c
#ld -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o boot.o -o main.so -lefi -lgnuefi
#objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-x86_64 main.so BOOTX64.efi

clang -I efi -I common -target x86_64-pc-win32-coff -fno-builtin -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone -c boot/boot.c
lld-link -subsystem:efi_application -nodefaultlib -dll -entry:efi_main boot.o -out:BOOTX64.efi

#Compile kernel
clang++ -I efi -I common -fno-stack-protector -fshort-wchar -mno-red-zone -c kernel/kernel.cpp
ld -T kernel/kernel.lds kernel.o -static -o kernel.elf
#clang $CFLAGS -o kernel.o kernel/kernel.c
#ld -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o kernel.o -o main.so -lefi -lgnuefi
#objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc main.so kernel.elf
