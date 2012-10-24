#ifndef _SIMULATION_H_
#define _SIMULATION_H_

/* Configuration */
#define NUM_DUCKS 9
#define TIME_PER_FRAME 10

/* Stuff */
enum { LEFT = 0 , LEFT_TURN , RIGHT , RIGHT_TURN };

/* Function Prototypes */
GLvoid simulationRender(GLvoid);
GLvoid simulationResize(int w, int h);
GLvoid simulationInit(GLvoid);
GLvoid simulationIdle(GLvoid);

GLvoid KeyboardHandler(unsigned char key, int x, int y);
GLvoid MouseHandler(int button, int state, int x, int y); 
GLvoid ContextMenuHandler(int choice);

GLvoid gfxTableLeg(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength);
GLvoid gfxTableLegYZ(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint nx);
GLvoid gfxTableLegXY(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint nz);
GLvoid gfxTableLegXZ(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint ny);

#endif
