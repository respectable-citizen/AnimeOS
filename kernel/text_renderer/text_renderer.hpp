#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <stdint.h>

#include "../../common/graphics_info.h"

namespace TextRenderer {
	namespace {
		GraphicsInfo m_graphics_info;
		uint16_t m_cursor_x;
		uint16_t m_cursor_y;
		uint32_t m_color;
		uint32_t m_scale_factor = 3; //Each pixel of the 8x8 font is actually 3x3 pixels, this might be changd or even decided dynamically in the future
	}
	
	void initialise(GraphicsInfo);
	void set_cursor_x(uint16_t);
	void set_cursor_y(uint16_t);
	void reset_cursor();
	uint16_t cursor_x();
	uint16_t cursor_y();
	GraphicsInfo graphics_info();
	void set_color(uint32_t);
	uint32_t color();
	void draw_character(uint8_t);
	void draw_string(char*);
	void draw_number(uint64_t);
	void fill_screen();
	void kernel_panic(char*);

	inline void draw_pixel(uint32_t x, uint32_t y, bool scale_pixels = false) {
		if (scale_pixels) {
			uint32_t scaled_x = x * m_scale_factor;
			uint32_t scaled_y = y * m_scale_factor;

			for (uint32_t y_offset = 0; y_offset < m_scale_factor; y_offset++) {
				for (uint32_t x_offset = 0; x_offset < m_scale_factor; x_offset++) {
					m_graphics_info.address[(scaled_y + y_offset) * m_graphics_info.pixels_per_scanline + (scaled_x + x_offset)] = color();
				}
			}
		} else {
			m_graphics_info.address[(y * m_graphics_info.pixels_per_scanline) + x] = color();
		}
	}
};

#endif
