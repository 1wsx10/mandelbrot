#include "draw.h"

void draw() {
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
		drawCircle(player1.coord.x, player1.coord.z, player1.radius, 10);
		//draw player line
		glBegin(GL_LINES);
			glVertex3f(player1.coord.x, player1.coord.z, 0);
			glVertex3f(player1.coord.x + sinf(player1.rot) * player1.line, player1.coord.z + cosf(player1.rot) * player1.line, 0);
		glEnd();

	glFlush();
}

void drawCircle(float x, float y, float rad, int num_verts) {

	glBegin(GL_POLYGON);
		double angle;
		for(int i = 0; i < num_verts; i++) {
			angle = i * 2 * PI / num_verts;
			glVertex3f(cos(angle) * rad + x, sin(angle) * rad + y, 0);
		}
	glEnd();
}
