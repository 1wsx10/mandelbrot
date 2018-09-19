#include "vector.h"

void add_v3d(v3d a, v3d b, v3d *ret) {
	ret->x = a.x + b.x;
	ret->y = a.y + b.y;
	ret->z = a.z + b.z;
}

void sub_v3d(v3d a, v3d b, v3d *ret) {
	ret->x = a.x - b.x;
	ret->y = a.y - b.y;
	ret->z = a.z - b.z;
}

void mul_v3d(v3d a, double b, v3d *ret) {
	ret->x = a.x * b;
	ret->y = a.y * b;
	ret->z = a.z * b;
}

double dot_v3d(v3d a, v3d b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double det(double a, double b, double c, double d) {
	return a * d - b * c;
}

void cro_v3d(v3d a, v3d b, v3d *ret) {
	double i = det(a.y, a.z, b.y, b.z);
	double j = det(a.x, a.z, b.x, b.z) * -1;
	double k = det(a.x, a.y, b.x, b.y);

	ret->x = i;
	ret->y = j;
	ret->z = k;
}

double len_v3d(v3d a) {
	return sqrt(lsq_v3d(a));
}

double lsq_v3d(v3d a) {
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

void nrm_v3d(v3d a, v3d *ret) {
	double len = len_v3d(a);

	ret->x = a.x / len;
	ret->y = a.y / len;
	ret->z = a.z / len;
}

void nrm_c_v3d(v3d *a) {
	double len = len_v3d(*a);

	a->x = a->x / len;
	a->y = a->y / len;
	a->z = a->z / len;
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
	a.x = 5.1, a.y = 2.7, a.z = 3;
	b.x = 3, b.y = 7, b.z = 4.2;

	double scalar = 2.34;
	double answer;

	v3d ret;

	add_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.x, 8.1) && EQ_DOUBLE(ret.y, 9.7) && EQ_DOUBLE(ret.z, 7.2))) {
		fprintf(stderr, "v3d add\nFILE: %s\nLINE: %d\nFUNCTION: %s\n", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	sub_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.x, 2.1) && EQ_DOUBLE(ret.y, -4.3) && EQ_DOUBLE(ret.z, -1.2))) {
		fprintf(stderr, "v3d sub\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	mul_v3d(a, scalar, &ret);

	if(!(EQ_DOUBLE(ret.x, 11.934) && EQ_DOUBLE(ret.y, 6.318) && EQ_DOUBLE(ret.z, 7.02))) {
		fprintf(stderr, "v3d mul\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	answer = dot_v3d(a, b);

	if(!EQ_DOUBLE(answer, 46.8)) {
		fprintf(stderr, "v3d dot\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "ans: %f\n", answer);
		return 0;
	}

	cro_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.x, -9.66) && EQ_DOUBLE(ret.y, -12.42) && EQ_DOUBLE(ret.z, 27.6))) {
		fprintf(stderr, "v3d cro\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
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

	if(!(EQ_DOUBLE(ret.x, 0.784151526) && EQ_DOUBLE(ret.y, 0.415139043) && EQ_DOUBLE(ret.z, 0.461265604))) {
		fprintf(stderr, "v3d normalized\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	ret = a;
	nrm_c_v3d(&ret);

	if(!(EQ_DOUBLE(ret.x, 0.784151526) && EQ_DOUBLE(ret.y, 0.415139043) && EQ_DOUBLE(ret.z, 0.461265604))) {
		fprintf(stderr, "v3d normalized in place\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	prj_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.x, 1.856160761) && EQ_DOUBLE(ret.y, 4.331041776) && EQ_DOUBLE(ret.z, 2.598625066))) {
		fprintf(stderr, "v3d projection\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}

	rej_v3d(a, b, &ret);

	if(!(EQ_DOUBLE(ret.x, 5.1 - 1.856160761) && EQ_DOUBLE(ret.y, 2.7 - 4.331041776) && EQ_DOUBLE(ret.z, 3 - 2.598625066))) {
		fprintf(stderr, "v3d projection\nFILE: %s\nLINE: %d\nFUNCTION: %s", __FILE__, __LINE__, __FUNCTION__);
		fprintf(stderr, "a: %f\tb: %f\tc: %f\n", ret.x, ret.y, ret.z);
		return 0;
	}


	return 1;
}
