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

void *tdraw(void *data) {
	/* unpack the data */
	struct tdraw_data *tdata = (struct tdraw_data*)data;

	// array of all threads, we use this to find new jobs
	static pthread_mutex_t thread_array_mutex = PTHREAD_MUTEX_INITIALIZER;
	struct tdraw_data *thread_array = tdata - tdata->idx;


	MANDLE_CONTROLS *cont = tdata->cont;
	FBINFO *fb = thread_fb;


	com current_pos;

	int dimension = MIN(fb->vinfo.xres, fb->vinfo.yres);
	int x_long = fb->vinfo.xres > fb->vinfo.yres;
	int aspect_diff = ABS(MAX(fb->vinfo.xres, fb->vinfo.yres) - MIN(fb->vinfo.xres, fb->vinfo.yres));
	
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

	// repeatedly draw frames
	while(cont->is_running) {

		// Set yourself as idle and signal to the main thread, when all threads are idle main will start
		assert(!pthread_mutex_lock(&tdata->state_mutex));
		tdata->state = T_IDLE;
		assert(!pthread_mutex_unlock(&tdata->state_mutex));
		assert(!pthread_mutex_lock(&currently_idle_mutex));
		currently_idle++;
		pthread_cond_signal(&currently_idle_cond);
		assert(!pthread_mutex_unlock(&currently_idle_mutex));

		// wait for work from main
		assert(!pthread_mutex_lock(&work_ready_mutex));
		while (!work_ready) {
			pthread_cond_wait(&work_ready_cond , &work_ready_mutex);
		}
		assert(!pthread_mutex_unlock(&work_ready_mutex));

		char start = (char)1;

		// draw lots of parts of a single frame
		while(1) {

			if(tdata->idx == 0 && start) {
				// we got the first job, lets skip the init
				start = (char)0;
			} else {
				// not the very first job, time to look for one

				// look for a job
				assert(!pthread_mutex_lock(&tdata->state_mutex));
				tdata->state = T_LOOKING;
				assert(!pthread_mutex_unlock(&tdata->state_mutex));

				// setup data for checking jobs
				int greatest_idx = -1;
				int greatest_size = 0;

				// locking array mutex. no other threads can look for a job while i am
				assert(!pthread_mutex_lock(&thread_array_mutex));
				// locking my bounds, nothing should be using my bounds
				assert(!pthread_mutex_lock(&tdata->bounds_mutex));
				for(int i = 0; i < tdata->num_threads; i++) {
					if(i == tdata->idx) continue;

					// check the state of this thread to find the greatest
					// no other thread is looking for a job, but the worker for this job can change its state
					assert(!pthread_mutex_lock(&thread_array[i].state_mutex));
					char curr_thread_state = thread_array[i].state;
					assert(!pthread_mutex_unlock(&thread_array[i].state_mutex));

					// don't want idle or looking
					if((curr_thread_state & (T_IDLE|T_LOOKING)) != (char)0) continue;

					if(curr_thread_state == T_WORKING) {
						// working, check if this is the greatest

						int size = 0;

						// lock and hold onto the mutex incase its the greatest
						assert(!pthread_mutex_lock(&thread_array[i].bounds_mutex));
						int TLx = thread_array[i].TLx;
						int TLy = thread_array[i].TLy;
						int BRx = thread_array[i].BRx;
						int BRy = thread_array[i].BRy;
						size = (BRx - TLx) * (BRy - TLy);

						// true if we haven't started yet or if this would be a better one to split
						if(size > greatest_size && size > MIN_JOB_SIZE) {
							if(greatest_idx != -1) {
								// unlock the previous greatest's bounds mutex
								assert(!pthread_mutex_unlock(&thread_array[greatest_idx].bounds_mutex));
							}
							greatest_idx = i;
							greatest_size = size;
						} else {
							// not the greatest, we don't want to hold onto this mutex
							assert(!pthread_mutex_unlock(&thread_array[i].bounds_mutex));
						}
					}
				}

				if(greatest_idx == -1) {
					// no jobs found, go idle waiting for next frame
					assert(!pthread_mutex_unlock(&thread_array_mutex));
					assert(!pthread_mutex_unlock(&tdata->bounds_mutex));
					break;
				}

				/* we are still holding onto the bounds mutex of the job we want.
				* we want because we don't want anyone looking at or modifying 
				* coorinates until we are done modifying it
				*	this also pauses the worker thread for this job
				*/

				// unlock array mutex, others can start looking for a job now
				assert(!pthread_mutex_unlock(&thread_array_mutex));

				// time to take the job we found
				int sizex = thread_array[greatest_idx].BRx - thread_array[greatest_idx].TLx;
				int sizey = thread_array[greatest_idx].BRy - thread_array[greatest_idx].TLy;

				if(sizex > sizey) {
					// vertical split - they get the left portion
					thread_array[greatest_idx].BRx = thread_array[greatest_idx].TLx + sizex / 2;

					tdata->TLx = thread_array[greatest_idx].TLx + sizex / 2;
					tdata->BRx = thread_array[greatest_idx].TLx + sizex;
					tdata->TLy = thread_array[greatest_idx].TLy;
					tdata->BRy = thread_array[greatest_idx].BRy;
				} else {
					// horizontal split - they get the top portion
					thread_array[greatest_idx].BRy = thread_array[greatest_idx].TLy + sizey / 2;

					tdata->TLx = thread_array[greatest_idx].TLx;
					tdata->BRx = thread_array[greatest_idx].BRx;
					tdata->TLy = thread_array[greatest_idx].TLy + sizey / 2;
					tdata->BRy = thread_array[greatest_idx].TLy + sizey;
				}

				// done modifying my bounds
				// unlocking my bounds
				assert(!pthread_mutex_unlock(&tdata->bounds_mutex));

				// we are finished with splitting the work to take half
				// unlock bounds for greatest idx
				//	so that the worker for these bounds can continue its work and other threads
				//	can look for a job here
				assert(!pthread_mutex_unlock(&thread_array[greatest_idx].bounds_mutex));

			}

			// set yourself as working
			assert(!pthread_mutex_lock(&tdata->state_mutex));
			tdata->state = T_WORKING;
			assert(!pthread_mutex_unlock(&tdata->state_mutex));
			assert(!pthread_mutex_lock(&currently_working_mutex));
			currently_working++;
			assert(!pthread_mutex_unlock(&currently_working_mutex));


			assert(!pthread_mutex_lock(&tdata->bounds_mutex));
			// Do the work
			for(int i = tdata->TLx; i < tdata->BRx; i++) {
				tdata->TLx = i;
				int ystart = tdata->TLy;
				int yend = tdata->BRy;
				assert(!pthread_mutex_unlock(&tdata->bounds_mutex));

				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
				current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

				for(int j = ystart; j < yend; j++) {
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
				assert(!pthread_mutex_lock(&tdata->bounds_mutex));
			}
			assert(!pthread_mutex_unlock(&tdata->bounds_mutex));

			// mark yourself as finished working
			assert(!pthread_mutex_lock(&currently_working_mutex));
			currently_working--;
			pthread_cond_signal(&currently_working_cond);
			assert(!pthread_mutex_unlock(&currently_working_mutex));
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

	// set the number of threads to the number of cores
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

	/* store the thread data */
	/* this never gets written to during operation, so we don't need to mutex it */
	struct tdraw_data threads[numCPU];

	/* start the threads */
	for(int i = 0; i < numCPU; i++) {
		threads[i].cont = cont;
		threads[i].idx = i;
		threads[i].num_threads = numCPU;
		//pthread_mutex_init(&threads[i].state_mutex, NULL);
		threads[i].state_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		threads[i].state = T_IDLE;
		threads[i].bounds_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		pthread_create(&threads[i].tid, NULL, tdraw, threads+i);
	}

	while (cont->is_running) {
		// wait until all are idle
		assert(!pthread_mutex_lock(&currently_idle_mutex));
		while(currently_idle < numCPU) {
			pthread_cond_wait(&currently_idle_cond, &currently_idle_mutex);
		}
		assert(!pthread_mutex_unlock(&currently_idle_mutex));

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
		assert(!pthread_mutex_lock(&work_ready_mutex));
		work_ready = 1;
		assert(!pthread_mutex_unlock(&work_ready_mutex));

		/* start the first thread with the whole FB, the others will take some work from them */
		assert(!pthread_mutex_lock(&threads[0].bounds_mutex));
		threads[0].TLx = 0;
		threads[0].TLy = 0;
		threads[0].BRx = thread_fb->vinfo.xres;
		threads[0].BRy = thread_fb->vinfo.yres;
		assert(!pthread_mutex_unlock(&threads[0].bounds_mutex));

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

