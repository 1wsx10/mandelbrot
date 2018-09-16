#include <stdlib.h>
#include <stdio.h>
/*#include <curses.h>*/
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

int tDown = 0;
int gDown = 0;
int fDown = 0;
int hDown = 0;

//x, y, z coordinate of one of the triangle corners
float corner[] = {0.0f, -0.5f, 0.0f};

// from opengl hello world tutorial
void drawTriangle();
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void mainloop();

int main();
