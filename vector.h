#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdio.h>

typedef struct Vector3d {
	double x;
	double y;
	double z;
} v3d;

// add a and b
void add_v3d(v3d a, v3d b, v3d *ret);
// subtract b from a
void sub_v3d(v3d a, v3d b, v3d *ret);
// multiply a by b
void mul_v3d(v3d a, double b, v3d *ret);

// dot a and b together
double dot_v3d(v3d a, v3d b);
// cross product of a onto b
void cro_v3d(v3d a, v3d b, v3d *ret);

// length of a
double len_v3d(v3d a);
// length of a squared
double lsq_v3d(v3d a);
// normalize a
void nrm_v3d(v3d a, v3d *ret);

// project a onto b
void prj_v3d(v3d a, v3d b, v3d *ret);
// reject a onto b
void rej_v3d(v3d a, v3d b, v3d *ret);

// test all the functions
int test_v3d();


#endif
