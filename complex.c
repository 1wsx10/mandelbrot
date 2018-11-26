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
	//= rr + 2ri - ii
	double rr = a->r * a->r;
	double ri = a->r * a->i;
	double ii = a->i * a->i * -1;//i^2 = -1

	ret->r = rr + ii;
	ret->i = ri * 2;
}

double magnitude_squared_com(com *a) {
	return a->i * a->i + a->r + a->r;
}

void print_com(com *a) {
	printf("%.2d + %.2di", a->r, a->i);
}
