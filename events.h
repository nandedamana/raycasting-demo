#ifndef NAND_RAYCAST_events_h
#define NAND_RAYCAST_events_h

typedef enum Movement Movement;
typedef enum EventType EventType;
typedef struct Event Event;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
enum Movement {
	MOVEMENT_FORWARD,
	MOVEMENT_REVERSE,
	MOVEMENT_GOLEFT,
	MOVEMENT_GORIGHT,
	MOVEMENT_TURNLEFT,
	MOVEMENT_TURNRIGHT,
	MOVEMENT_TURN180,
};

enum EventType {
	EVENT_TYPE_MOVE_PLAYER,
	EVENT_TYPE_RENDER,
};

struct Event {
	EventType type;
	Movement movement;
};

Event event_default();

#endif /* NAND_RAYCAST_events_h */
