#ifndef P3763_map_h
#define P3763_map_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "image.h"
GrayImage * upscale_map(GrayImage *inmap, int scalefac);
GrayImage * new_sample_map();

#endif /* P3763_map_h */
