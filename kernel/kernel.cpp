#include <efi.h>

#include "../common/graphics_info.h"
#include "../common/memory_map.h"

#include "memory.h"

#include "text_renderer/text_renderer.h"

extern "C" void entry_point(MemoryMap memory_map, GraphicsInfo graphics_info) {
	TextRenderer text_renderer(graphics_info);
	for (int i = 0; i < 128; i++) {
		if (text_renderer.draw_character((char) i)) text_renderer.set_cursor_x(text_renderer.cursor_x() + 1);
	}
	for (;;);
}
