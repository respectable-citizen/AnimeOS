#pragma once

#include <stdint.h>

extern "C" void* memcpy(void *destination, void *source, uint64_t size);
extern "C" void* memmove(void *destination, void *source, uint64_t size);
extern "C" void* memset(void *location, uint8_t byte, uint64_t size);
