#pragma once

#include <stdint.h>

namespace Block32 {
	void initialise();
	void* allocate();
	void free(void*);
};
