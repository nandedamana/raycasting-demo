#ifndef NAND_RAYCAST_image_h
#define NAND_RAYCAST_image_h

typedef struct GrayImage GrayImage;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
struct GrayImage {
	float *arr;
	int w;
	int h;
};

void gray_image_construct(GrayImage *this, int width, int height);
void gray_image_clear(GrayImage *this);
void gray_image_copy_from(GrayImage *this, GrayImage *src);
double gray_image_get_pixel(GrayImage *this, int x, int y);
_Bool gray_image_oob(GrayImage *this, int x, int y);
void gray_image_set_pixel(GrayImage *this, int x, int y, float color);
void gray_image_try_set_pixel(GrayImage *this, int x, int y, float color);
void gray_image_draw_hline(GrayImage *this, int x1, int x2, int y, float color);
void gray_image_destruct(GrayImage *this);

#endif /* NAND_RAYCAST_image_h */
