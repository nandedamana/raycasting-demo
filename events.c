#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "events.h"


Event event_default()
{
	Event s;
	s.type = EVENT_TYPE_MOVE_PLAYER;
	s.movement = MOVEMENT_FORWARD;
	return s;
}
