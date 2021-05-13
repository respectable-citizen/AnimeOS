#include "pmm.hpp"

#include "text_renderer/text_renderer.hpp"

#include "utils.hpp"

namespace PMM {
	namespace {
		uint8_t *m_page_map;
		uint64_t m_pages_for_page_map;

		uint64_t m_total_pages;

		EFI_MEMORY_DESCRIPTOR* get_memory_descriptor(MemoryMap memory_map, uint32_t index) {
			return (EFI_MEMORY_DESCRIPTOR*) ((uint8_t*) memory_map.memory_descriptors + (memory_map.descriptor_size * index));
		}
	}
	
	void initialise(MemoryMap memory_map, uint64_t kernel_page, uint64_t kernel_size_pages) {
		//Determine number of pages
		uint32_t descriptor_count = memory_map.memory_map_size / memory_map.descriptor_size;
		EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, descriptor_count - 1);
		m_total_pages = (descriptor->PhysicalStart / 4096) + descriptor->NumberOfPages;
		m_pages_for_page_map = bytes_to_pages(m_total_pages);
		
		//Find a block of pages large enough to store our page map
		bool allocated_page_map = false;
		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, i);
			
			if (descriptor->Type == EfiConventionalMemory && descriptor->NumberOfPages >= m_pages_for_page_map) {
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
		lock_pages(0, m_total_pages);

		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = get_memory_descriptor(memory_map, i);
			
			uint64_t descriptor_page = descriptor->PhysicalStart / 4096;

			if (descriptor->Type == EfiConventionalMemory) {
				free_pages(descriptor_page, descriptor->NumberOfPages);
			}
		}

		//Mark page map as reserved memory
		lock_pages(address_to_page_number(m_page_map), m_pages_for_page_map);

		//Mark kernelspace as reserved memory
		lock_pages(kernel_page, kernel_size_pages);
	}

	void lock_pages(uint64_t page, uint64_t page_count) {
		for (uint64_t i = 0; i < page_count; i++) m_page_map[page + i] = PAGE_LOCKED;
	}

	void free_pages(uint64_t page, uint64_t page_count) {
		for (uint64_t i = 0; i < page_count; i++) m_page_map[page + i] = PAGE_FREE;
	}

	uint64_t request_page() {
		for (uint64_t i = 0; i < m_total_pages; i++) {
			if (m_page_map[i] == PAGE_FREE) {
				lock_pages(i, 1);
				return i;
			}
		}

		TextRenderer::kernel_panic((char*) "PMM could not find a free page to allocate");
		return 0;
	}

	void* alloc_page() {
		return PMM::page_number_to_address(request_page());
	}

	void* page_map() {
		return m_page_map;
	}
	
	uint64_t pages_for_page_map() {
		return m_pages_for_page_map;
	}
}
