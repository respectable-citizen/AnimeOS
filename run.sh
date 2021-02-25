#!/bin/sh
./build.sh
qemu-system-x86_64 -cpu qemu64 -bios /usr/share/OVMF/x64/OVMF.fd -net none -kernel BOOTX64.efi
