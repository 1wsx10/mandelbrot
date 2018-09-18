#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GL/freeglut.h>
#include <GL/gl.h>

extern int tDown;
extern int gDown;
extern int fDown;
extern int hDown;

extern void keyDown(unsigned char key, int x, int y);
extern void keyUp(unsigned char key, int x, int y);


#endif
