# AnimeOS
An x64 UEFI hobby OS.

[FLOW.md](FLOW.md) describes the order of tasks which are executed to get the system running, this is just for my reference.

The bootloader is written in C, as it does not require more advanced features, while the kernel is written in C++.

Not POSIX-compliant but many functions will be named accordingly (eg. memcpy).

## Todo
* Invalidate old translation when setting page translation
* Map the PML4
* Support custom sized pages

## Roadmap
Done:
* Text renderer
* Memory manager
* GDT
* IDT

Planned (Could change at any time) (also in a very vague order):
* Heap (next)
* Scheduler
* SMP
* initrd
* Filesystem
* ELF loader
* Userspace

## Use
```Bash
git clone --recurse-submodules https://github.com/catgirladdict/AnimeOS
cd AnimeOS
./run.sh
```

A focus of this project is to have as easy setup as possible, so all you need is QEMU and clang.
