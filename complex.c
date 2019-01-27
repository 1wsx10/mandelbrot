#include "complex.h"


com * create_com(double real, double imag) {
	com *ret;
	ret = malloc(sizeof(*ret));

	// check for failure
	if(ret == NULL) {
		ret = NULL;
	} else {
		ret->r = real;
		ret->i = imag;
	}

	return ret;
}

void add_com(com *a, com *b, com *ret) {
	ret->i = a->i + b->i;
	ret->r = a->r + b->r;
}

void sub_com(com *a, com *b, com *ret) {
	ret->i = a->i - b->i;
	ret->r = a->r - b->r;
}

void mul_comsc(com *a, double b, com *ret) {
	ret->i = a->i * b;
	ret->r = a->r * b;
}

void mul_comcom(com *a, com *b, com *ret) {
	// (r1 + i1)(r2 + i2)

	// this way is clearer, and doesn't mangle the input if input and output are the same ptr
	double r1r2 = a->r * b->r;
	double r1i2 = a->r * b->i;
	double i1r2 = a->i * b->r;
	double i1i2 = a->i * b->i * -1;//i^2 = -1

	ret->r = r1r2 + i1i2;
	ret->i = r1i2 + i1r2; 
}

void sqr_com(com *a, com *ret) {
	// (r + i)(r + i)
	double rr = a->r * a->r;
	double ri = a->r * a->i;
	double ii = a->i * a->i * -1;//i^2 = -1

	//= rr + 2ri - ii
	ret->r = rr + ii;
	ret->i = ri * 2;
}

double magnitude_squared_com(com *a) {
	return a->i * a->i + a->r * a->r;
}

void test_com_stack() {
	com test;
	com test2;
	com ret;
	com ret2;
	com ret3;
	test.r = 4;
	test.i = 7;
	test2.r = 3;
	test2.i = 8;

	add_com(&test, &test2, &ret);
	if(ret.r != 7 || ret.i != 15) {
		printf("test failed: (4 + 7i) + (3 + 8i) = (7 + 15i). got: ");
		println_com(&ret);
	} else {
		printf("add test passed\n");
	}

	//(4 + 7i)(4 + 7i)
	//16 + 56i - 49
	//-33 + 56i
	sqr_com(&test, &ret2);
	if(ret2.r != -33 || ret2.i != 56) {
		printf("test failed: (4 + 7i)^2 = (-33 + 56i). got: ");
		println_com(&ret2);
	} else {
		printf("sqr test passed\n");
	}

	// (4 + 7i) * (3 + 8i)
	// 12 + 32i + 21i + 56ii
	// 12 + 53i - 56
	// -44 + 53i
	mul_comcom(&test, &test2, &ret3);
	if(ret3.r != -44 || ret3.i != 53) {
		printf("test failed: (4 + 7i) * (3 + 8i) = (-44 + 53i). got: ");
		println_com(&ret3);
	} else {
		printf("mul test passed\n");
	}

	// magnitude squared
	// 4+7i
	// (4^2 + 7^2) = 65
	int mag_sqr = magnitude_squared_com(&test);
	if(mag_sqr != 65) {
		printf("test failed: (4^2 + 7i^2) = 65. got: %d\n", mag_sqr);
	} else {
		printf("mag_sqr test passed\n");
	}

}

void test_com_heap() {
	com *test;
	com *test2;
	com *ret;
	com *ret2;
	com *ret3;
	test = create_com(4, 7);
	test2 = create_com(3, 8);
	ret = create_com(0, 0);
	ret2 = create_com(0, 0);
	ret3 = create_com(0, 0);

	add_com(test, test2, ret);
	if(ret->r != 7 || ret->i != 15) {
		printf("test failed: (4 + 7i) + (3 + 8i) = (7 + 15i). got: ");
		println_com(ret);
	} else {
		printf("add test passed\n");
	}

	//(4 + 7i)(4 + 7i)
	//16 + 56i - 49
	//-33 + 56i
	sqr_com(test, ret2);
	if(ret2->r != -33 || ret2->i != 56) {
		printf("test failed: (4 + 7i)^2 = (-33 + 56i). got: ");
		println_com(ret2);
	} else {
		printf("sqr test passed\n");
	}

	// (4 + 7i) * (3 + 8i)
	// 12 + 32i + 21i + 56ii
	// 12 + 53i - 56
	// -44 + 53i
	mul_comcom(test, test2, ret3);
	if(ret3->r != -44 || ret3->i != 53) {
		printf("test failed: (4 + 7i) * (3 + 8i) = (-44 + 53i). got: ");
		println_com(ret3);
	} else {
		printf("mul test passed\n");
	}

	// magnitude squared
	// 4+7i
	// (4^2 + 7^2) = 65
	int mag_sqr = magnitude_squared_com(test);
	if(mag_sqr != 65) {
		printf("test failed: (4^2 + 7i^2) = 65. got: %d\n", mag_sqr);
	} else {
		printf("mag_sqr test passed\n");
	}
	

	free(test);
	free(test2);
	free(ret);
	free(ret2);
	free(ret3);
}

void println_com(com *a) {
	printf("%.2f + %.2fi\n", a->r, a->i);
}

void print_com(com *a) {
	printf("%.2f + %.2fi", a->r, a->i);
}
