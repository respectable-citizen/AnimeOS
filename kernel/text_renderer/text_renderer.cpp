#include "text_renderer.hpp"

#include "font.hpp"
#include "../utils.hpp"

namespace TextRenderer {
	namespace {
		GraphicsInfo m_graphics_info;
		uint16_t m_cursor_x;
		uint16_t m_cursor_y;
		uint32_t m_color;
	}

	void initialise(GraphicsInfo graphics_info) {
		m_graphics_info = graphics_info;
		set_cursor_x(0);
		set_cursor_y(0);
		set_color(0xffffff);
	}
	
	void set_cursor_x(uint16_t x) {
		m_cursor_x = x;
	}
	
	void set_cursor_y(uint16_t y) {
		m_cursor_y = y;
	}
	
	void reset_cursor() {
		set_cursor_x(0);
		set_cursor_y(0);
	}
	
	uint16_t cursor_x() {
		return m_cursor_x;
	}
	
	uint16_t cursor_y() {
		return m_cursor_y;
	}
	
	GraphicsInfo graphics_info() {
		return m_graphics_info;
	}

	void set_color(uint32_t color) {
		m_color = color;
	}
	
	uint32_t color() {
		return m_color;
	}
	
	void draw_pixel(uint32_t x, uint32_t y) {
		m_graphics_info.address[y * m_graphics_info.width + x] = color();
	}
	
	void draw_character(char c) {
		int char_index;
		if (c >= ' ' && c <= '~') {
			char_index = c - ' ';
		} else {
			return;
		}
	
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				int array_index = (char_index * 8) + y;
				int bit_index = 8 - x;
				if (((font[array_index] >> bit_index) & 1) == 1) {
					uint32_t cursor_x_offset = m_cursor_x * 7;
					uint32_t cursor_y_offset = m_cursor_y * 10 + 1;
					draw_pixel(x + cursor_x_offset, y + cursor_y_offset);
				}
			}
		}
	
		set_cursor_x(m_cursor_x + 1);
	}
	
	void draw_string(char *text) {
		int i = 0;
		for (;;) {
			char c = text[i++];
			if (c == '\r') set_cursor_x(0);
			if (c == '\n') set_cursor_y(m_cursor_y + 1);
			if (c == '\0') break;
			draw_character(c);
		}
	}
	
	void draw_number(uint64_t number) {
		if (number == 0) {
			draw_character('0');
			return;
		}
		char chars[20];
		chars[19] = '\0';
		int i = 18;
		for (;;) {
			int digit = number % 10;
			chars[i--] = '0' + digit;
			number /= 10;
			if (number == 0) break;
		}
		draw_string(chars + i + 1);
	}
	
	void fill_screen() {
		for (uint32_t x = 0; x < m_graphics_info.width; x++) {
			for (uint32_t y = 0; y < m_graphics_info.height; y++) {
				draw_pixel(x, y);
			}
		}				
	}
	
	void kernel_panic(char *error) {
		reset_cursor();
		set_color(0xff0000);
		fill_screen();
		set_color(0x000000);
		draw_string((char*) "KERNEL PANIC\r\n\r\nError: ");
		draw_string(error);
		hang();
	}

}
