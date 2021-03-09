#ifndef GRAPHICS_INFO_H
#define GRAPHICS_INFO_H

typedef struct {
	uint32_t *address;
	uint64_t buffer_size;
	uint64_t width;
	uint64_t height;
	uint64_t pixels_per_scanline;
} GraphicsInfo;

#endif
