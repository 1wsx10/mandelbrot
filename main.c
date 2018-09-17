#include "main.h"

int main(int argc, char **argv) {

	player1.coord[0] = 0.0f;
   player1.coord[1] = 0.0f;
	player1.rot = 0.0f;
	player1.radius = 0.1f;
	player1.line = 0.2f;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	//glutInitWindowSize(500, 500);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL - Creating a triangle");
	glutDisplayFunc(drawTriangle);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutIdleFunc(mainloop);
	glutMainLoop();

	return EXIT_SUCCESS;

}

void mainloop() {
	
	//vertical
	do {
		if(tDown && gDown) break;

		if(tDown) {
			player1.coord[1] += 0.001f;
		}
		if(gDown) {
			player1.coord[1] -= 0.001f;
		}
	} while(0);
	//horizontal
	do {
		if(hDown && fDown) break;

		if(hDown) {
			player1.coord[0] += 0.001f;
		}
		if(fDown) {
			player1.coord[0] -= 0.001f;
		}
	} while(0);

	glutPostRedisplay();
}

void drawTriangle() {
	glClearColor(0,0,0,0.4);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

		/*glBegin(GL_LINES);
			glVertex3f(-0.7, 0.7, 0);
			glVertex3f(corner[0], corner[1], corner[2]);
		glEnd();


		glBegin(GL_TRIANGLES);
			glVertex3f(-0.7, 0.7, 0);
			glVertex3f(0.0, -1.0, 0.0);
			glVertex3f(corner[0], corner[1], corner[2]);
		glEnd();*/

		//draw player circle
		glBegin(GL_POLYGON);
			for(double i = 0; i < 2 * PI; i += PI / 20) {
				glVertex3f(cos(i) * player1.radius + player1.coord[0], sin(i) * player1.radius + player1.coord[1], 0);
			}
		glEnd();
		//draw player line

	glFlush();
}


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

