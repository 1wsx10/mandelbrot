#include "main.h"


#ifndef PI
#define PI 3.14159f
#endif
void make_colour(int val, int depth, RGBT *ret) {
	double val_tx = (int)(val * PI + PI/4);
	ret->r = (int)(255 * sin(val_tx));
	ret->g = (int)(255 * sin(val_tx * PI * 2.0/3));
	ret->b = (int)(255 * sin(val_tx * PI * 4.0/3));
}



int main(int argc, char **argv) {
	FBINFO *fb = init();
	com current_pos;

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#endif
#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
	int dimension = MIN(fb->vinfo.xres, fb->vinfo.yres);
	int x_long = fb->vinfo.xres > fb->vinfo.yres;
	int aspect_diff = ABS(MAX(fb->vinfo.xres, fb->vinfo.yres) - MIN(fb->vinfo.xres, fb->vinfo.yres));

	int depth = 1000;
	//int depth = 50;
	//long double cpanR = 0;
	//long double cpanI = 0;
	//double zoom = 1;
	// coordinate pan
	//long double cpanR = -1.74995768370609350360221450607069970727110579726252077930242837820286008082972804887218672784431700831100544507655659531379747541999999995;
	//long double cpanI = 0.00000000000000000278793706563379402178294753790944364927085054500163081379043930650189386849765202169477470552201325772332454726999999995;
	//long double cpanR = -0.75;
	//long double cpanI = 0.0;
	//long double cpanR = -7/4.0;
	//long double cpanI = 0.0;
	//double zoom = 25;
	//double zoom = 125;
	//double zoom = 625;
	//double zoom = 3125;
	//double zoom = 15625;
	//double zoom = 78125;
	//double zoom = 200000;
	//double zoom = 2000000;
	//long double cpanR = -0.170337;
	//long double cpanI = -1.06506;
	//double zoom = 10;
	//double zoom = 1000;
	//double zoom = 100000;
	//long double cpanR = 0.42884;
	//long double cpanI = -0.231345;
	//double zoom = 100000;
	//long double cpanR = -1.62917;
	//long double cpanI = -0.0203968;
	//double zoom = 100000;
	double zoom = 1000;
	long double cpanR = -0.761574;
	long double cpanI = -0.0847596;
	


	//double cpanR = 0;
	//double cpanI = 0;
	// pixel pan
	int ppanx = 0;
	int ppany = 0;


	RGBT white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	white.t = 255;
	PIXEL pix;
	int px = 0;
	int py = 0;
	pix.x = &px;
	pix.y = &py;
	pix.colour = &white;

	for(int i = 0; i < fb->vinfo.xres; i++) {
		current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2 : 0)) * (4/zoom) / dimension) - (2/zoom) + cpanR;

		for(int j = 0; j < fb->vinfo.yres; j++) {
			/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) */
			current_pos.i = ((j + ppany + -1 * (x_long ? 0 : aspect_diff/2 )) * (4/zoom) / dimension) - (2/zoom) + cpanI;

			int result = itterate(&current_pos, depth);
			if(result >= 0) {
				/* outside the set */
				*pix.x = i;
				*pix.y = j;
				make_colour(result, depth, &white);
				/*white.r = result;
				white.g = result;
				white.b = result;*/

				draw(fb, &pix);
			}
		}
	}
	
	end(fb);


#if 0
	md *test;
	test = create_md(3, 3);

	test->m[0][0] = 00;
	test->m[0][1] = 01;
	test->m[0][2] = 02;
	test->m[1][0] = 10;
	test->m[1][1] = 11;
	test->m[1][2] = 12;
	test->m[2][0] = 20;
	test->m[2][1] = 21;
	test->m[2][2] = 22;
	
	print_md(test);
#endif

	return EXIT_SUCCESS;

}

