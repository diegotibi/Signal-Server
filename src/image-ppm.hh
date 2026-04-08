#ifndef _IMAGE_PPM_HH
#define _IMAGE_PPM_HH

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "image.hh"

int ppm_init(image_ctx_t *ctx);
int ppm_add_pixel(image_ctx_t *ctx,const uint8_t r,const uint8_t g,const uint8_t b,const uint8_t a);
int ppm_get_pixel(image_ctx_t *ctx,const size_t x,const size_t y,const uint8_t *r,const uint8_t *g,const uint8_t *b,const uint8_t *a);
int ppm_write(image_ctx_t *ctx, FILE* fd);

image_dispatch_table_t ppm_dt = {\
	ppm_init, \
	ppm_add_pixel, \
	NULL, \
	ppm_get_pixel, \
	ppm_write, \
	NULL
};

#endif
