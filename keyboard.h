#ifndef KEYBOARD
#define KEYBOARD

extern int tDown;
extern int gDown;
extern int fDown;
extern int hDown;

extern void keyDown(unsigned char key, int x, int y);
extern void keyUp(unsigned char key, int x, int y);


#endif
