#ifndef PLAYER
#define PLAYER

struct Player {
	float coord[2];
	float rot;

	float radius;//radius of player circle
	float line;//length of line sticking out
};

extern struct Player player1;

#endif
