#ifndef PLAYER
#define PLAYER

#include "vector.h"

struct Player {
	v3d coord;
	float rot;

	float radius;//radius of player circle
	float line;//length of line sticking out
};

extern struct Player player1;

#endif
