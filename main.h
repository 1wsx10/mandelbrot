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
#include <assert.h>

#include <curses.h>

typedef struct mandle_controls {
	long int current_frame;
	char is_running;
	int depth;
	long double zoom;
	long double R;
	long double I;
} MANDLE_CONTROLS;

#define MIN_JOB_SIZE 256
#define T_WORKING (char)0
#define T_LOOKING (char)1
#define T_IDLE (char)2
#define T_CHECKED (char)3
#define T_UNCHECKED (char)4
struct tdraw_data {
	pthread_t tid;
	int idx;
	int num_threads;
	MANDLE_CONTROLS *cont;
	pthread_mutex_t state_mutex;
	char state;
	pthread_mutex_t bounds_mutex;
	int TLx;
	int TLy;
	int BRx;
	int BRy;
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
