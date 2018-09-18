#include "vector.h"

void add_v3d(v3d a, v3d b, v3d *ret) {
	ret->a = a.a + b.a;
	ret->b = a.b + b.b;
	ret->c = a.c + b.c;
}

void sub_v3d(v3d a, v3d b, v3d *ret) {
	ret->a = a.a - b.a;
	ret->b = a.b - b.b;
	ret->c = a.c - b.c;
}

void mul_v3d(v3d a, double b, v3d *ret) {
	ret->a = a.a * b;
	ret->b = a.b * b;
	ret->c = a.c * b;
}

double dot_v3d(v3d a, v3d b) {
	return a.a * b.a + a.b * b.b + a.c * b.c;
}

double det(double a, double b, double c, double d) {
	return a * d - b * c;
}

void cro_v3d(v3d a, v3d b, v3d *ret) {
	double i = det(a.b, a.c, b.b, b.c);
	double j = -1 * det(a.a, a.c, b.a, b.c);
	double k = det(a.a, a.b, b.a, b.b);

	ret->a = i;
	ret->b = j;
	ret->c = k;
}

double len_v3d(v3d a) {
	return sqrt(lsq_v3d(a));
}

double lsq_v3d(v3d a) {
	return a.a * a.a + a.b * a.b + a.c * a.c;
}

void nrm_v3d(v3d a, v3d *ret) {
	double len = len_v3d(a);
	
	ret->a = a.a / len;
	ret->b = a.b / len;
	ret->c = a.c / len;
}

void prj_v3d(v3d a, v3d b, v3d *ret) {
	//a|| = (a . b^) * b^
	v3d bhat;
	nrm_v3d(b, &bhat);

	double dot = dot_v3d(a, bhat);
	mul_v3d(bhat, dot, ret);
}

void rej_v3d(v3d a, v3d b, v3d *ret) {
	v3d parallel;
	prj_v3d(a, b, &parallel);
	sub_v3d(a, parallel, ret);
}

#define EQ_DOUBLE(X, Y) (fabs((X) - (Y)) < 0.0001)

int test_v3d() {
	v3d a, b;
	a.a = 5.1, a.b = 2.7, a.c = 3;
	b.a = 3, b.b = 7, b.c = 4.2;

	double scalar = 2.34;
	double answer;

	v3d ret;

	add_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.a, 8.1) && EQ_DOUBLE(ret.b, 9.7) && EQ_DOUBLE(ret.c, 7.2))) {
		fprintf(stderr, "v3d add\nFILE: %s\nLINE: %d\nFUNCTION: %s\n", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	sub_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.a, 2.1) && EQ_DOUBLE(ret.b, -4.3) && EQ_DOUBLE(ret.c, -1.2))) {
		fprintf(stderr, "v3d sub\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	mul_v3d(a, scalar, &ret);

	if(!(EQ_DOUBLE(ret.a, 11.934) && EQ_DOUBLE(ret.b, 6.318) && EQ_DOUBLE(ret.c, 7.02))) {
		fprintf(stderr, "v3d mul\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	answer = dot_v3d(a, b);

	if(!EQ_DOUBLE(answer, 46.8)) {
		fprintf(stderr, "v3d dot\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "ans: %f\n", answer);
		return 0;
	}

	cro_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.a, -9.66) && EQ_DOUBLE(ret.b, -12.42) && EQ_DOUBLE(ret.c, 27.6))) {
		fprintf(stderr, "v3d cro\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	answer = lsq_v3d(a);

	if(!EQ_DOUBLE(answer, 42.3)) {
		fprintf(stderr, "v3d length squared\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "ans: %f\n", answer);
		return 0;
	}

	answer = len_v3d(a);

	if(!EQ_DOUBLE(answer, 6.503845016)) {
		fprintf(stderr, "v3d length \nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "ans: %f\n", answer);
		return 0;
	}

	nrm_v3d(a, &ret);

	if(!(EQ_DOUBLE(ret.a, 0.784151526) && EQ_DOUBLE(ret.b, 0.415139043) && EQ_DOUBLE(ret.c, 0.461265604))) {
		fprintf(stderr, "v3d normalized\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	prj_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.a, 1.856160761) && EQ_DOUBLE(ret.b, 4.331041776) && EQ_DOUBLE(ret.c, 2.598625066))) {
		fprintf(stderr, "v3d projection\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}

	rej_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.a, 5.1 - 1.856160761) && EQ_DOUBLE(ret.b, 2.7 - 4.331041776) && EQ_DOUBLE(ret.c, 3 - 2.598625066))) {
		fprintf(stderr, "v3d projection\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.a, ret.b, ret.c);
		return 0;
	}


	return 1;
}
