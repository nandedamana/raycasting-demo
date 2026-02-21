#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "map.h"


GrayImage * upscale_map(GrayImage *inmap, int scalefac)
{
	int outw = inmap->w * scalefac;
	int outh = inmap->h * scalefac;

	GrayImage *outmap = (GrayImage *) malloc(sizeof(GrayImage));
	if(outmap == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gray_image_construct(outmap, outw, outh);
	for(int y = 0; y < inmap->h; y += 1) {
		for(int x = 0; x < inmap->w; x += 1) {
			int ytstart = y * scalefac;
			for(int yt = ytstart; yt < (ytstart + scalefac); yt += 1) {
				int xtstart = x * scalefac;
				gray_image_draw_hline(outmap, xtstart, xtstart + (scalefac - 1), yt, gray_image_get_pixel(inmap, x, y));
			}
		}
	}

	GrayImage *_tmp_1;
	_tmp_1 = outmap;
	outmap = NULL;
	return _tmp_1;
}

GrayImage * new_sample_map()
{
	int mapdat[16][20] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, {0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0}, {0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0}, {0, 4, 4, 3, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
	int maxh = 24;

	GrayImage *map = (GrayImage *) malloc(sizeof(GrayImage));
	if(map == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gray_image_construct(map, 20, 16);
	for(int y = 0; y < 16; y += 1) {
		for(int x = 0; x < 20; x += 1) {
			double c = ((double) mapdat[y][x]) / maxh;
			gray_image_set_pixel(map, x, y, c);
		}
	}

	GrayImage *_tmp_1;
	_tmp_1 = map;
	map = NULL;
	return _tmp_1;
}
