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

FBINFO *thread_fb;
struct tdraw_data {
	pthread_t tid;
	int id;
	int total;
	MANDLE_CONTROLS *cont;
};
void *tdraw(void *data) {
	/* unpack the data */
	struct tdraw_data *tdata = (struct tdraw_data*)data;

	int id = tdata->id;
	int total = tdata->total;
	MANDLE_CONTROLS *cont = tdata->cont;
	FBINFO *fb = thread_fb;


	com current_pos;

	int dimension = MIN(fb->vinfo.xres, fb->vinfo.yres);
	int x_long = fb->vinfo.xres > fb->vinfo.yres;
	int aspect_diff = ABS(MAX(fb->vinfo.xres, fb->vinfo.yres) - MIN(fb->vinfo.xres, fb->vinfo.yres));
	
	/* figure out xres start and limit */
	int start = id * (fb->vinfo.xres / total);
	int limit = start + fb->vinfo.xres / total;


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

	while(cont->is_running) {

		//for(int i = 0; i < fb->vinfo.xres; i++) {
		for(int i = start; i < limit; i++) {

			/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
			current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

			for(int j = 0; j < fb->vinfo.yres; j++) {
				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for y coordinate */
				current_pos.i = ((j + ppany + -1 * (x_long ? 0 : aspect_diff/2.0 )) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->I;

				int result = itterate(&current_pos, cont->depth);

				*pix.x = i;
				*pix.y = j;
				if(result >= 0) {
					/* outside the set, choose a colour */
					make_colour(result, cont->depth, &white);
				} else {
					/* inside the set, draw black */
					white.r = 0;
					white.g = 0;
					white.b = 0;
					white.t = 0;
				}
				draw(fb, &pix);
			}
		}

		//printf("zoom: %f\n\rdepth: %d\n\rR: %1.5Lf\n\rI: %1.5Lf\n\r", cont->zoom, cont->depth, cont->R, cont->I);
	}

	return NULL;
}

/* draws multiple threads */
void thread_display(MANDLE_CONTROLS *cont) {
	/* start the frame buffer */
	thread_fb = init();

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

	/* store the thread data */
	struct tdraw_data threads[numCPU];

	/* start the threads */
	for(int i = 0; i < numCPU; i++) {
		threads[i].id = i;
		threads[i].total = numCPU;
		threads[i].cont = cont;
		pthread_create(&threads[i].tid, NULL, tdraw, threads+i);
	}
	/* wait for the threads to end */
	for(int i = 0; i < numCPU; i++) {
		pthread_join(threads[i].tid, NULL);
	}

	/* free the frame buffer */
	end(thread_fb);
}

/* draws using a single thread */
void display(MANDLE_CONTROLS *cont) {
	FBINFO *fb = init();
	com current_pos;

	int dimension = MIN(fb->vinfo.xres, fb->vinfo.yres);
	int x_long = fb->vinfo.xres > fb->vinfo.yres;
	int aspect_diff = ABS(MAX(fb->vinfo.xres, fb->vinfo.yres) - MIN(fb->vinfo.xres, fb->vinfo.yres));

	//int depth = 1000;
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
	//double zoom = 1000;
	//long double cpanR = -0.761574;
	//long double cpanI = -0.0847596;
	


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

	while(cont->is_running) {

		for(int i = 0; i < fb->vinfo.xres; i++) {

			/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
			current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

			for(int j = 0; j < fb->vinfo.yres; j++) {
				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for y coordinate */
				current_pos.i = ((j + ppany + -1 * (x_long ? 0 : aspect_diff/2.0 )) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->I;

				int result = itterate(&current_pos, cont->depth);

				*pix.x = i;
				*pix.y = j;
				if(result >= 0) {
					/* outside the set, choose a colour */
					make_colour(result, cont->depth, &white);
				} else {
					/* inside the set, draw black */
					white.r = 0;
					white.g = 0;
					white.b = 0;
					white.t = 0;
				}
				draw(fb, &pix);
			}
		}

		//printf("zoom: %f\n\rdepth: %d\n\rR: %1.5Lf\n\rI: %1.5Lf\n\r", cont->zoom, cont->depth, cont->R, cont->I);
	}
	
	end(fb);
}

int main(int argc, char **argv) {
	MANDLE_CONTROLS *cont = mmap(NULL, sizeof(*cont), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	
// TODO: replace all controls ptrs for ones in the struct

	cont->is_running = 1;
	cont->depth = 50;
	cont->zoom = 1;
	cont->R = 0;
	cont->I = 0;


	int child_pid = fork();
	if(!child_pid) {
		// child
		thread_display(cont);
		//display(cont);
		return EXIT_SUCCESS;
	}
	// parent from here on out


	// init ncurses and choose settings
	initscr();
	timeout(-1);
	// stdscr: the default window
	// 0: block until input is available, rather than returning ERR on no input
	cbreak();
	// makes input return immediately on every char, rather than buffering a line
	noecho();
	// stops characters printing
	nodelay(stdscr, 0);
	// hide the cursor
	curs_set(0);

	while(cont->is_running) {

		int key = getch();
		//printf("KEY: %c\t #: %d\n\r", (char)key, key);


		switch(key) {
			case KEY_UP:
			case (int)'w':
				cont->I -= 1/ cont->zoom;
				break;
			case KEY_DOWN:
			case (int)'s':
				cont->I += 1/ cont->zoom;
				break;
			case KEY_RIGHT:
			case (int)'d':
				cont->R += 1/ cont->zoom;
				break;
			case KEY_LEFT:
			case (int)'a':
				cont->R -= 1/ cont->zoom;
				break;

			case (int)'p':
				cont->depth *= 1.1f;
				break;
			case (int)'o':
				cont->depth /= 1.1f;
				break;


			case (int)'+':
				cont->zoom *= 1.5f;
				break;
			case (int)'-':
				cont->zoom /= 1.5f;
				break;



			case KEY_EXIT:
				cont->is_running = 0;
				break;
		}
	}

	//ncurses
	endwin();

	// undo memory map
	munmap(cont, sizeof(*cont));
	return EXIT_SUCCESS;

}

