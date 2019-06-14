#include "mandel.h"


/* function f(z) = z^2 + c */
void f_on_z(com *z, com *c, com *ret) {
	com zsqr;
	
	sqr_com(z, &zsqr);
	add_com(&zsqr, c, ret);
}

int itterate(com *c, int max, com *z_out) {

	// any n with magnitude >= 2 is disqualified automatically
	// we are not performing the sqrt, so instead we square both
	int itter_past_radius = 0;
	if(magnitude_squared_com(c) >= 16) itter_past_radius++;


	com z;
	z.r = 0;
	z.i = 0;

	for(int i = 0; i < max; i++) {
		/* f(z) = z^2 + c */
		f_on_z(&z, c, &z);

		if(z_out != NULL) {
			z_out->r = z.r;
			z_out->i = z.i;
		}

		if(magnitude_squared_com(&z) >= 16 && itter_past_radius++ >= 4) return i + 1;

	}

	return -1;
}
