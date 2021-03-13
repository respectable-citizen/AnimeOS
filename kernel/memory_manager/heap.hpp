#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

namespace Heap {
	void initialise(uint64_t);
	void defragment();
	void* malloc(uint64_t);
};

#endif
