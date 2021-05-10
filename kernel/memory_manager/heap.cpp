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
		for (uint64_t i = 0; i < m_heap_descriptors.size(); i++) {
			HeapDescriptor descriptor = m_heap_descriptors[i];
			if (descriptor.free && descriptor.size >= bytes) {
				if (descriptor.size == bytes) {
					//Descriptor is same as required size so we don't need to split it
					descriptor.free = 0;
					m_heap_descriptors.set(i, descriptor);
					return descriptor.start;
				} else {
					//The new descriptor goes directly after the free descriptor that it's taking memory from
					//Update old descriptor
					descriptor.size -= bytes;
					m_heap_descriptors.set(i, descriptor);	
					
					//Create new descriptpr
					HeapDescriptor new_descriptor;
					new_descriptor.start = (void*) ((uint64_t) descriptor.start + descriptor.size);
					new_descriptor.size = bytes;
					new_descriptor.free = 0;
					m_heap_descriptors.insert(i + 1, new_descriptor);

					return new_descriptor.start;
				}
			}
		}

		//If we haven't returned by this point the heap must be out of memory, later on this should expand the heap
		TextRenderer::kernel_panic((char*) "Kernel heap ran out of memory");
		return nullptr;
	}

	void debug() {
		for (uint64_t i = 0; i < m_heap_descriptors.size(); i++) {
			HeapDescriptor descriptor = m_heap_descriptors[i];
			TextRenderer::draw_string((char*) "start(");
			TextRenderer::draw_number((uint64_t) descriptor.start);
			TextRenderer::draw_string((char*) ") : size(");
			TextRenderer::draw_number(descriptor.size);
			TextRenderer::draw_string((char*) ") : free(");
			TextRenderer::draw_number(descriptor.free);
			TextRenderer::draw_string((char*) ")\r\n");
		}
	}

	void free(void *p) {
		for (uint64_t i = 0; i < m_heap_descriptors.size(); i++) {
			HeapDescriptor descriptor = m_heap_descriptors[i];
			if (descriptor.start == p) {
				if (descriptor.free) TextRenderer::kernel_panic((char*) "Attempt to double free a pointer");

				descriptor.free = 1;
				m_heap_descriptors.set(i, descriptor);
				
				//The rest of this code is just to merge any free sections to the left and right, it's maybe a little bit confusing because we have to handle indexes changing when we delete an item (is there a better way to write the vector class to avoid this?)

				//Merge right
				if ((i + 1) < m_heap_descriptors.size()) {
					HeapDescriptor right_descriptor = m_heap_descriptors[i + 1];
					if (right_descriptor.free) {
						descriptor.size += right_descriptor.size;
						m_heap_descriptors.set(i, descriptor);
						m_heap_descriptors.remove(i + 1);
					}
				}

				//Merge left
				if (i != 0) {
					HeapDescriptor left_descriptor = m_heap_descriptors[i - 1];
					if (left_descriptor.free) {
						left_descriptor.size += descriptor.size;
						m_heap_descriptors.set(i - 1, left_descriptor);
						m_heap_descriptors.remove(i);
					}
				}
				return;
			}
		}

		TextRenderer::kernel_panic((char*) "Attempt to free invalid pointer");
	}
};
