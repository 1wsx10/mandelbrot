#include "keyboard.h"

int tDown = 0;
int gDown = 0;
int fDown = 0;
int hDown = 0;


void keyDown(unsigned char key, int x, int y) {
	switch(key) {
		//quit
		case 'Q':
		case 'q':
			glutLeaveMainLoop();
			break;

		//arrow keys
		case 't':
		case 'T':
			tDown = 1;
			break;
		case 'g':
		case 'G':
			gDown = 1;
			break;
		case 'f':
		case 'F':
			fDown = 1;
			break;
		case 'h':
		case 'H':
			hDown = 1;
			break;
	}
}

void keyUp(unsigned char key, int x, int y) {
	switch(key) {

		//arrow keys
		case 't':
		case 'T':
			tDown = 0;
			break;
		case 'g':
		case 'G':
			gDown = 0;
			break;
		case 'f':
		case 'F':
			fDown = 0;
			break;
		case 'h':
		case 'H':
			hDown = 0;
			break;
	}
}

