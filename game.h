#ifndef NAND_RAYCAST_game_h
#define NAND_RAYCAST_game_h

typedef struct vec2 vec2;
typedef struct Position Position;
typedef struct PlanarPosition PlanarPosition;
typedef struct Player Player;
typedef struct Game Game;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "events.h"
#include "raycast.h"
extern double CLIMBABLE;
extern double EYEFROMFLOOR;
struct vec2 {
	double x;
	double z;
};

struct Position {
	double x;
	double y;
	double z;
	double angle;
};

struct PlanarPosition {
	double x;
	double z;
	double angle;
};

#define MOVESTEP (0.05)
struct Player {
	Position pos;
	vec2 udirvec;
};

struct Game {
	GrayImage *map;
	GrayImage *out;
	GrayImage *raymap;
	Player *player;
};

vec2 vec2_mul(double fac, vec2 vt);
void player_set_position(Player *this, Position newpos);
PlanarPosition player_move_once_dry(Player *this, Movement act);
void player_destruct(Player *this);
void player_construct(Player *this);
void game_construct(Game *this, GrayImage *map);
void game_execute_event(Game *this, Event event);
void game_execute_move_player(Game *this, Movement movement);
void game_execute_render(Game *this);
void game_render(Game *this);
void game_repos_player(Game *this, PlanarPosition newpos);
void game_move_player(Game *this, Movement act);
void game_turn_player(Game *this, double dangle);
void game_destruct(Game *this);
vec2 vec2_default();
Position position_default();
PlanarPosition planar_position_default();

#endif /* NAND_RAYCAST_game_h */
