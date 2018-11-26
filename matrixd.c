#include "matrixd.h"


md * create_md(int rows, int cols) {
	md *ret;
	ret = malloc(sizeof(*ret));
	ret->rows = rows;
	ret->cols = cols;

	ret->m = malloc(sizeof(**ret->m) * rows * cols);

	// check for failure
	if(ret->m == NULL) {
		free(ret);
		ret = NULL;
	}

	return ret;
}

md * resize_md(int rows, int cols, md *ret) {
	ret->rows = rows;
	ret->cols = cols;

	ret->m = realloc(ret->m, sizeof(*ret->m) * rows * cols);

	// check for failure
	if(ret->m == NULL) {
		free(ret);
		ret = NULL;
	}
	
	return ret;
}

int add_md(md *a, md *b, md *ret) {
	if(!MD_DIMS_SAME(a, b)) {
		return 0;
	}
	if(!MD_DIMS_SAME(a, ret)) {
		ret = resize_md(a->rows, a->cols, ret);
		if(ret == NULL) { return 0; }
	}

	for(int i = 0; i < a->rows; i++) {
		for(int j = 0; j < a->cols; j++) {
			ret->m[i][j] = a->m[i][j] + b->m[i][j];
		}
	}

	return 1;
}

int sub_md(md *a, md *b, md *ret) {
	if(!MD_DIMS_SAME(a, b)) {
		return 0;
	}
	if(!MD_DIMS_SAME(a, ret)) {
		ret = resize_md(a->rows, a->cols, ret);
		if(ret == NULL) { return 0; }
	}

	for(int i = 0; i < a->rows; i++) {
		for(int j = 0; j < a->cols; j++) {
			ret->m[i][j] = a->m[i][j] - b->m[i][j];
		}
	}

	return 1;
}

int mul_mdsc(md *a, double b, md *ret) {
	if(!MD_DIMS_SAME(a, ret)) {
		ret = resize_md(a->rows, a->cols, ret);
		if(ret == NULL) { return 0; }
	}

	for(int i = 0; i < a->rows; i++) {
		for(int j = 0; j < a->cols; j++) {
			ret->m[i][j] = b * a->m[i][j];
		}
	}

	return 1;
}

int mul_mdmd(md *a, md *b, md *ret) {
	// check if we can do this multiplication
	if(a->cols != b->rows) {
		return 0;
	}

	// make ret the required size
	if(!(ret->rows == a->rows && ret->cols == b->cols)) {
		ret = resize_md(a->rows, b->cols, ret);
		if(ret == NULL) { return 0; }
	}

	// do the multiplication
	for(int i = 0; i < ret->rows; i++) {
		for(int j = 0; j < ret->cols; j++) {
			ret->m[i][j] = 0;
			for(int k = 0; k < a->cols; a++) {

				// a has the same row as ret, b has the same col as ret
				ret->m[i][j] += a->m[i][k] * b->m[k][j];
			}
		}
	}

	return 1;
}

double highest_md(md *a) {
	double ret = 0;

	for(int i = 0; i < a->rows; i++) {
		for(int j = 0; j < a->cols; j++) {
			ret = MAX(ret, ABS(a->m[i][j]));
		}
	}
	return ret;
}

int print_md(md *a) {
	// create format for a regular string
	char str[100] = "";
	double highest = highest_md(a);
	sprintf(str, "%f", highest);
	int len = strlen(str);
	char fmt[100] = "";
	sprintf(fmt, " %%-%d.3", len);


	for(int i = 0; i < a->rows; i++) {
		printf("| ");
		for(int j = 0; j < a->cols; j++) {
			printf(fmt, a->m[i][j]);
		}
		printf(" |\n");
	}
	return 1;
}
