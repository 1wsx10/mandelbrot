#include "main.h"

int main(int argc, char **argv) {

	test_v3d();
	test_com();

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
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL - Creating a triangle");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutIdleFunc(mainloop);
	glutMainLoop();

	return EXIT_SUCCESS;

}

