#!/bin/sh
./build.sh
sudo cp BOOTX64.efi /boot/EFI/Custom/BOOTX64.efi
sudo cp kernel.elf /boot/EFI/Custom/kernel.elf
