#include "mandle.h"


void f_on_z(com *z, com *c, com *ret) {
	com zsqr;
	
	sqr_com(z, &zsqr);
	add_com(&zsqr, c, ret);
}

int itterate(com *n, int max) {

	// any n with magnitude >= 2 is disqualified automatically
	// we are not performing the sqrt, so instead we square both
	if(magnitude_squared_com(n) >= 4) return 0;

	com z;
	z.r = 0;
	z.i = 0;

	for(int i = 0; i < max; i++) {

		f_on_z(&z, n, &z);

		if(magnitude_squared_com(&z) >= 4) return i + 1;

	}

	return -1;
}
