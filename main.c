#include "main.h"


#ifndef PI
#define PI 3.14159f
#endif
void make_colour(double val, RGBT *ret) {
#define RED_YELLOW_WHITE_YELLOW

#ifdef RED_YELLOW_WHITE_YELLOW
	double val_tx = val * 2 * PI * 0.02;
	ret->r = (int)255;
	ret->g = (int)255 * (sin(val_tx - (PI/2) + cos(val_tx - (PI/2))) + 1)/2;
	ret->b = (int)255 * (-1 * cos(val_tx) + 1)/2;
	ret->t = 0;
#endif
#ifdef RED_YELLOW_WHITE_PINK
	double val_tx = val * 2 * PI * 0.02;
	ret->r = (int)255;
	ret->g = (int)255 * (sin(val_tx - (PI/2) + cos(val_tx - (PI/2))) + 1)/2;
	ret->b = (int)255 * (-1 * sin(val_tx + cos(val_tx)) + 1)/2;
	ret->t = 0;
#endif
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




MANDLE_CONTROLS *cont;
FBINFO *thread_fb;

void log_mutex_info(int id, char *mutex_name, char is_getting_file) {
#ifdef LOG_MUTEXES

	assert(!pthread_mutex_lock(&logfile_mutex));

	if(is_getting_file) {
		// getting mutex
		// dprintf for file descriptor
		dprintf(logfile_fd, "%3d   locking %20s\n", id, mutex_name);
	} else {
		// releasing mutex
		dprintf(logfile_fd, "%3d unlocking %20s\n", id, mutex_name);
	}

	assert(!pthread_mutex_unlock(&logfile_mutex));
#endif
}

void *tdraw(void *data) {
	/* unpack the data */
	struct tdraw_data *tdata = (struct tdraw_data*)data;

	// array of all threads, we use this to find new jobs
	// only want 1 thread looking for a job at a time
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

	// repeatedly draw frames
	while(cont->is_running) {

		// Set yourself as idle and signal to the main thread, when all threads are idle main will start
		log_mutex_info(tdata->idx, "my_state", (char)1);
		assert(!pthread_mutex_lock(&tdata->state_mutex));
		tdata->state = T_IDLE;
		assert(!pthread_mutex_unlock(&tdata->state_mutex));
		log_mutex_info(tdata->idx, "my_state", (char)0);
		log_mutex_info(tdata->idx, "currently_idle", (char)1);
		assert(!pthread_mutex_lock(&currently_idle_mutex));
		currently_idle++;
		pthread_cond_signal(&currently_idle_cond);
		log_mutex_info(tdata->idx, "currently_idle", (char)0);
		assert(!pthread_mutex_unlock(&currently_idle_mutex));

		// wait for work from main
		log_mutex_info(tdata->idx, "stay_idle", (char)1);
		assert(!pthread_mutex_lock(&stay_idle_mutex));
		while(stay_idle) {
			pthread_cond_wait(&stay_idle_cond , &stay_idle_mutex);
		}
		assert(!pthread_mutex_unlock(&stay_idle_mutex));
		log_mutex_info(tdata->idx, "stay_idle", (char)1);

		// we have just been told to go, reset frame_update
		log_mutex_info(tdata->idx, "my_bounds", (char)1);
		assert(!pthread_mutex_lock(&tdata->bounds_mutex));
		tdata->frame_update = (char)0;
		assert(!pthread_mutex_unlock(&tdata->bounds_mutex));
		log_mutex_info(tdata->idx, "my_bounds", (char)0);

		char start = (char)1;

		// draw lots of parts of a single frame
		while(1) {

			if(tdata->idx == 0 && start) {
				// we got the first job, lets skip the job search
				start = (char)0;
			} else {
				// time to look for a job

				// look for a job
				//log locking
				log_mutex_info(tdata->idx, "my_state", (char)1);

				assert(!pthread_mutex_lock(&tdata->state_mutex));
				tdata->state = T_LOOKING;
				assert(!pthread_mutex_unlock(&tdata->state_mutex));
				//log unlocking
				log_mutex_info(tdata->idx, "my_state", (char)0);

				// setup data for checking jobs
				int greatest_idx = -1;
				int greatest_size = 0;

				// locking array mutex. no other threads can look for a job while i am
				//log locking
				log_mutex_info(tdata->idx, "thread_array", (char)1);

				assert(!pthread_mutex_lock(&thread_array_mutex));

				// locking my bounds, nothing should be using my bounds
				//log locking
				log_mutex_info(tdata->idx, "my_bounds", (char)1);

				assert(!pthread_mutex_lock(&tdata->bounds_mutex));
				for(int i = 0; i < tdata->num_threads; i++) {
					if(i == tdata->idx) continue;

					// check the state of this thread to find the greatest
					// no other thread is looking for a job, but the worker for this job can change its state
					
					//log locking
					char mutexname[20];
					snprintf(mutexname, 20, "thr_arr[%2d].state", i);
					log_mutex_info(tdata->idx, mutexname, (char)1);

					assert(!pthread_mutex_lock(&thread_array[i].state_mutex));
					char curr_thread_state = thread_array[i].state;
					assert(!pthread_mutex_unlock(&thread_array[i].state_mutex));

					//log unlocking
					log_mutex_info(tdata->idx, mutexname, (char)0);

					// don't want idle or looking
					if((curr_thread_state & (T_IDLE|T_LOOKING)) != (char)0) continue;

					if(curr_thread_state == T_WORKING) {
						// working, check if this is the greatest

						int size = 0;

						// log locking
						snprintf(mutexname, 20, "thr_arr[%2d].bounds", i);
						log_mutex_info(tdata->idx, mutexname, (char)1);

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

								// log unlocking
								snprintf(mutexname, 20, "thr_arr[%2d].bounds", greatest_idx);
								log_mutex_info(tdata->idx, mutexname, (char)0);
							}
							greatest_idx = i;
							greatest_size = size;
						} else {
							// not the greatest, we don't want to hold onto this mutex
							assert(!pthread_mutex_unlock(&thread_array[i].bounds_mutex));

							// log unlocking
							snprintf(mutexname, 20, "thr_arr[%2d].bounds", i);
							log_mutex_info(tdata->idx, mutexname, (char)0);
						}
					}
				}

				if(greatest_idx == -1) {
					// no jobs found, go idle waiting for next frame

					// we don't have lock from greatest, no need to unlock that
					//log unlocking
					log_mutex_info(tdata->idx, "thread_array", (char)0);

					assert(!pthread_mutex_unlock(&thread_array_mutex));
					//log unlocking
					log_mutex_info(tdata->idx, "my_bounds", (char)0);

					assert(!pthread_mutex_unlock(&tdata->bounds_mutex));
					// tell ourselves and others that we need to go idle
					//log locking
					log_mutex_info(tdata->idx, "stay_idle", (char)1);
					assert(!pthread_mutex_lock(&stay_idle_mutex));
					stay_idle = (char)1;
					assert(!pthread_mutex_unlock(&stay_idle_mutex));
					//log unlocking
					log_mutex_info(tdata->idx, "stay_idle", (char)0);
					break;
				}

				/* we are still holding onto the bounds mutex of the job we want.
				* we want because we don't want anyone looking at or modifying 
				* coorinates until we are done modifying it
				*	this also pauses the worker thread for this job
				*/

				// unlock array mutex, others can start looking for a job now
				//log unlocking
				log_mutex_info(tdata->idx, "thread_array", (char)0);

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
				//log unlocking
				log_mutex_info(tdata->idx, "my_bounds", (char)0);

				assert(!pthread_mutex_unlock(&tdata->bounds_mutex));

				// we are finished with splitting the work to take half
				// unlock bounds for greatest idx
				//	so that the worker for these bounds can continue its work and other threads
				//	can look for a job here
				assert(!pthread_mutex_unlock(&thread_array[greatest_idx].bounds_mutex));
				char mutexname[20];
				snprintf(mutexname, 20, "thr_arr[%2d].bounds", greatest_idx);
				log_mutex_info(tdata->idx, mutexname, (char)0);

			}

			// set yourself as working
			log_mutex_info(tdata->idx, "my_state", (char)1);
			assert(!pthread_mutex_lock(&tdata->state_mutex));
			tdata->state = T_WORKING;
			assert(!pthread_mutex_unlock(&tdata->state_mutex));
			log_mutex_info(tdata->idx, "my_state", (char)0);
			log_mutex_info(tdata->idx, "currently_working", (char)1);
			assert(!pthread_mutex_lock(&currently_working_mutex));
			currently_working++;
			assert(!pthread_mutex_unlock(&currently_working_mutex));
			log_mutex_info(tdata->idx, "currently_working", (char)0);


			log_mutex_info(tdata->idx, "my_bounds", (char)1);
			assert(!pthread_mutex_lock(&tdata->bounds_mutex));
			// Do the work
			for(int i = tdata->TLx; i < tdata->BRx; i++) {
				if(tdata->frame_update) {
					// reset, the frame has changed
					break;
				}
				tdata->TLx = i;
				int ystart = tdata->TLy;
				int yend = tdata->BRy;
				assert(!pthread_mutex_unlock(&tdata->bounds_mutex));
				log_mutex_info(tdata->idx, "my_bounds", (char)0);

				/* translate (0,0) (xres,yres) into (-2,2i) (2,-2i) for x coordinate */
				current_pos.r = ((i+ ppanx + -1 * (x_long ? aspect_diff/2.0 : 0)) * (4.0/ cont->zoom) / dimension) - (2.0/ cont->zoom) + cont->R;

				for(int j = ystart; j < yend; j++) {
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
				log_mutex_info(tdata->idx, "my_bounds", (char)1);
				assert(!pthread_mutex_lock(&tdata->bounds_mutex));
			}
			assert(!pthread_mutex_unlock(&tdata->bounds_mutex));
			log_mutex_info(tdata->idx, "my_bounds", (char)0);

			// mark yourself as finished working
			log_mutex_info(tdata->idx, "currently_working", (char)1);
			assert(!pthread_mutex_lock(&currently_working_mutex));
			currently_working--;
			pthread_cond_signal(&frame_update_cond);
			assert(!pthread_mutex_unlock(&currently_working_mutex));
			log_mutex_info(tdata->idx, "currently_working", (char)0);
		}


		//printf("zoom: %f\n\rdepth: %d\n\rR: %1.5Lf\n\rI: %1.5Lf\n\r", cont->zoom, cont->depth, cont->R, cont->I);
	}

	return NULL;
}

/* draws multiple threads */
void *thread_display(void *asdf) {
	/* start the frame buffer */
	thread_fb = init();

	// set the number of threads to the number of cores
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

	/* store the thread data */
	/* this never gets written to during operation, so we don't need to mutex it */
	struct tdraw_data threads[numCPU];

	stay_idle = (char)1;

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

		// frame update
		assert(!pthread_mutex_lock(&currently_idle_mutex));
		if(currently_idle > 0) {
			// notify all threads of frame update
			for(int i = 0; i < numCPU; i++) {
				assert(!pthread_mutex_lock(&threads[i].bounds_mutex));
				threads[i].frame_update = (char)1;
				assert(!pthread_mutex_unlock(&threads[i].bounds_mutex));
			}
		}

		// wait until all are idle
		while(currently_idle < numCPU) {
			pthread_cond_wait(&currently_idle_cond, &currently_idle_mutex);
		}
		assert(!pthread_mutex_unlock(&currently_idle_mutex));

		/* start the first thread with the whole FB, the others will take some work from them */
		assert(!pthread_mutex_lock(&threads[0].bounds_mutex));
		threads[0].TLx = 0;
		threads[0].TLy = 0;
		threads[0].BRx = thread_fb->vinfo.xres;
		threads[0].BRy = thread_fb->vinfo.yres;
		assert(!pthread_mutex_unlock(&threads[0].bounds_mutex));

		// set all threads working
		assert(!pthread_mutex_lock(&stay_idle_mutex));
		stay_idle = (char)0;
		pthread_cond_broadcast(&stay_idle_cond);
		assert(!pthread_mutex_unlock(&stay_idle_mutex));


		// wait for the desired image to change
		assert(!pthread_mutex_lock(&frame_update_mutex));
		while(!frame_update) {
			pthread_cond_wait(&frame_update_cond , &frame_update_mutex);
		}
		frame_update = (char)0;

		// reset stay_idle before we release the lock, so that threads can't interrupt before being told to idle
		assert(!pthread_mutex_lock(&stay_idle_mutex));
		stay_idle = (char)1;
		assert(!pthread_mutex_unlock(&stay_idle_mutex));

		assert(!pthread_mutex_unlock(&frame_update_mutex));

	}


	/* wait for the threads to end */
	for(int i = 0; i < numCPU; i++) {
		pthread_join(threads[i].tid, NULL);
	}

	/* free the frame buffer */
	end(thread_fb);
	return NULL;
}

int main(int argc, char **argv) {
	cont = mmap(NULL, sizeof(*cont), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);


#ifdef LOG_MUTEXES
	remove(MUTEX_FNAME);

	assert(!pthread_mutex_lock(&logfile_mutex));
	//                                       create a file with rw-r--r--
	logfile_fd = open(MUTEX_FNAME, O_CREAT|O_WRONLY|O_APPEND, 0b110100100);

	dprintf(logfile_fd, "starting logs\n");
	assert(!pthread_mutex_unlock(&logfile_mutex));
#endif

	
// TODO: replace all controls ptrs for ones in the struct

	cont->is_running = 1;
	cont->depth = 50;
	cont->zoom = 1;
	cont->R = 0;
	cont->I = 0;

	frame_update = (char)1;
	pthread_t display_id;
	pthread_create(&display_id, NULL, thread_display, cont);


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

		assert(!pthread_mutex_lock(&frame_update_mutex));
		frame_update = (char)1;
		pthread_cond_signal(&frame_update_cond);
		assert(!pthread_mutex_unlock(&frame_update_mutex));
	}

#ifdef LOG_MUTEXES
	assert(!pthread_mutex_lock(&logfile_mutex));
	close(logfile_fd);
	assert(!pthread_mutex_unlock(&logfile_mutex));
#endif

	//ncurses
	endwin();

	// undo memory map
	munmap(cont, sizeof(*cont));
	return EXIT_SUCCESS;

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

