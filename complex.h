/*
 * any pointer called 'ret' will almost certainly have a side-effect
 * the other pointers will not be modified unless explicitly stated
 */
#ifndef COMPLEX_H
#define COMPLEX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct Complex {
	long double r;
	long double i;
} com;

#if 0
#define MD_ROWS_EQUAL(X, Y) ((X)->rows == (Y)->rows)
#define MD_COLS_EQUAL(X, Y) ((X)->cols == (Y)->cols)
#define MD_DIMS_SAME(X, Y) (MD_ROWS_EQUAL((X), (Y)) && MD_COLS_EQUAL((X), (Y)))

#define ABS(X) ((X) < 0 ? -1 * (X) : (X))
#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))
#endif

// allocates the complex number on the heap
com * create_com(long double real, long double imag);
//you can just use free();
//void free_com(com *ret);

// a +- b
void add_com(com *a, com *b, com *ret);
void sub_com(com *a, com *b, com *ret);


// multiply complex with complex
void mul_comcom(com *a, com *b, com *ret);
// multiply complex with scalar
void mul_comsc(com *a, long double b, com *ret);

// square the complex number
void sqr_com(com *a, com *ret);
long double magnitude_squared_com(com *a);

void test_com_heap();
void test_com_stack();
void print_com(com *a);
void println_com(com *a);


#endif
