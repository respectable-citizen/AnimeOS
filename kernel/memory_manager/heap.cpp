#include <stdint.h>

#include "pmm.hpp"
#include "../lib/vector.hpp"
#include "../text_renderer/text_renderer.hpp"

namespace Heap {
	namespace {
		struct HeapDescriptor {
			void *start;
			uint64_t size;
			bool free;
		};

		void *m_heap;
		Vector<HeapDescriptor> m_heap_descriptors;
	}

	void initialise() {
		uint64_t page_count = 64;
		m_heap = PMM::allocate_kernel_pages(page_count);
		HeapDescriptor initial_descriptor;
		initial_descriptor.start = m_heap;
		initial_descriptor.size = PMM::pages_to_bytes(page_count);
		initial_descriptor.free = 1;
		m_heap_descriptors.push(initial_descriptor);
	}

	void* malloc(uint64_t bytes) {
		bytes = 1;
		return nullptr;
	}

	void free(void *p) {
		p = (void*) 1;
	}
};
