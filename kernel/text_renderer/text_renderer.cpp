#include "text_renderer.hpp"

#include "font.hpp"
#include "utils.hpp"

namespace TextRenderer {
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
		
	void draw_character(uint8_t c) {
		if (c < 32 || c > 126) return; //Don't try print the character if it's outside of the printable range

		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				bool pixel_bit = (font[c][y] >> x) & 1;
				if (pixel_bit) {
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
		char chars[30];
		chars[29] = '\0';
		int i = 28;
		for (;;) {
			int digit = number % 10;
			chars[i--] = '0' + digit;
			number /= 10;
			if (number == 0) break;
		}
		draw_string(chars + i + 1);
	}
	
	void fill_screen() {
		for (uint32_t y = 0; y < m_graphics_info.height; y++) {
			for (uint32_t x = 0; x < m_graphics_info.width; x++) {
				draw_pixel(x, y);
			}
		}				
	}
	
	void kernel_panic(char *error) {
		//Draw background
		for (uint64_t y = 0; y < m_graphics_info.height; y++) {
			for (uint64_t x = 0; x < m_graphics_info.width; x++) {
				uint8_t r = (x * 2) % 64;
				uint8_t g = (y * 2) % 64;
				uint8_t b = ((x + y) * 2) % 64;
				m_graphics_info.address[x + (y * m_graphics_info.pixels_per_scanline)] = (r << 16) | (g << 8) | b;
			}
		}

		uint64_t rax;
		uint64_t rbx;
		uint64_t rcx;
		uint64_t rdx;
		uint64_t rbp;
		uint64_t rsp;
		uint64_t rsi;
		uint64_t rdi;

		asm volatile ("mov %%rax, %0": "=g"(rax)::"memory");
		asm volatile ("mov %%rbx, %0": "=g"(rbx)::"memory");
		asm volatile ("mov %%rcx, %0": "=g"(rcx)::"memory");
		asm volatile ("mov %%rdx, %0": "=g"(rdx)::"memory");
		asm volatile ("mov %%rbp, %0": "=g"(rbp)::"memory");
		asm volatile ("mov %%rsp, %0": "=g"(rsp)::"memory");
		asm volatile ("mov %%rsi, %0": "=g"(rsi)::"memory");
		asm volatile ("mov %%rdi, %0": "=g"(rdi)::"memory");
		
		//Draw kernel panic
		reset_cursor();
		set_color(0xffffff);
		draw_string((char*) "KERNEL PANIC\r\n\r\nError: ");
		draw_string(error);

		//Draw registers
		draw_string((char*) "\r\n\r\nRAX: ");
		draw_number(rax);

		draw_string((char*) "\r\nRBX: ");
		draw_number(rbx);

		draw_string((char*) "\r\nRCX: ");
		draw_number(rcx);

		draw_string((char*) "\r\nRDX: ");
		draw_number(rdx);

		draw_string((char*) "\r\nRBP: ");
		draw_number(rbp);

		draw_string((char*) "\r\nRSP: ");
		draw_number(rsp);

		draw_string((char*) "\r\nRSI: ");
		draw_number(rsi);

		draw_string((char*) "\r\nRDI: ");
		draw_number(rdi);

		/*
		struct stackframe {
			struct stackframe* rbp;
			uint64_t rip;
		};

		uint64_t max_frames = 5;
		stackframe *frame = (stackframe*) rbp;
		draw_string((char*) "\r\nstack trace:\r\n");
		draw_string((char*) "rip: \r\n");
		draw_number((uint64_t) &max_frames);
		draw_string((char*) "\r\n");
		for (uint64_t frame_count = 0; frame && frame_count < max_frames; frame_count++) {
			draw_string((char*) "rip: \r\n");
			draw_number(frame->rip);
			draw_string((char*) "\r\n");
			//if (stk->rbp == 0) break;
			frame = frame->rbp;
		}*/

		hang();
	}

}
