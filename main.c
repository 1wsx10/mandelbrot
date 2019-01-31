#include "main.h"


#ifndef PI
#define PI 3.14159f
#endif
void make_colour(double val, RGBT *ret) {
#define RAINBOW
#ifdef BRIGHT_ORANGE_DARK_PURPLE
	double val_tx = val * 2 * PI * 0.02;
	ret->r = (int)(127 * (cos(val_tx/2) + 1) / 2) + 128;
	ret->g = (int)(127 * (sin(val_tx / 2 + PI /3) + 1.5 - cos(val_tx + PI / 3)) / 3.5) + 64;
	ret->b = (int)(63 * (sin(val_tx/2) - cos(val_tx) + 1) / 3) + 64;
	ret->t = 0;
#endif
#ifdef FLORAL
	double val_tx = val * 2 * PI * 0.02;
	ret->r = (int)(127 * (sin(val_tx) + 1) / 2) + 128;
	ret->g = (int)(63 * (sin(val_tx * 2 + PI * 2.0/3) + 1) / 2) + 64;
	ret->b = (int)(63 * (sin(val_tx + PI * 2.0/3) + 2 + sin(val_tx + PI * 2.0/3)) / 2) + 64;
	//ret->b = (int)(0 * (sin(val_tx + PI * 4.0/3) + 1) / 2);
	ret->t = 0;
#endif
#ifdef RAINBOW
	double val_tx = val * 2 * PI * 0.01;
	ret->r = (int)(255 * (sin(val_tx) + 1) / 2);
	ret->g = (int)(255 * (sin(val_tx + PI * 2.0/3) + 1) / 2);
	ret->b = (int)(255 * (sin(val_tx + PI * 4.0/3) + 1) / 2);
	ret->t = 0;
#endif
}

void make_smooth_colour(int val, RGBT *ret, com *z) {
	double log_zn = log(z->r * z->r + z->i * z->i) / 2;
	//double log_zn = log(sqrt(z->r * z->r - z->i * z->i));
	double nu = log2(log_zn);
	double newval = val + 1 - nu;
	newval = val + 5 - log(log(sqrt(z->r * z->r + z->i * z->i))) / log(2);

	RGBT c1;
	RGBT c2;
	make_colour((int)newval, &c1);
	make_colour((int)newval + 1, &c2);


	
	c1.r = 255;
	c1.g = 0;
	c1.b = 0;
	c2.r = 0;
	c2.g = 0;
	c2.b = 0;
	

	ret->r = (int)(255 * cos((newval - (int)newval) * 2 * PI));
	ret->g = 0;
	ret->b = 0;
	ret->t = 0;
	//interp_RGBT(&c1, &c2, newval - (int)newval, ret);
	//make_colour((int)newval, ret);
	make_colour(newval, ret);
}




FBINFO *thread_fb;

void *tdraw(void *data) {
	/* unpack the data */
	struct tdraw_data *tdata = (struct tdraw_data*)data;

	MANDLE_CONTROLS *cont = tdata->cont;
	FBINFO *fb = thread_fb;


	com current_pos;

	int dimension = MIN(fb->vinfo.xres, fb->vinfo.yres);
	int x_long = fb->vinfo.xres > fb->vinfo.yres;
	int aspect_diff = ABS(MAX(fb->vinfo.xres, fb->vinfo.yres) - MIN(fb->vinfo.xres, fb->vinfo.yres));
	
	int ppanx = 0;
	int ppany = 0;

	RGBT colour;
	colour.r = 255;
	colour.g = 255;
	colour.b = 255;
	colour.t = 255;
	PIXEL pix;
	int px = 0;
	int py = 0;
	pix.x = &px;
	pix.y = &py;
	pix.colour = &colour;

	int start;
	int limit;

	// repeatedly draw frames
	while(cont->is_running) {

		// Set yourself as idle and signal to the main thread, when all threads are idle main will start
		pthread_mutex_lock(&currently_idle_mutex);
		currently_idle++;
		pthread_cond_signal(&currently_idle_cond);
		pthread_mutex_unlock(&currently_idle_mutex);

		// wait for work from main
		pthread_mutex_lock(&work_ready_mutex);
		while (!work_ready) {
			pthread_cond_wait(&work_ready_cond , &work_ready_mutex);
		}
		pthread_mutex_unlock(&work_ready_mutex);

		// draw lots of parts of a single frame
		while(1) {

			// take a job slot
			pthread_mutex_lock(&work_done_mutex);
			if(work_done < work_count) {
				start = work_done * work_length;
				limit = start + work_length;
				work_done++;
				pthread_mutex_unlock(&work_done_mutex);
			} else {
				// no work left, break out to go back to idle (waiting for next frame)
				pthread_mutex_unlock(&work_done_mutex);
				// set work to not ready
				pthread_mutex_lock(&work_ready_mutex);
				work_ready = 0;
				pthread_mutex_unlock(&work_ready_mutex);
				break;
			}

			// set yourself as working
			pthread_mutex_lock(&currently_working_mutex);
			currently_working++;
			pthread_mutex_unlock(&currently_working_mutex);


			// Do the work
			for(int i = start; i < limit; i++) {

				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
				current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

				for(int j = 0; j < fb->vinfo.yres; j++) {
					/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for y coordinate */
					current_pos.i = ((j + ppany + -1 * (x_long ? 0 : aspect_diff/2.0 )) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->I;

					com z_out;
					int result = itterate(&current_pos, cont->depth, &z_out);

					*pix.x = i;
					*pix.y = j;
					if(result >= 0) {
						/* outside the set, choose a colour */
						make_smooth_colour(result, &colour, &z_out);
						//make_colour(result, &colour);
					} else {
						/* inside the set, draw black */
						colour.r = 0;
						colour.g = 0;
						colour.b = 0;
						colour.t = 0;
					}
					draw(fb, &pix);
				}
			}

			// mark yourself as finished working
			pthread_mutex_lock(&currently_working_mutex);
			currently_working--;
			pthread_cond_signal(&currently_working_cond);
			pthread_mutex_unlock(&currently_working_mutex);
		}


		//printf("zoom: %f\n\rdepth: %d\n\rR: %1.5Lf\n\rI: %1.5Lf\n\r", cont->zoom, cont->depth, cont->R, cont->I);
	}

	return NULL;
}

/* draws multiple threads */
void thread_display(MANDLE_CONTROLS *cont) {
	/* start the frame buffer */
	thread_fb = init();
	long int current_frame = 0;

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

	/* store the thread data */
	struct tdraw_data threads[numCPU];

	int splits = 64;

	/* start the threads */
	for(int i = 0; i < numCPU; i++) {
		threads[i].cont = cont;
		pthread_create(&threads[i].tid, NULL, tdraw, threads+i);
	}

	while (cont->is_running) {
		// wait until all are idle
		pthread_mutex_lock(&currently_idle_mutex);
		while(currently_idle < numCPU) {
			pthread_cond_wait(&currently_idle_cond, &currently_idle_mutex);
		}
		pthread_mutex_unlock(&currently_idle_mutex);

		// wait for the desired image to change
		// TODO: remove this busy wait by using pthread cond signal (turning control process into a thread) or something
		while(1) {
			sleep(0.1);
			if(cont->current_frame > current_frame) {
				// invalid data, reset
				current_frame = cont->current_frame;
				break;
			}
		}

		// set all work ready
		// no threads are running currently, its safe to just modify this
		pthread_mutex_lock(&work_ready_mutex);
		work_ready = 1;
		pthread_mutex_unlock(&work_ready_mutex);

		pthread_mutex_lock(&work_done_mutex);
		work_done = 0;
		work_count = splits;
		work_length = thread_fb->vinfo.xres / splits;
		pthread_mutex_unlock(&work_done_mutex);

		// set all threads working
		pthread_cond_broadcast(&work_ready_cond);
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


	RGBT colour;
	colour.r = 255;
	colour.g = 255;
	colour.b = 255;
	colour.t = 255;
	PIXEL pix;
	int px = 0;
	int py = 0;
	pix.x = &px;
	pix.y = &py;
	pix.colour = &colour;

	while(cont->is_running) {

		for(int i = 0; i < fb->vinfo.xres; i++) {

			/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
			current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

			for(int j = 0; j < fb->vinfo.yres; j++) {
				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for y coordinate */
				current_pos.i = ((j + ppany + -1 * (x_long ? 0 : aspect_diff/2.0 )) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->I;

				com z_out;
				int result = itterate(&current_pos, cont->depth, &z_out);

				*pix.x = i;
				*pix.y = j;
				if(result >= 0) {
					/* outside the set, choose a colour */
					make_smooth_colour(result, &colour, &z_out);
					//make_colour(result, &colour);
				} else {
					/* inside the set, draw black */
					colour.r = 0;
					colour.g = 0;
					colour.b = 0;
					colour.t = 0;
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

	cont->current_frame = 0;
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
				cont->current_frame++;
				break;
			case KEY_DOWN:
			case (int)'s':
				cont->I += 1/ cont->zoom;
				cont->current_frame++;
				break;
			case KEY_RIGHT:
			case (int)'d':
				cont->R += 1/ cont->zoom;
				cont->current_frame++;
				break;
			case KEY_LEFT:
			case (int)'a':
				cont->R -= 1/ cont->zoom;
				cont->current_frame++;
				break;

			case (int)'p':
				cont->depth *= 1.1f;
				cont->current_frame++;
				break;
			case (int)'o':
				cont->depth /= 1.1f;
				cont->current_frame++;
				break;


			case (int)'+':
				cont->zoom *= 1.5f;
				cont->current_frame++;
				break;
			case (int)'-':
				cont->zoom /= 1.5f;
				cont->current_frame++;
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

