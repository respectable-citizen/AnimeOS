# hobby-os
An x64 UEFI hobby OS.

[FLOW.md](FLOW.md) describes the order of tasks which are executed to get the system running, this is just for my reference.

The bootloader is written in C, as it does not require more advanced features, while the kernel is written in C++.

## Use
```Bash
git clone --recurse-submodules https://github.com/catgirladdict/hobby-os
cd hobby-os
./run.sh
```

A focus of this project is to have as easy setup as possible, so all you need is QEMU and clang.
