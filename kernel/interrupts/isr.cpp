#include "isr.hpp"

#include "../text_renderer/text_renderer.hpp"

#include "../utils.hpp"

__attribute((interrupt)) void isr8(void *p) {
	TextRenderer::draw_string((char*) "double fault!");
	for (;;);
	p = (void*) 1;
	//hang();
}

__attribute((interrupt)) void isr13(void *p) {
	TextRenderer::draw_string((char*) "page fault!");
	for (;;);
	p = (void*) 1;
	//hang();
}

__attribute((interrupt)) void isr14(void *p) {
	TextRenderer::draw_string((char*) "page fault!");
	for (;;);
	p = (void*) 1;
	//hang();
}

__attribute((interrupt)) void isr15(void *p) {
	TextRenderer::draw_string((char*) "page fault!");
	for (;;);
	p = (void*) 1;
	//hang();
}
