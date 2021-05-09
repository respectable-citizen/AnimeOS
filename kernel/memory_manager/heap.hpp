#pragma once

#include <stdint.h>

namespace Heap {
	void initialise();
	void* malloc(uint64_t);
	void free(void*);
};
