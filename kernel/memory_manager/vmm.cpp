#include "vmm.hpp"

#include "pmm.hpp"

#include "text_renderer/text_renderer.hpp"
#include "lib/memory.hpp"

#include "utils.hpp"

namespace VMM {
	namespace {
		uint64_t *kernel_paging_table;
		uint64_t *current_paging_table;
		uint64_t FLAG_PRESENT = 1 << 0;
		uint64_t FLAG_READWRITE = 1 << 1;
		uint64_t FLAG_WT = 1 << 3;
		uint64_t FLAG_PAT = 1 << 7;
		uint64_t FLAGS_DEFAULT = FLAG_PRESENT | FLAG_READWRITE;
	}


	void initialise(uint64_t kernel_page, uint64_t kernel_size_pages) {
		//Check if kernel is compatible with our memory layout
		if ((kernel_page + kernel_size_pages) >= KERNEL_MEMORY_PAGES) {
			TextRenderer::kernel_panic((char*) "Kernel must end before 2GiB in memory.");
		}

		//Setup kernel page table
		kernel_paging_table = (uint64_t*) PMM::allocate_kernel_pages(1);
		memset((void*) kernel_paging_table, 0, 4096);
		current_paging_table = kernel_paging_table;
		
		TextRenderer::draw_number(FLAG_PRESENT);
		TextRenderer::draw_string((char*) "\r\n");
		//Identity map required amount of pages for kernel
		VMM::set_translation(0, 0, KERNEL_MEMORY_PAGES);
		
		//Identity map the framebuffer
		uint64_t framebuffer_page = PMM::address_to_page_number(TextRenderer::graphics_info().address);
		VMM::set_translation(framebuffer_page, framebuffer_page, PMM::bytes_to_pages(TextRenderer::graphics_info().buffer_size), FLAGS_DEFAULT);
		
		//Identity map the PMM page map
		//uint64_t page_map = PMM::address_to_page_number(PMM::page_map());
		//VMM::set_translation(page_map, page_map, PMM::page_map_size());
		
		asm("mov %0, %%cr3" : : "r" (current_paging_table)); //Now that we have setup the kernel VAS, we can safely enable paging without messing anything up
	}

	void set_translation(uint64_t physical_page, uint64_t virtual_page, uint64_t page_count, uint64_t flags) {
		//if (page_size != PageSize::FOUR_KIB) TextRenderer::kernel_panic((char*) "Only 4KiB pages are currently supported.");
		TextRenderer::draw_number(flags);
		TextRenderer::draw_string((char*) "\r\n");
		for (uint64_t i = 0; i < page_count; i++)
			set_page_translation(physical_page + i, virtual_page + i, flags);
	}
	
	void set_page_translation(uint64_t physical_page, uint64_t virtual_page, uint64_t flags) {
		uint8_t page_size = 4;

		uint64_t *current_table = current_paging_table;
		for (uint8_t i = 0; i < page_size; i++) {
			uint64_t shift_amount = 9 * (3 - i);
			uint64_t mask = 0x1FFULL << shift_amount;
			uint64_t table_index = (virtual_page & mask) >> shift_amount;
	
			//We have reached the required table depth, time to create the actual table entry
			if (i == (page_size - 1)) {
				uint64_t address = physical_page << 12;
				asm volatile("invlpg (%0)" :: "r" (address) : "memory");
				current_table[table_index] = (address | flags);
				break;
			}

			void *subtable_address;
			if (current_table[table_index] & FLAG_PRESENT) {
				subtable_address = (void*) (((current_table[table_index]) >> 12) << 12); //We do this bitshifting to clear the flags and get just the address
			} else {
				//Table doesn't yet exist, allocate it
				void *sub_table = PMM::allocate_kernel_pages(1);
				memset(sub_table, 0, 4096);
				current_table[table_index] = ((uint64_t) sub_table | flags);
				subtable_address = (uint64_t*) sub_table;
			}

			current_table = (uint64_t*) subtable_address;
		}
	}

}
