#include "draw.h"

void draw() {


	glClearColor(0,0,0,0.4);
	glClear(GL_COLOR_BUFFER_BIT);



	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	//printf("x: %d y: %d \t width: %d height: %d\n", vp[0], vp[1], vp[2], vp[3]);

	int width = vp[1];
	int height = vp[2];
	com c;
	unsigned char buffer[width][height][3];
	
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {

			double real = i;
			real *= 4;
			real /= width;
			real -= 2;

			double imag = j;
			imag *= 4;
			imag /= height;
			imag -= 2;

			c.i = imag;
			c.r = real;

			int value = itterate(&c, 50);
			printf("value: %d\n", value);
			if(0 && value == -1) {
				buffer[i][j][0] = buffer[i][j][1] = buffer[i][j][2] = 0;
			} else {
				buffer[i][j][0] = buffer[i][j][1] = buffer[i][j][2] = 255;
			}
		}
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)buffer);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
	glTexCoord2d(1.0, 0.0); glVertex2d(width, 0.0);
	glTexCoord2d(1.0, 1.0); glVertex2d(width, height);
	glTexCoord2d(0.0, 1.0); glVertex2d(0.0, height);
	glEnd();

	//glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);

	//glFlush();
	glutSwapBuffers();
	return;

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
