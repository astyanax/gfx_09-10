/*
 * TODO:
 *  - Scoring system (-1 miss, +5 hit sth kontinoterh eu8eia, +15 hit sth makrynh eu8eia)
 *  - Hxoi?
 *  - Levels?
 *  - Main menu??
 * KNOWN BUGS:
 *  - Ta 2 prwta kai teleutaia papakia erxontai pio konta. The 2nd one is the culprit!!
 */

#include <cstdio>
#include <cstdlib>

#include "glut.h"
#include "simulation.h"

GLint windowWidth = 800, windowHeight = 600;

int main(int argc, char* argv[])
{ 
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(120,80);  

    glutCreateWindow("\"Graphics I\" 2009-2010 Project, by std06066 & std06101");

    simulationInit();
    glutMainLoop();

    return EXIT_SUCCESS;	
}
