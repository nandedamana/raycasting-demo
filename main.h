#ifndef P3763_main_h
#define P3763_main_h

typedef struct GuiMilliTimer GuiMilliTimer;
typedef enum ActiveKeys ActiveKeys;
typedef struct App App;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <cairo.h>
_Bool GuiMilliTimerCallback(void *user_data);
struct GuiMilliTimer {
	unsigned int srcid;
	unsigned int freq;
	void *callback;
	void *callback_data;
};

#include "game.h"
#include "./map.h"
#include "events.h"
enum ActiveKeys {
	ACTIVE_KEYS_up = 1,
	ACTIVE_KEYS_down = 2,
	ACTIVE_KEYS_left = 4,
	ACTIVE_KEYS_right = 8,
	ACTIVE_KEYS_comma = 16,
	ACTIVE_KEYS_period = 32,
};

struct App {
	GtkDrawingArea *da_ray;
	GtkDrawingArea *da_out;
	Game *game;
	cairo_surface_t *surf_out;
	cairo_surface_t *surf_ray;
	ActiveKeys activekeys;
	GuiMilliTimer *timer;
};

void gui_milli_timer_set_interval(GuiMilliTimer *this, unsigned int freq_in);
void gui_milli_timer_set_callback(GuiMilliTimer *this, void *callback_in);
void gui_milli_timer_set_callback_data(GuiMilliTimer *this, void *callback_data_in);
void gui_milli_timer_construct(GuiMilliTimer *this, unsigned int interval, void *callback, void *callback_data);
void gui_milli_timer_start(GuiMilliTimer *this);
void gui_milli_timer_stop(GuiMilliTimer *this);
void gui_milli_timer_destruct(GuiMilliTimer *this);
void app_construct(App *this);
void app_paint(App *this);
void app_copy_grayimage_to_cairo_surface(App *this, GrayImage *img, cairo_surface_t *surf);
void app_destruct(App *this);
int main(int argc, char * *argv);
void on_da_paint(GtkWidget *w, cairo_t *cr, cairo_surface_t *surf);
_Bool on_tick(App *app);

#endif /* P3763_main_h */
