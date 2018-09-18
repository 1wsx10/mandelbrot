#include "loop.h"


void mainloop() {
	
	//movement
	do {
		if(tDown && gDown) { break; }

		if(tDown) {
			player1.coord.x += sinf(player1.rot) * 0.001f;
			player1.coord.z += cosf(player1.rot) * 0.001f;
		}
		if(gDown) {
			player1.coord.x -= sinf(player1.rot) * 0.001f;
			player1.coord.z -= cosf(player1.rot) * 0.001f;
		}
	} while(0);

	//rotation
	do {
		if(hDown && fDown) { break; }

		if(hDown) {
			player1.rot += 0.003f;
		}
		if(fDown) {
			player1.rot -= 0.003f;
		}

		while(player1.rot > 2 * PI) {
			player1.rot -= 2 * PI;
		}
		while(player1.rot < 0) {
			player1.rot += 2 * PI;
		}

	} while(0);

	glutPostRedisplay();
}

