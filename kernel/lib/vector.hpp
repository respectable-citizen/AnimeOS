#pragma once

#include "memory.hpp"
#include "../memory_manager/pmm.hpp"
#include "../text_renderer/text_renderer.hpp"

template <class T>
class Vector {
private:
	T *items;
	uint64_t current_item_capacity;
	uint64_t current_size;

public:
	Vector() {
		current_item_capacity = 64;
		current_size = 0;
		items = (T*) PMM::allocate_kernel_pages(PMM::bytes_to_pages(current_item_capacity * sizeof(T)));
	}

	void push(T item) {
		if (current_size >= current_item_capacity) {
			uint64_t size_bytes = current_size * sizeof(T);
			uint64_t size_pages = PMM::bytes_to_pages(size_bytes);
			uint64_t new_size_pages = size_pages * 2;

			T *new_items = (T*) PMM::allocate_kernel_pages(new_size_pages);
			memcpy((void*) new_items, (void*) items, size_bytes);
			PMM::free_pages(PMM::address_to_page_number(items), size_pages);
			items = new_items;
			current_item_capacity *= 2;
		}

		items[current_size++] = item;
	}

	void remove(uint64_t i) {
		if (i == (current_size - 1)) {
			//We're removing the last item, so we can just pop
			pop();
			return;
		}

		if (i >= current_size) TextRenderer::kernel_panic((char*) "Out of bounds remove in vector");
		uint64_t size = current_size - i - 1;
		if (size == 0) return;
		memcpy((void*) (items + i), (void*) (items + i + 1), size * sizeof(T));
		current_size--;
	}

	void pop() {
		current_size--;
	}

	void insert(uint64_t index, T value) {
		if (index > current_size) TextRenderer::kernel_panic((char*) "Out of bounds insert in vector");

		if (index == current_size) {
			//If you try to insert at the next index which doesn't exist, then we should just add it to the end
			push(value);
		} else {
		//This is all a pretty hacky way of making the list expand if necessary, any better way to do this?
			T last_element = items[current_size - 1];
			memmove((void*) (items + index + 1), (void*) (items + index), (current_size - index) * sizeof(T));
			items[index] = value;
			push(last_element);
		}
	}

	void set(uint64_t index, T value) {
		items[index] = value;
	}

	T operator[](uint64_t i) {
		if (i >= current_size) TextRenderer::kernel_panic((char*) "Out of bounds read in vector");
		return items[i];
	}

	uint64_t size() {
		return current_size;
	}

	bool empty() {
		return current_size == 0;
	} 
}; 
