#include "heap.hpp"

#include "text_renderer/text_renderer.hpp"

#include "lib/memory.hpp"

#include "pmm.hpp"

//TODO: This entire implementation is slow and incomplete, definitely need to work on this.
namespace Heap {
	namespace {
		uint8_t *heap_start;
		uint8_t *heap_current; //Stores the first free byte of the heap (ignoring bytes that have become free after calling free())
		uint8_t *heap_end;     //Store the actual end of the heap

		uint64_t total_bytes;
		uint64_t free_bytes;
	}
	
	void initialise(uint64_t page_count) {
		heap_start = (uint8_t*) PMM::allocate_kernel_pages(page_count);
		heap_end = (uint8_t*) (((uint64_t) heap_start) + (page_count * 4096));
		heap_current = heap_start;

		total_bytes = (page_count * 4096);
		free_bytes = total_bytes;
	}
	
	void defragment() {
		TextRenderer::kernel_panic((char*) "defragmenting\r\n");
	}

	void* malloc(uint64_t bytes) {
		uint64_t required_bytes = bytes + 8; //We actually need eight extra bytes to store the size
		if (free_bytes < required_bytes) {
			//TODO: This should expand the heap, but we currently have no support for that so we instead kernel panic
			TextRenderer::kernel_panic((char*) "Ran out of memory for kernel heap.");
			return nullptr;
		} else {
			uint64_t bytes_till_heap_end = (uint64_t) heap_end - (uint64_t) heap_current;
			if (bytes_till_heap_end < required_bytes) {
				//The heap has enough space but it is currently too fragmend to allocate, so defragment first
				defragment();
			}
			*((uint64_t*) heap_current) = bytes;
			uint8_t *allocated_memory = heap_current + 8;
			heap_current = (uint8_t*) ((uint64_t) heap_current + required_bytes);
			free_bytes -= required_bytes;
			return (void*) allocated_memory;
		}
	}

	void free(void *p) {
		uint8_t *entry = (uint8_t*) p;
		entry -= 8;
		uint64_t size = *((uint64_t*) entry);
	
		memset(entry, 0, size + 8);
	}
}
