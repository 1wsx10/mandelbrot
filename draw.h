#ifndef DRAW_H
#define DRAW_H

#include <math.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#include "player.h"
#include "constants.h"
#include "complex.h"
#include "mandle.h"

// draw everything
extern void draw();

extern void drawCircle(float x, float y, float rad, int num_verts);

#endif
