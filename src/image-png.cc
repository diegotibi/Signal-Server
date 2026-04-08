#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <png.h>
#include "image.hh"

int png_init(image_ctx_t *ctx){
	size_t buf_size;
	size_t pixel_size;

	if(ctx->canvas != NULL)
		return EINVAL;

	ctx->format = IMAGE_PNG;
	ctx->extension = (char*)".png";
	if (ctx->model != IMAGE_RGB && ctx->model != IMAGE_RGBA)
		return EINVAL;
	pixel_size = (ctx->model == IMAGE_RGBA ? RGBA_SIZE : RGB_SIZE);

	buf_size = ctx->width * ctx->height * pixel_size;

	ctx->canvas = (uint8_t*) calloc(buf_size,sizeof(uint8_t));
	ctx->next_pixel = ctx->canvas;
	if(ctx->canvas == NULL)
		return ENOMEM;

	return 0;
}

int png_add_pixel(image_ctx_t *ctx,const uint8_t r,const uint8_t g,const uint8_t b,const uint8_t a){
	uint8_t* next;

	next = ctx->next_pixel;
	next[0] = r;
	next[1] = g;
	next[2] = b;
	if (ctx->model == IMAGE_RGBA) {
		next[3] = a;
		ctx->next_pixel += RGBA_SIZE;
	} else {
		ctx->next_pixel += RGB_SIZE;
	}

	return 0;
}

int png_get_pixel(image_ctx_t *ctx,const size_t x,const size_t y,const uint8_t *r,const uint8_t *g,const uint8_t *b,const uint8_t *a){
	(void)ctx;
	(void)x;
	(void)y;
	(void)r;
	(void)g;
	(void)b;
	(void)a;
	return 0;
}

int png_write(image_ctx_t *ctx, FILE* fd){
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	std::vector<png_bytep> rows;
	size_t rowbytes;
	int color_type;
	size_t pixel_size;
	png_text text[4];
	char north_str[64], east_str[64], south_str[64], west_str[64];

	if(ctx == NULL || ctx->canvas == NULL || fd == NULL)
		return EINVAL;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
		return ENOMEM;

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		return ENOMEM;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return EIO;
	}

	png_init_io(png_ptr, fd);
	color_type = (ctx->model == IMAGE_RGBA ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB);
	pixel_size = (ctx->model == IMAGE_RGBA ? RGBA_SIZE : RGB_SIZE);
	png_set_IHDR(png_ptr, info_ptr,
		(png_uint_32)ctx->width,
		(png_uint_32)ctx->height,
		8,
		color_type,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE);

	if (ctx->has_geo_bounds) {
		snprintf(north_str, sizeof(north_str), "%.6f", ctx->north);
		snprintf(east_str, sizeof(east_str), "%.6f", ctx->east);
		snprintf(south_str, sizeof(south_str), "%.6f", ctx->south);
		snprintf(west_str, sizeof(west_str), "%.6f", ctx->west);

		memset(text, 0, sizeof(text));
		text[0].compression = PNG_TEXT_COMPRESSION_NONE;
		text[0].key = (char*)"north";
		text[0].text = north_str;
		text[1].compression = PNG_TEXT_COMPRESSION_NONE;
		text[1].key = (char*)"east";
		text[1].text = east_str;
		text[2].compression = PNG_TEXT_COMPRESSION_NONE;
		text[2].key = (char*)"south";
		text[2].text = south_str;
		text[3].compression = PNG_TEXT_COMPRESSION_NONE;
		text[3].key = (char*)"west";
		text[3].text = west_str;
		png_set_text(png_ptr, info_ptr, text, 4);
	}

	png_write_info(png_ptr, info_ptr);

	rowbytes = ctx->width * pixel_size;
	rows.resize(ctx->height);
	for (size_t y = 0; y < ctx->height; ++y) {
		rows[y] = (png_bytep)(ctx->canvas + (y * rowbytes));
	}

	png_write_image(png_ptr, rows.data());
	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return 0;
}
