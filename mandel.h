#ifndef MANDLE_H
#define MANDLE_H

#include "complex.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/** itterates f(z) = z^2 + c
 * 
 * @param number	the complex number inserted into c
 * @param max		the max itterations to do
 * @param z_out		the address of a value to output the final z value
 * @param return	the number of itterations before distance >= 2
 *						-1 signifies magnitude didn't become >= 2
 */
int itterate(com *number, int itter_count, com *z_out);






#endif
