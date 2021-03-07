#include "text_renderer.h"

TextRenderer::TextRenderer(GraphicsInfo graphics_info) {
	this->m_graphics_info = graphics_info;
	this->set_cursor_x(0);
	this->set_cursor_y(0);
	this->set_color(0xffffff);
}

void TextRenderer::set_cursor_x(uint16_t x) {
	this->m_cursor_x = x;
}

void TextRenderer::set_cursor_y(uint16_t y) {
	this->m_cursor_y = y;
}

uint16_t TextRenderer::cursor_x() {
	return this->m_cursor_x;
}

uint16_t TextRenderer::cursor_y() {
	return this->m_cursor_y;
}

void TextRenderer::set_color(uint32_t color) {
	this->m_color = color;
}

uint32_t TextRenderer::color() {
	return this->m_color;
}

void TextRenderer::draw_pixel(uint32_t x, uint32_t y) {
	this->m_graphics_info.address[y * this->m_graphics_info.width + x] = this->color();
}

bool TextRenderer::draw_character(char c) {
	int char_index;
	if (c >= ' ' && c <= '~') {
		char_index = c - ' ';
	} else {
		return false;
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			int array_index = (char_index * 8) + y;
			int bit_index = 8 - x;
			if (((font[array_index] >> bit_index) & 1) == 1) {
				uint32_t cursor_x_offset = this->m_cursor_x * 7;
				uint32_t cursor_y_offset = this->m_cursor_y * 10;
				this->draw_pixel(x + cursor_x_offset, y + cursor_y_offset);
			}
		}
	}
	return true;
}

void TextRenderer::draw_string(char *text) {
	int i = 0;
	for (;;) {
		char c = text[i++];
		if (c == '\r') this->set_cursor_x(0);
		if (c == '\n') this->set_cursor_y(this->m_cursor_y + 1);
		if (c == '\0') break;
		if (this->draw_character(c)) this->set_cursor_x(this->m_cursor_x + 1); //Only increment cursor if the character is a printing-character (eg, don't try print \r or \n)
	}
}
