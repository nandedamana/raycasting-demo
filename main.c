#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "main.h"


void gui_milli_timer_set_interval(GuiMilliTimer *this, unsigned int freq_in)
{
	this->freq = freq_in;
}

void gui_milli_timer_set_callback(GuiMilliTimer *this, void *callback_in)
{
	this->callback = callback_in;
}

void gui_milli_timer_set_callback_data(GuiMilliTimer *this, void *callback_data_in)
{
	this->callback_data = callback_data_in;
}

void gui_milli_timer_construct(GuiMilliTimer *this, unsigned int interval, void *callback, void *callback_data)
{
	this->srcid = 0u;
	this->freq = interval;

	this->callback = callback;
	this->callback_data = callback_data;
}

void gui_milli_timer_start(GuiMilliTimer *this)
{
	this->srcid = g_timeout_add(this->freq, this->callback, this->callback_data);
}

void gui_milli_timer_stop(GuiMilliTimer *this)
{
	g_source_remove(this->srcid);
	this->srcid = 0;
}

void gui_milli_timer_destruct(GuiMilliTimer *this)
{
}

void app_construct(App *this)
{
	GuiMilliTimer *_tmp_1 = (GuiMilliTimer *) malloc(sizeof(GuiMilliTimer));
	if(_tmp_1 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gui_milli_timer_construct(_tmp_1, 100, on_tick, this);
	this->timer = _tmp_1;
	this->da_out = (GtkDrawingArea *) gtk_drawing_area_new();
	this->da_ray = (GtkDrawingArea *) gtk_drawing_area_new();
	this->activekeys = (ActiveKeys) 0;
	Game *_tmp_2 = (Game *) malloc(sizeof(Game));
	if(_tmp_2 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	game_construct(_tmp_2, upscale_map(new_sample_map(), 20));
	this->game = _tmp_2;

	GtkWindow *mw = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect((GtkWidget *) mw, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	GtkBox *bx = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_container_set_border_width((GtkContainer *) bx, (unsigned int) 4);
	GtkBox *vbx = bx;
	gtk_container_add(GTK_CONTAINER((GtkWidget *) mw), GTK_WIDGET((GtkWidget *) vbx));
	GtkBox *_ngg_tmp_9_bx = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_container_set_border_width((GtkContainer *) _ngg_tmp_9_bx, (unsigned int) 4);
	GtkBox *hbx = _ngg_tmp_9_bx;
	gtk_container_add(GTK_CONTAINER((GtkWidget *) vbx), GTK_WIDGET((GtkWidget *) hbx));
	gtk_container_add(GTK_CONTAINER((GtkWidget *) hbx), GTK_WIDGET((GtkWidget *) this->da_out));
	gtk_widget_set_size_request((GtkWidget *) this->da_out, this->game->out->w, this->game->out->h);
	gtk_container_add(GTK_CONTAINER((GtkWidget *) hbx), GTK_WIDGET((GtkWidget *) this->da_ray));
	gtk_widget_set_size_request((GtkWidget *) this->da_ray, this->game->raymap->w, this->game->raymap->h);
	gtk_widget_show((GtkWidget *) this->da_ray);
	gtk_widget_show((GtkWidget *) this->da_out);
	gtk_widget_show((GtkWidget *) hbx);
	gtk_widget_show((GtkWidget *) vbx);
	gtk_widget_show((GtkWidget *) mw);

	this->surf_out = cairo_image_surface_create(CAIRO_FORMAT_RGB24, this->game->out->w, this->game->out->h);
	this->surf_ray = cairo_image_surface_create(CAIRO_FORMAT_RGB24, this->game->raymap->w, this->game->raymap->h);

	gtk_widget_set_size_request((GtkWidget *) this->da_out, this->game->out->w, this->game->out->h);
	gtk_widget_set_size_request((GtkWidget *) this->da_ray, this->game->raymap->w, this->game->raymap->h);

	g_signal_connect((GtkWidget *) this->da_out, "draw", G_CALLBACK(on_da_paint), this->surf_out);
	g_signal_connect((GtkWidget *) this->da_ray, "draw", G_CALLBACK(on_da_paint), this->surf_ray);

	game_execute_render(this->game);
	gui_milli_timer_start(this->timer);
}

void app_paint(App *this)
{
	app_copy_grayimage_to_cairo_surface(this, this->game->out, this->surf_out);
	app_copy_grayimage_to_cairo_surface(this, this->game->raymap, this->surf_ray);
	gtk_widget_queue_draw((GtkWidget *) this->da_out);
	gtk_widget_queue_draw((GtkWidget *) this->da_ray);
}

void app_copy_grayimage_to_cairo_surface(App *this, GrayImage *img, cairo_surface_t *surf)
{
	int x = 0;
	int y = x;

	unsigned int stride = (unsigned int) cairo_image_surface_get_stride(surf);
	unsigned char *ibuf = cairo_image_surface_get_data(surf);

	assert(img->w == cairo_image_surface_get_width(surf)); /* main.ngg:77 */

	assert(img->h == cairo_image_surface_get_height(surf)); /* main.ngg:78 */

	unsigned int rowpos = 0u;
	for(int y = 0; y < img->h; y += 1) {
		unsigned int pixpos = rowpos;
		for(int x = 0; x < img->w; x += 1) {
			unsigned char *dest = ibuf + ((int) pixpos);
			for(int i = 0; i < 3; i += 1) {
				dest[2 - i] = (unsigned char) (255 * gray_image_get_pixel(img, x, y));
			}

			pixpos = pixpos + 4u;
		}

		rowpos = rowpos + stride;
	}

	cairo_surface_mark_dirty(surf);
}

void app_destruct(App *this)
{
	if(this->game) {
		game_destruct(this->game);
		free(this->game);
	}

	if(this->surf_out) {
		cairo_surface_destroy(this->surf_out);
	}

	if(this->surf_ray) {
		cairo_surface_destroy(this->surf_ray);
	}

	if(this->timer) {
		gui_milli_timer_destruct(this->timer);
		free(this->timer);
	}
}

int main(int argc, char * *argv)
{
	gtk_init(0, NULL);
	App *app = (App *) malloc(sizeof(App));
	if(app == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	app_construct(app);
	gtk_main();
	if(app) {
		app_destruct(app);
		free(app);
	}

	return 0;
}

void on_da_paint(GtkWidget *w, cairo_t *cr, cairo_surface_t *surf)
{
	cairo_set_source_surface(cr, surf, 0, 0);

	cairo_paint(cr);
}

_Bool on_tick(App *app)
{
	game_execute_move_player(app->game, MOVEMENT_TURNRIGHT);
	game_execute_render(app->game);
	app_paint(app);
	return true;
}
