#include "pmm.hpp"

#include "vmm.hpp"

#include "text_renderer/text_renderer.hpp"

#include "utils.hpp"

namespace PMM {
	namespace {
		uint8_t *m_page_map;
		uint64_t m_page_map_size; //In pages

		//These fields are purely informational, they are kept up to date by the corresponding functions
		uint64_t m_total_pages; //This is NOT representational of the amount of installed RAM, it is simply the number of pages reported by the memory map (which may have inconsistencies)
		uint64_t m_free_pages;
	
		EFI_MEMORY_DESCRIPTOR* get_memory_descriptor(MemoryMap memory_map, uint32_t index) {
			return (EFI_MEMORY_DESCRIPTOR*) ((uint8_t*) memory_map.memory_descriptors + (memory_map.descriptor_size * index));
		}
	}
	
	//TODO: Handle memory map being out of order
	//TODO: Handle overlapping descriptors
	void initialise(MemoryMap memory_map, uint64_t kernel_page, uint64_t kernel_size_pages) {
		//Determine number of pages
		uint32_t descriptor_count = memory_map.memory_map_size / memory_map.descriptor_size;
		EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, descriptor_count - 1);
		m_total_pages = (descriptor->PhysicalStart / 4096) + descriptor->NumberOfPages;
		
		//Find a block of pages large enough to store our page map
		m_page_map_size = (m_total_pages / 4096) + 1;
		bool allocated_page_map = false;
		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, i);
			
			if (descriptor->Type == EfiConventionalMemory && descriptor->NumberOfPages >= m_page_map_size) {
				m_page_map = (uint8_t*) descriptor->PhysicalStart;
				allocated_page_map = true;
				break;
			}
		}

		if (!allocated_page_map) {
			//We couldn't find enough contiguous memory to store the page map
			//I don't think this would ever happen, but, just in case it does, error
			TextRenderer::kernel_panic((char*) "Not enough space to store page map.");
		}

		//Let's now fill the page map with real information, we do this by assuming all pages are in use, and slowly freeing the pages as we find out we can use them
		m_free_pages = 0;
		lock_pages(0, m_total_pages);

		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, i);
			
			uint64_t descriptor_page = descriptor->PhysicalStart / 4096;

			if (descriptor->Type == EfiConventionalMemory) {
				free_pages(descriptor_page, descriptor->NumberOfPages);
			}
		}

		//Mark page map as reserved memory
		lock_pages(address_to_page_number(m_page_map), m_page_map_size);

		//Mark kernelspace as reserved memory
		lock_pages(kernel_page, kernel_size_pages);
	}
	
	uint8_t get_page(uint64_t page_number) {
		return m_page_map[page_number];
	}
	
	uint64_t set_page_status(uint64_t page_start, uint64_t page_count, uint8_t page_status) {
		uint64_t modified_pages = 0;
		for (uint64_t i = 0; i < page_count; i++) {
			uint8_t page = get_page(page_start + i);
			if (page != page_status) {
				m_page_map[page_start + i] = page_status;
				modified_pages++;
			}
		}
		return modified_pages;
	}
	
	void lock_pages(uint64_t page_start, uint64_t page_count) {
		m_free_pages -= set_page_status(page_start, page_count, PAGE_LOCKED);
	}

	void free_pages(uint64_t page_start, uint64_t page_count) {
		m_free_pages += set_page_status(page_start, page_count, PAGE_FREE);
	}

	void* allocate_pages(uint64_t page_count, bool kernelspace, PageSize page_size, bool map) {
		uint64_t contiguous_pages = 0;
		uint64_t contiguous_pages_start = 0;

		//Choose which address space to search for available pages
		uint64_t start_page = 0;
		uint64_t end_page = KERNEL_MEMORY_PAGES;

		if (!kernelspace) {
			start_page = KERNEL_MEMORY_PAGES;
			end_page = m_total_pages;
		}

		for (uint64_t i = start_page; i < end_page; i++) {
			uint8_t page = get_page(i);
			if (page == PAGE_FREE) {
				if (contiguous_pages == 0) contiguous_pages_start = i;
				contiguous_pages++;
				if (contiguous_pages == page_count) {
					lock_pages(contiguous_pages_start, page_count);
					if (map) VMM::set_translation(contiguous_pages_start, contiguous_pages_start, page_count, page_size);

					return page_number_to_address(contiguous_pages_start);
				}
			}
		}

		if (kernelspace) {
			TextRenderer::kernel_panic((char*) "Could not find enough contiguous pages in kernelspace to allocate memory.");
		} else {
			TextRenderer::kernel_panic((char*) "Could not find enough contiguous pages in userspace to allocate memory.");
		}
		return nullptr;
	}

	void* allocate_kernel_pages(uint64_t page_count, PageSize page_size) {
		return allocate_pages(page_count, true, page_size, false);
	}

	void* allocate_user_pages(uint64_t page_count, PageSize page_size) {
		return allocate_pages(page_count, false, page_size, true);
	}

	uint64_t address_to_page_number(void* address) {
		return (uint64_t) address / 4096;
	}
	
	void* page_number_to_address(uint64_t page_number) {
		return (void*) (page_number * 4096);
	}

	void* page_map() {
		return m_page_map;
	}

	uint64_t page_map_size() {
		return m_page_map_size;
	}
}
