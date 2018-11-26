#include "main.h"

int main(int argc, char **argv) {

	test_v3d();

	printf("testing complex number functions\n");
	com *test;
	com *test2;
	com *ret;
	com *ret2;
	test = create_com(4, 7);
	test2 = create_com(3, 8);
	ret = create_com(0, 0);
	ret2 = create_com(0, 0);

	add_com(test, test2, ret);
	if(ret->r != 7 || ret->i != 15) {
		printf("test failed: (4 + 7i) + (3 + 8i) = (7 + 15i). got:\n");
		print_com(ret);
	}

	//(4 + 7i)(4 + 7i)
	//16 + 56i - 49
	//-33 + 56i
	sqr_com(test, ret2);
	if(ret2->r != -33 || ret2->i != 56) {
		printf("test failed: (4 + 7i)^2 = (-33 + 56i). got:\n");
		print_com(ret2);
	}

	// (4 + 7i) * (3 + 8i)
	// 12 + 32i + 21i + 56ii
	// 12 + 53i - 56
	// -44 + 53i
	mul_comcom(test, test2, test2);
	if(test2->r != -44 || test2->i != 53) {
		printf("test failed: (4 + 7i) * (3 + 8i) = (-44 + 53i). got:\n");
		print_com(test2);
	}

	free(test);
	free(test2);
	free(ret);
	free(ret2);
	printf("done testing complex numbers\n");

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

