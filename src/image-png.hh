#ifndef _IMAGE_PNG_HH
#define _IMAGE_PNG_HH

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "image.hh"

int png_init(image_ctx_t *ctx);
int png_add_pixel(image_ctx_t *ctx,const uint8_t r,const uint8_t g,const uint8_t b,const uint8_t a);
int png_get_pixel(image_ctx_t *ctx,const size_t x,const size_t y,const uint8_t *r,const uint8_t *g,const uint8_t *b,const uint8_t *a);
int png_write(image_ctx_t *ctx, FILE* fd);

image_dispatch_table_t png_dt = {\
	png_init, \
	png_add_pixel, \
	NULL, \
	png_get_pixel, \
	png_write, \
	NULL
};

#endif
