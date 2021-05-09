#include "block32.hpp"

#include "./text_renderer/text_renderer.hpp"
#include "pmm.hpp"

namespace Block32 {
	namespace {
		void *blocks;
		uint64_t block_count;

		uint8_t *block_map;
	}

	void initialise() {
		//Allocate space for blocks
		block_count = 4096; //This number is chosen so the block map fits neatly into one page
		blocks = PMM::allocate_kernel_pages(PMM::bytes_to_pages(block_count * 32));

		//Allocate block map
		block_map = (uint8_t*) PMM::allocate_kernel_pages(PMM::bytes_to_pages(block_count));

		//Mark all blocks as free
		for (uint64_t i = 0; i < block_count; i++) {
			block_map[i] = 1;
		}
	}

	void* allocate() {
		for (uint64_t i = 0; i < block_count; i++) {
			if (block_map[i]) {
				block_map[i] = 0;
				return (void*) ((uint64_t) blocks + (32 * i));
			}
		}

		//None of the blocks were free, we have to allocate another page for blocks (not yet implemented)
		TextRenderer::kernel_panic((char*) "Block32 ran out of memory");
		return nullptr;
	}

	void free(void *p) {
		uint64_t block_number = ((uint64_t) p - (uint64_t) blocks) / 32;
		block_map[block_number] = 1;
	}
}
