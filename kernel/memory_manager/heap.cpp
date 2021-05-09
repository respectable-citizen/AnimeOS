#include <stdint.h>

#include "pmm.hpp"
#include "block32.hpp"
#include "../text_renderer/text_renderer.hpp"

namespace Heap {
	namespace {
	}

	void initialise() {
	}

	void* malloc(uint64_t bytes) {
		bytes = 1;
		return nullptr;
	}

	void free(void *p) {
		p = (void*) 1;
	}
};
