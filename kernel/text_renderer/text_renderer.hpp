#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <stdint.h>

#include "../../common/graphics_info.h"

namespace TextRenderer {
	void initialise(GraphicsInfo);
	void set_cursor_x(uint16_t);
	void set_cursor_y(uint16_t);
	void reset_cursor();
	uint16_t cursor_x();
	uint16_t cursor_y();
	GraphicsInfo graphics_info();
	void set_color(uint32_t);
	uint32_t color();
	void draw_pixel(uint32_t, uint32_t);
	void draw_character(uint8_t);
	void draw_string(char*);
	void draw_number(uint64_t);
	void fill_screen();
	void kernel_panic(char*);
};

#endif
