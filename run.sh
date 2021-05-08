#!/bin/sh
./build.sh
#dd if=/dev/zero of=disk.img bs=1M count=2
#mformat -i disk.img ::
#mmd -i disk.img ::/EFI
#mmd -i disk.img ::/EFI/Custom
#mmd -i disk.img ::/EFI/BOOT
#mcopy -i disk.img kernel.elf ::/EFI/Custom/kernel.elf
#mcopy -i disk.img BOOTX64.efi ::/EFI/BOOT/BOOTx64.efi
rm -r fs
mkdir -p fs/EFI/BOOT
mkdir fs/EFI/Custom
cp BOOTX64.efi fs/EFI/BOOT
cp kernel.elf fs/EFI/Custom
qemu-system-x86_64 \
				-vga virtio \
				-cpu qemu64 \
				-bios /usr/share/OVMF/x64/OVMF.fd \
				-net none \
				-m 10G \
				--no-shutdown --no-reboot \
				-drive file=fat:rw:fs,format=raw #-drive format=raw,file=disk.img
