#include "main.h"


void setupTexture() {
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	//printf("x: %d y: %d \t width: %d height: %d\n", vp[0], vp[1], vp[2], vp[3]);

	int width = vp[1];
	int height = vp[2];

	unsigned char buffer[width][height][3];
	
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {


			buffer[i][j][0] = buffer[i][j][1] = buffer[i][j][2] = 0;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glEnable(GL_TEXTURE_2D);
}


void reshape_window(GLsizei w, GLsizei h) {
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}


int main(int argc, char **argv) {

	test_v3d();
	test_com();

	com c;
	c.i = 0;
	c.r = 1;

	printf("mandle(1 + 0i) = %d\n", itterate(&c, 10));

	c.r = -1;
	printf("mandle(-1 + 0i) = %d\n", itterate(&c, 10));

	c.r = -7.0 / 4;
	printf("mandle(-7/4 + 0i) = %d\n", itterate(&c, 10));

	c.r = 1.0 /4;
	printf("mandle(1/4 + 0i) = %d\n", itterate(&c, 10));

	c.r = 1.0 /4 + 1.0 /16;
	printf("mandle(1/4 + 1/16 + 0i) = %d\n", itterate(&c, 50));




#if 0
	md *test;
	test = create_md(3, 3);

	test->m[0][0] = 5;
	test->m[0][1] = 5;
	test->m[0][2] = 5;
	test->m[1][0] = 5;
	test->m[1][1] = 5;
	test->m[1][2] = 5;
	test->m[2][0] = 5;
	test->m[2][1] = 5;
	test->m[2][2] = 5;
	
	print_md(test);
#endif

	player1.coord.x = 0.0f;
	player1.coord.z = 0.0f;
	player1.rot = 0.0f;
	player1.radius = 0.1f;
	player1.line = 0.2f;

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_SINGLE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Mandlebrot");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutIdleFunc(mainloop);
	glutReshapeFunc(reshape_window);

	setupTexture();

	glutMainLoop();

	return EXIT_SUCCESS;

}

