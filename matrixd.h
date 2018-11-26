/*
 * any pointer called 'ret' will almost certainly have a side-effect
 * the other pointers will not be modified unless explicitly stated
 */
#ifndef MATRIXD_H
#define MATRIXD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct Matrixd {
	double **m;
	int rows;
	int cols;
} md;

#define MD_ROWS_EQUAL(X, Y) ((X)->rows == (Y)->rows)
#define MD_COLS_EQUAL(X, Y) ((X)->cols == (Y)->cols)
#define MD_DIMS_SAME(X, Y) (MD_ROWS_EQUAL((X), (Y)) && MD_COLS_EQUAL((X), (Y)))

#define ABS(X) ((X) < 0 ? -1 * (X) : (X))
#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))

// allocates the md on the stack
md * create_md(int rows, int cols);
md * resize_md(int rows, int cols, md *ret);
void free_md(md *ret);

// a +- b
// returns success
int add_md(md *a, md *b, md *ret);
int sub_md(md *a, md *b, md *ret);


// multiply matrixd by matrixd
int mul_mdmd(md *a, md *b, md *ret);
// multiply matrixd by scalar
int mul_mdsc(md *a, double b, md *ret);

int print_md(md *a);


#endif
