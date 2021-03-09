#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

extern "C" void* memcpy(void *destination, void *source, uint64_t size);
extern "C" void* memset(void *location, uint8_t byte, uint64_t size);

#endif
