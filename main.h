#ifndef MAIN_H
#define MAIN_H

#include "constants.h"
#include "vector.h"
#include "matrixd.h"
#include "complex.h"
#include "mandle.h"

#include "lib/write_screen/drawlib.h"

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include <curses.h>

typedef struct mandle_controls {
	long int current_frame;
	char is_running;
	int depth;
	long double zoom;
	long double R;
	long double I;
} MANDLE_CONTROLS;

struct tdraw_data {
	pthread_t tid;
	MANDLE_CONTROLS *cont;
};

//threads:
// 1: mark as idle
// 2: wait to be told to work
// (loop):
//	3: take a job slot
//		or break to go back to 1 if no jobs
//	4: set as working
//	== do the work ==
//	5: set as finished

/* signals to the controller that the thread is waiting */
pthread_mutex_t currently_idle_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  currently_idle_cond  = PTHREAD_COND_INITIALIZER;
int currently_idle;

/* signals to the thread they can take a job */
pthread_mutex_t work_ready_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t work_ready_cond = PTHREAD_COND_INITIALIZER;
char work_ready;

/* protects the work_done value */
pthread_mutex_t work_done_mutex = PTHREAD_MUTEX_INITIALIZER;
int work_done;
int work_length;
int work_count;

pthread_cond_t  currently_working_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t currently_working_mutex= PTHREAD_MUTEX_INITIALIZER;
int currently_working;

/**
 * makes a RGBT value out of a number (0-255)
 */
void make_colour(int val, int depth, RGBT *ret);

#ifndef PI
#define PI 3.14159f
#endif

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#endif
#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

//entrypoint, init and start opengl
extern int main();


#endif
