#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "image.h"


void gray_image_construct(GrayImage *this, int width, int height)
{
	this->w = width;
	this->h = height;
	float *_tmp_1 = (float *) calloc((size_t) (this->w * this->h), sizeof(float));
	if(_tmp_1 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	this->arr = _tmp_1;
}

void gray_image_clear(GrayImage *this)
{
	for(int y = 0; y < this->h; y += 1) {
		for(int x = 0; x < this->w; x += 1) {
			gray_image_set_pixel(this, x, y, 0);
		}
	}
}

void gray_image_copy_from(GrayImage *this, GrayImage *src)
{
	assert(src->w == this->w); /* image.ngg:28 */
	assert(src->h == this->h); /* image.ngg:29 */

	for(int y = 0; y < this->h; y += 1) {
		for(int x = 0; x < this->w; x += 1) {
			gray_image_set_pixel(this, x, y, gray_image_get_pixel(src, x, y));
		}
	}
}

double gray_image_get_pixel(GrayImage *this, int x, int y)
{
	assert(!gray_image_oob(this, x, y)); /* image.ngg:44 */

	int idx = (this->w * y) + x;
	return this->arr[idx];
}

_Bool gray_image_oob(GrayImage *this, int x, int y)
{
	return (((x < 0) || (y < 0)) || (x >= this->w)) || (y >= this->h);
}

void gray_image_set_pixel(GrayImage *this, int x, int y, float color)
{
	assert(!gray_image_oob(this, x, y)); /* image.ngg:54 */
	int idx = (this->w * y) + x;
	this->arr[idx] = color;
}

void gray_image_try_set_pixel(GrayImage *this, int x, int y, float color)
{
	if(!gray_image_oob(this, x, y)) {
		gray_image_set_pixel(this, x, y, color);
	}
}

void gray_image_draw_hline(GrayImage *this, int x1, int x2, int y, float color)
{
	for(int i = x1; i <= x2; i += 1) {
		gray_image_set_pixel(this, i, y, color);
	}
}

void gray_image_destruct(GrayImage *this)
{
	if(this->arr) {
		free(this->arr);
	}
}
