#include "pmm.hpp"

#include "vmm.hpp"

#include "text_renderer/text_renderer.hpp"

namespace PMM {
	namespace {
		/*
		Bits 0-5 are not in use
		Bit 6 is free flag
		Bit 7 is reserved flag
		*/

		uint8_t *m_page_map;

		//These fields are purely informational, they are kept up to date by the corresponding functions
		uint64_t m_total_pages;
		uint64_t m_free_pages;
		uint64_t m_reserved_pages;
	}

	void initialise(MemoryMap memory_map, uint64_t kernel_page, uint64_t kernel_size_pages) {
		m_total_pages = 0;
		m_free_pages = 0;
		m_reserved_pages = 0;

		//Count how many pages there are, and find largest memory segment. Then determine if the largest memory segment is big enough to store our paging information
		uint32_t descriptor_count = memory_map.memory_map_size / memory_map.descriptor_size;

		uint64_t largest_segment_size = 0;
		uint8_t *largest_segment;

		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR*) (((uint8_t*) memory_map.memory_descriptors) + (memory_map.descriptor_size * i));
			
			if (descriptor->Type == EfiConventionalMemory && (descriptor->NumberOfPages > largest_segment_size)) {
				largest_segment_size = descriptor->NumberOfPages;
				largest_segment = (uint8_t*) descriptor->PhysicalStart;
			}

			m_total_pages = (descriptor->PhysicalStart / 4096) + descriptor->NumberOfPages;
		}
	
		if (m_total_pages > (largest_segment_size * 4096)) {
			//There isn't enough space in the largest segment to store the page information
			//I don't think this would ever happen, but, just in case it does, error
			TextRenderer::kernel_panic((char*) "Not enough space to store page map.");
		}
	
		m_page_map = largest_segment;
		
		//Assume all memory is reserved to start with, then slowly unreserve memory as we find that it is free
		reserve_pages(0, m_total_pages);
		m_reserved_pages = m_total_pages;

		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR*) (((uint8_t*) memory_map.memory_descriptors) + (memory_map.descriptor_size * i));
			
			uint64_t descriptor_page = descriptor->PhysicalStart / 4096;

			if (descriptor->Type == EfiConventionalMemory) {
				m_free_pages += descriptor->NumberOfPages;
				unreserve_pages(descriptor_page, descriptor->NumberOfPages);
			}
		}

		//Mark page map as reserved memory
		uint64_t pages_required_for_page_map = (m_total_pages / 4096) + 1;
		reserve_pages(address_to_page_number(m_page_map), pages_required_for_page_map);

		//Mark kernelspace as reserved memory
		reserve_pages(kernel_page, kernel_size_pages);
	}

	uint64_t free_pages() {
		return m_free_pages;		
	}

	uint64_t reserved_pages() {
		return m_reserved_pages;		
	}

	uint64_t used_pages() {
		return total_pages() - free_pages();
	}

	uint64_t total_pages() {
		return m_total_pages;
	}

	uint8_t get_page(uint64_t page) {
		return m_page_map[page];
	}

	uint64_t find_contiguous_pages(uint64_t page_count) {
		uint64_t contiguous_pages = 0;
		uint64_t page_start;
		for (uint64_t i = 0; i < m_total_pages; i++) {
			if ((m_page_map[i] & PAGE_FREE) && !(m_page_map[i] & PAGE_RESERVED)) {
				contiguous_pages++;
				if (contiguous_pages == 1) page_start = i;
				if (contiguous_pages == page_count) break;
			} else {
				contiguous_pages = 0;

			}
		}
		if (contiguous_pages != page_count) TextRenderer::kernel_panic((char*) "Could not find sufficient contiguous pages.");

		return page_start;
	}

	uint64_t modify_pages(uint64_t start_page, uint64_t page_count, uint8_t attributes) {
		uint64_t total_modified_pages = 0;
		for (uint64_t i = 0; i < page_count; i++) {
			if (get_page(start_page + i) != attributes) total_modified_pages++;
			
			m_page_map[start_page + i] = attributes;
		}
		return total_modified_pages;
	}

	void lock_pages(uint64_t start_page, uint64_t page_count) {
		m_free_pages -= modify_pages(start_page, page_count, PAGE_LOCKED);
	}
	
	void free_pages(uint64_t start_page, uint64_t page_count) {
		m_free_pages += modify_pages(start_page, page_count, PAGE_FREE);
	}
	
	void reserve_pages(uint64_t start_page, uint64_t page_count) {
		uint64_t modified_pages = modify_pages(start_page, page_count, PAGE_RESERVED);
		m_reserved_pages += modified_pages;
		m_free_pages -= modified_pages;
	}
	
	void unreserve_pages(uint64_t start_page, uint64_t page_count) {
		uint64_t modified_pages = modify_pages(start_page, page_count, PAGE_FREE);
		m_reserved_pages -= modified_pages;
		m_free_pages += modified_pages;
	}

	uint64_t allocate_pages(uint64_t page_count, bool reserve) {
		uint64_t page_number = find_contiguous_pages(page_count);
		if (reserve)
			reserve_pages(page_number, page_count);
		else
			lock_pages(page_number, page_count);
		
		return page_number;
	}

	uint64_t address_to_page_number(void *address) {
		return ((uint64_t) address) / 4096;
	}

	void* page_number_to_address(uint64_t page_number) {
		return (void*) (page_number * 4096);
	}
}
