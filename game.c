#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "game.h"


double CLIMBABLE = 0.01;

double EYEFROMFLOOR = 0.04;

vec2 vec2_mul(double fac, vec2 vt)
{
	return (vec2) { fac * vt.x, fac * vt.z };
}

void player_set_position(Player *this, Position newpos)
{
	this->pos = newpos;
	this->udirvec.x = cos(this->pos.angle);
	this->udirvec.z = sin(this->pos.angle);
}

PlanarPosition player_move_once_dry(Player *this, Movement act)
{
	PlanarPosition newpos = (PlanarPosition) { this->pos.x, this->pos.z, this->pos.angle };
	switch(act) {
	case MOVEMENT_FORWARD:
	{
		vec2 dirvec = vec2_mul(MOVESTEP, this->udirvec);
		newpos.x += dirvec.x;
		newpos.z += dirvec.z;
		break;
	}
	case MOVEMENT_REVERSE:
	{
		vec2 dirvec = vec2_mul(MOVESTEP, this->udirvec);
		newpos.x -= dirvec.x;
		newpos.z -= dirvec.z;
		break;
	}
	case MOVEMENT_GOLEFT:
	{
		vec2 dirvec = vec2_mul(MOVESTEP, this->udirvec);
		newpos.x += dirvec.z;
		newpos.z += (0 - dirvec.x);
		break;
	}
	case MOVEMENT_GORIGHT:
	{
		vec2 dirvec = vec2_mul(MOVESTEP, this->udirvec);
		newpos.x -= dirvec.z;
		newpos.z -= (0 - dirvec.x);
		break;
	}
	case MOVEMENT_TURNLEFT:
	{
		newpos.angle -= MOVESTEP;
		break;
	}
	case MOVEMENT_TURNRIGHT:
	{
		newpos.angle += MOVESTEP;
		break;
	}
	case MOVEMENT_TURN180:
	{
		newpos.angle += M_PI;
		break;
	}
	}

	return newpos;
}

void player_destruct(Player *this)
{
}

void player_construct(Player *this)
{
	this->udirvec = vec2_default();
	this->pos = position_default();
}

void game_construct(Game *this, GrayImage *map)
{
	Player *_tmp_1 = (Player *) malloc(sizeof(Player));
	if(_tmp_1 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	player_construct(_tmp_1);
	this->player = _tmp_1;
	this->map = map;
	player_set_position(this->player, (Position) { 0, 1, 0, 0 });
	game_repos_player(this, (PlanarPosition) { 0, 0, 0 });
	GrayImage *_tmp_2 = (GrayImage *) malloc(sizeof(GrayImage));
	if(_tmp_2 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gray_image_construct(_tmp_2, 640, 640);
	this->out = _tmp_2;

	GrayImage *_tmp_3 = (GrayImage *) malloc(sizeof(GrayImage));
	if(_tmp_3 == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gray_image_construct(_tmp_3, map->w, map->h);
	this->raymap = _tmp_3;
}

void game_execute_event(Game *this, Event event)
{
	switch(event.type) {
	case EVENT_TYPE_MOVE_PLAYER:
	{
		game_move_player(this, event.movement);
		break;
	}
	case EVENT_TYPE_RENDER:
	{
		game_render(this);
		break;
	}
	}
}

void game_execute_move_player(Game *this, Movement movement)
{
	Event event = event_default();
	event.type = EVENT_TYPE_MOVE_PLAYER;
	event.movement = movement;
	game_execute_event(this, event);
}

void game_execute_render(Game *this)
{
	Event event = event_default();
	event.type = EVENT_TYPE_RENDER;
	game_execute_event(this, event);
}

void game_render(Game *this)
{
	render_stateless(this->out, this->map, this->raymap, this->player->pos.x, this->player->pos.y + EYEFROMFLOOR, this->player->pos.z, M_PI / 2, this->player->pos.angle);
}

void game_repos_player(Game *this, PlanarPosition newpos)
{
	_ngg_tuple_playercoords_to_mapcoords _ngg_tmp_0 = playercoords_to_mapcoords(newpos.x, newpos.z, this->map);
	double mapy = _ngg_tmp_0.m1;
	double mapx = _ngg_tmp_0.m0;
	if(gray_image_oob(this->map, (int) mapx, (int) mapy)) {
		return;
	}

	double newloch = gray_image_get_pixel(this->map, (int) mapx, (int) mapy);
	if(newloch > (this->player->pos.y + CLIMBABLE)) {
		return;
	}

	player_set_position(this->player, (Position) { newpos.x, newloch, newpos.z, newpos.angle });
}

void game_move_player(Game *this, Movement act)
{
	PlanarPosition newpos = player_move_once_dry(this->player, act);
	game_repos_player(this, newpos);
}

void game_turn_player(Game *this, double dangle)
{
	PlanarPosition newpos = (PlanarPosition) { this->player->pos.x, this->player->pos.z, this->player->pos.angle };
	newpos.angle -= dangle;
	game_repos_player(this, newpos);
}

void game_destruct(Game *this)
{
	if(this->map) {
		gray_image_destruct(this->map);
		free(this->map);
	}

	if(this->out) {
		gray_image_destruct(this->out);
		free(this->out);
	}

	if(this->raymap) {
		gray_image_destruct(this->raymap);
		free(this->raymap);
	}

	if(this->player) {
		player_destruct(this->player);
		free(this->player);
	}
}

vec2 vec2_default()
{
	vec2 s;
	s.x = 0.0;
	s.z = 0.0;
	return s;
}

Position position_default()
{
	Position s;
	s.x = 0.0;
	s.y = 0.0;
	s.z = 0.0;
	s.angle = 0.0;
	return s;
}

PlanarPosition planar_position_default()
{
	PlanarPosition s;
	s.x = 0.0;
	s.z = 0.0;
	s.angle = 0.0;
	return s;
}
