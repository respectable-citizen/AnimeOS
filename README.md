# AnimeOS
**Notice: This project is paused for now as I want to write my own language, at some point in time I would like to rewrite this project in said language. I don't think the current codebase runs (or, not properly at least) because I was rewriting the project when I stopped working on it.**

An x64 UEFI hobby OS dedicated to all forms of degeneracy.

[FLOW.md](FLOW.md) describes the order of tasks which are executed to get the system running, this is just for my reference.

The bootloader is written in C, as it does not require more advanced features, while the kernel is written in C++.

Not POSIX-compliant but many functions will be named accordingly (eg. memcpy).

## Roadmap
Done:
* Text renderer
* Memory manager
* GDT
* IDT
* Heap

Planned (Could change at any time) (also in a very vague order):
* Refactor codebase and write combining for framebuffer (In progress)
* PCI
* NVMe
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
