#include "text_renderer.h"

TextRenderer::TextRenderer(GraphicsInfo graphics_info) {
	this->m_graphics_info = graphics_info;
	this->set_cursor_x(0);
	this->set_cursor_y(0);
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
				uint8_t *pixel = (uint8_t*) ((uint64_t) this->m_graphics_info.address + (x * 4) + (y * 4 * this->m_graphics_info.width));
				pixel += this->m_cursor_x * 4 * 7;
				pixel += this->m_cursor_y * this->m_graphics_info.width * 4 * 10;
				pixel[0] = 0xff;
				pixel[1] = 0xff;
				pixel[2] = 0xff;
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
