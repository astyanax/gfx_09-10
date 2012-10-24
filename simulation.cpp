#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <windows.h>

#include "gl/glut.h"
#include "gfxFuncs.h"
#include "simulation.h"

/*
 *   To antikeimeno gfxSim tha periexei plhrofories sxetikes me to olo simulation,
 *   p.x. tis sta8eres L,R,D, ean trexei to simulation, th 8esh ths kameras k.o.k.
 */
struct gfx_sSim
{
    GLfloat L;              // Sta8era ekfwnhshs
    GLfloat R;              // Sta8era ekfwnhshs
    GLfloat D;              // Sta8era ekfwnhshs
    GLfloat H;              // Sta8era ekfwnhshs
    GLfloat v;              // Sta8era ekfwnhshs
    GLfloat g;              // Sta8era ekfwnhshs
    GLfloat dtDuck;         // Xronikh akribeia sth kinhsh twn papiwn
    GLfloat dtBall;         // Xronikh akribeia sth kinhsh ths mpalas
    GLfloat duckScale;      // Syntelesths mege8ous papiwn
    GLfloat rotationAngle;  // Gwnia peristrofhs oloklhrhs ths skhnhs
    GLint enable;           // Dhlwnei ean trexei to simulation
    GLfloat ColPos[3];      // H 8esh ths teleutaias sugkroushs
    GLfloat ColSize;        // Scaling factor gia to efe ths sygkroushs
    GLint drawCollision;    // Flag gia to an prepei na zwgrafistei collision
    GLint hRot, vRot;       // Peristrofh ths kameras orizontia kai katakoryfa
    GLint sceneRotation;    // Flag gia to an tha peristrefetai h skhnh
} gfxSim;

/*
 *  To antikeimeno gfxBall perigrafei th mpala-blhma.
 */
struct gfx_sSphere
{
    GLfloat pos[3];         // H twrinh 8esh ths mpalas
    GLfloat startPos[3];    // H arxikh 8esh ths mpalas (prin thn ektokseush)
    GLfloat speed;          // H taxuthta ektokseushs ths mpalas
    GLfloat R;              // H aktina ths mpalas
    GLfloat angle;          // H gwnia ektokseushs ths mpalas
    GLfloat time;           // H twrinh xronikh stigmh ths parabolikhs kinhshs
    GLint enable;           // Flag gia to an exei ektokseu8ei h mpala
} gfxBall;


/*
 *  To antikeimeno gfxDuckState krata orismenes plhrofories gia ena papi.
 *  8ewrhtika tha kratage kai orismenes plhrofories pou briskontai twra sto
 *  sto struct gfxModel, alla gia logous RAD protimh8hke auth h antimetwpish.
 */
typedef struct gfx_sDuckState
{
    GLint state;            // H katastash kinhshs tou papiou
    GLfloat time;           // H twrinh xronikh stigmh kommatiou ths kinhshs tou papiou
    GLfloat turnPos[3];     // H 8esh tou papiou kata tis strofes tou
} gfxDuckState;

/*
 *  Exoume loipon enan pinaka me ta papia,
 *  ena pinaka me tis katastaseis twn papiwn
 *  kai gia eukolia enan counter tou plh8ous twn papiwn. (To game teleiwnei otan duckCounter == 0)
 *
 */
gfxModel * duck[NUM_DUCKS];
gfxDuckState duckState[NUM_DUCKS];
int duckCounter = NUM_DUCKS;

/*
 *  Auth h sunarthsh arxikopoiei thn OpenGL, ka8ws kai to simulation to idio.
 */
GLvoid simulationInit(GLvoid)
{
    /*
    **  Init OpenGL
    */
    glutDisplayFunc(simulationRender);
    glutReshapeFunc(simulationResize);
    glutIdleFunc(simulationIdle);
    glutKeyboardFunc(KeyboardHandler);
    glutMouseFunc(MouseHandler);

    glutCreateMenu(ContextMenuHandler);
    glutAddMenuEntry("Οδηγίες Εκτέλεσης", 0);
    glutAddMenuEntry("-------------------", 0);
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("m - Start Game", 0);
    glutAddMenuEntry("h - Throw Ball", 0);
    glutAddMenuEntry("g - Strafe Left", 0);
    glutAddMenuEntry("j - Strafe Right", 0);
    glutAddMenuEntry("y - Increase Throw Velocity", 0);
    glutAddMenuEntry("v - Decrease Throw Velocity", 0);
    glutAddMenuEntry("u - Increase Throw Angle", 0);
    glutAddMenuEntry("b - Decrease Throw Angle", 0);
    glutAddMenuEntry("q - Exit Game", 0);
    glutAddMenuEntry("w/a/s/d - Rotate View", 0);
    glutAddMenuEntry("r - Reset View", 0);
    glutAddMenuEntry("t - Rotate scene", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    fprintf(stderr, "[Status] Simulation initiating...\n");
    //glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    /* Orismos genikoterou ambient fwtos */
    GLfloat globalAmbient[]  = { 0.05f, 0.05f, 0.05f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

    /* Orizoume enan "hlio" pisw-aristera kai panw ap'ta kefalia mas */
    GLfloat light0Position[] = { -20.0f, 50.0f, 100.0f, 1.0f };
    GLfloat light0Ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat light0Diffuse[]  = { 0.6f, 0.6f, 0.4f, 1.0f };
    GLfloat light0Specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv( GL_LIGHT0, GL_POSITION, light0Position );
    glLightfv( GL_LIGHT0, GL_AMBIENT, light0Ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light0Diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light0Specular );

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //glEnable(GL_CULL_FACE);

    /* Blue sky */
    glClearColor(0.2f,0.7f,0.85f,1.0f);
    glFlush();

    /*
    **  Init simulation data
    */
    gfxSim.L = 5;
    gfxSim.R = 2;
    gfxSim.D = 6;
    gfxSim.H = 5;
    gfxSim.v = 5;
    gfxSim.g = 10;
    gfxSim.dtDuck = 0.01;
    gfxSim.dtBall = 0.015;
    gfxSim.duckScale = 0.4;
    gfxSim.rotationAngle = 0;
    gfxSim.enable = gfxBall.enable = GL_FALSE;
    gfxSim.drawCollision = GL_FALSE;
    gfxSim.ColPos[X] = gfxSim.ColPos[Y] = gfxSim.ColPos[Z] = gfxSim.ColSize = 0;
    gfxSim.vRot = 20;
    gfxSim.hRot = 0;
    gfxSim.sceneRotation = GL_FALSE;

    gfxBall.startPos[X] = gfxBall.pos[X] = 0;
    gfxBall.startPos[Y] = gfxBall.pos[Y] = gfxSim.H + 1.5;
    gfxBall.startPos[Z] = gfxBall.pos[Z] = gfxSim.D;
    gfxBall.speed = 5;
    gfxBall.angle = 20;
    gfxBall.R = 0.25;
    gfxBall.time = 0;
    gfxBall.enable = GL_FALSE;

    for (GLint i=0 ; i < NUM_DUCKS ; i++) {
        duckState[i].state = RIGHT;
        duckState[i].time = 0;
        duckState[i].turnPos[X] = duckState[i].turnPos[Y] = duckState[i].turnPos[Z] = 0;
    }

    /*
    **  Load and init ducks
    */
    fprintf(stderr, "[Status] Loading ducks!\n");
    glPushMatrix();
    for (GLint i=0 ; i < NUM_DUCKS ; i++) { 
        if ( (duck[i] = gfxLoadObj("duck.obj")) == NULL ) {
            fprintf(stderr, "[Error] Could not load resource \"duck.obj\"!\n");
            exit(EXIT_FAILURE);
        }

        if (gfxNormalizeToUnit(duck[i], gfxSim.duckScale) != GFX_OK) {
            fprintf(stderr, "[Warning] Model normalization failed!\n");
        }

        // H arxikh 8esh einai proka8orismenh, sta retries omws einai tyxaia
        duck[i]->CoM[X] = gfxSim.L - 3* i * duck[i]->Rmax;
        duck[i]->CoM[Y] = gfxSim.H + duck[i]->Rmax/2;
        duck[i]->CoM[Z] = gfxSim.R;
        duck[i]->dirAngle = 180.0f;
    }
    glPopMatrix();
    fprintf(stderr, "[Status] Ducks loaded, initiation completed...\n");
}

GLvoid simulationRender(GLvoid)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

    /*
    **  Topo8ethsh kameras
    */
    //gluLookAt(6.0f, 12.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glTranslated(0, -gfxSim.H, -15);
    glRotatef(-gfxSim.rotationAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(gfxSim.vRot,1,0,0);
    glRotatef(gfxSim.hRot,0,1,0);

    /*
    **  Topo8ethsh mple ducktrack :o
    */
    glColor3f(0.1f, 0.1f, 0.9f);

    // Deksia strofh
    glPushMatrix();
    glTranslatef(gfxSim.L, gfxSim.H, 0);
    for(GLfloat i = -90; i < 90 ; i+=0.1) {
        glBegin(GL_LINES);
        glNormal3i(0, 0, -1);
        glVertex3f( (gfxSim.R-(0.5*gfxSim.duckScale))*cos(ANGLE2RAD(i)), 0 , (gfxSim.R-(0.5*gfxSim.duckScale))*sin(ANGLE2RAD(i)) );
        glNormal3i(0, 0, -1);
        glVertex3f( (gfxSim.R+(0.5*gfxSim.duckScale))*cos(ANGLE2RAD(i)), 0 , (gfxSim.R+(0.5*gfxSim.duckScale))*sin(ANGLE2RAD(i)) );
        glEnd();
    }
    glPopMatrix();

    // Aristerh strofh
    glPushMatrix();
    glTranslatef(-gfxSim.L, gfxSim.H, 0);
    for(GLfloat i = 90; i < 270 ; i+=0.1) {
        glBegin(GL_LINES);
        glNormal3i(0, 0, -1);
        glVertex3f( (gfxSim.R-(0.5*gfxSim.duckScale))*cos(ANGLE2RAD(i)), 0 , (gfxSim.R-(0.5*gfxSim.duckScale))*sin(ANGLE2RAD(i)) );
        glNormal3i(0, 0, -1);
        glVertex3f( (gfxSim.R+(0.5*gfxSim.duckScale))*cos(ANGLE2RAD(i)), 0 , (gfxSim.R+(0.5*gfxSim.duckScale))*sin(ANGLE2RAD(i)) );
        glEnd();
    }
    glPopMatrix();

    // Boh8htikh metablhth gia na glytwsoume ypologismous
    GLfloat halfWidth = gfxSim.duckScale/2;

    // Katw eu8eia
    glPushMatrix();
    glTranslatef(0, gfxSim.H, gfxSim.R);
    glBegin(GL_QUAD_STRIP);
    glNormal3i(0, 0, -1);
    glVertex3f(-gfxSim.L,0,-halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(-gfxSim.L,0,halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(gfxSim.L,0,-halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(gfxSim.L,0,halfWidth);
    glEnd();
    glPopMatrix();

    // Panw eu8eia
    glPushMatrix();
    glTranslatef(0, gfxSim.H, -gfxSim.R);
    glBegin(GL_QUAD_STRIP);
    glNormal3i(0, 0, -1);
    glVertex3f(-gfxSim.L,0,-halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(-gfxSim.L,0,halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(gfxSim.L,0,-halfWidth);
    glNormal3i(0, 0, -1);
    glVertex3f(gfxSim.L,0,halfWidth);
    glEnd();
    glPopMatrix();

    // Topo8ethsh podiwn trapeziou
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();   
    glTranslatef(0, gfxSim.H, 0);

    gfxTableLeg( -gfxSim.L+gfxSim.duckScale, -gfxSim.R, halfWidth, gfxSim.H );
    gfxTableLeg( -gfxSim.L+gfxSim.duckScale, gfxSim.R, halfWidth, gfxSim.H );
    gfxTableLeg( gfxSim.L-gfxSim.duckScale, -gfxSim.R, halfWidth, gfxSim.H );
    gfxTableLeg( gfxSim.L-gfxSim.duckScale, gfxSim.R, halfWidth, gfxSim.H );
    glPopMatrix();

    /*
    ** Topo8ethsh papakIwn! :O!
    */
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow ducky is yellow :v

    for (GLint i=0 ; i < NUM_DUCKS ; i++) {
        if ( duck[i] != NULL )
        {
            glPushMatrix();

            // Topo8etoume to papi sth twrinh tou 8esh
            glTranslatef(duck[i]->CoM[X], duck[i]->CoM[Y], duck[i]->CoM[Z]);

            // to peristrefoume oso prepei
            glRotatef(duck[i]->dirAngle, 0, 1.0f, 0);

            // kai telos to apeikonizoume/"zwgrafizoume"
            gfxRenderObj(duck[i]);

            glPopMatrix();
        }
    }

    // An exei ginei collision, prepei na zwgrafistei kai auto
    if ( gfxSim.drawCollision == GL_TRUE ) {
        glPushMatrix();
        glTranslatef(gfxSim.ColPos[X], gfxSim.ColPos[Y], gfxSim.ColPos[Z]);
        gfxDrawCollision(gfxSim.ColSize);
        glPopMatrix();
        glFlush();
    }

    /* Topo8ethsh mpalas */
    glPushMatrix();
    glColor3f(0.6f, 0.0f, 0.0f);

    glFrontFace(GL_CW);
    glTranslatef(gfxBall.pos[X], gfxBall.pos[Y], gfxBall.pos[Z]);
    glutSolidSphere(gfxBall.R, 32, 24);
    glFrontFace(GL_CCW);
    
    glPopMatrix();

    /* Zwgrafizoume kai mia epifaneia edafous */
    glPushMatrix();
    glColor3f(0.375f, 0.1f, 0.007f);
    glBegin(GL_POLYGON);
    glVertex3f(-2000, 0, -2000);
    glVertex3f(+2000, 0, -2000);
    glVertex3f(+2000, 0, +2000);
    glVertex3f(-2000, 0, +2000);
    glEnd();
    glPopMatrix();

    glFlush();
    glutSwapBuffers();

    // Ean egine collision, perimenoume gia na apolausoume to efe ;)
    if ( gfxSim.drawCollision == GL_TRUE ) {
        Sleep(1000);
        gfxSim.drawCollision = GL_FALSE;
    }
}

GLvoid simulationIdle(GLvoid)
{
    Sleep(TIME_PER_FRAME);

    // An fagame ola ta papakia, telos tou paixnidiou :O
    if (duckCounter == 0) {
        MessageBox(NULL, (const CHAR*)"The evil duckies are dead!\nGame over! :O\n\nAnother cheesy production\nby Kavaax studios!",
                   (const CHAR*)"At last, success!", MB_OK);
        exit(EXIT_SUCCESS);
    }

    // Ypologismos gwnias peristrofhs skhnhs
    if ( gfxSim.sceneRotation == GL_TRUE ) {
        if (gfxSim.rotationAngle > 360)
            gfxSim.rotationAngle = 0;
        gfxSim.rotationAngle += 1.0;
    }

    // Efoson trexei to simulation, tha ginetai animated olh h skhnh
    if (gfxSim.enable == GL_TRUE)
    {
        for (GLint i=0 ; i < NUM_DUCKS ; i++)
        {
            /* Efoson exei "katastrafei" to papi, proxwrame */
            if ( duck[i] == NULL )
                continue;

            // Duck movement
            GLfloat newPos[3];      // H nea 8esh tou papiou
            GLfloat angVel, theta;  // Gwniakh taxuththa kai gwnia peristrofhs
            GLfloat p1[3], p2[3];   // Boh8htikes metablhtes pou symbolizoun shmeia

            angVel = theta = 0;

            /*
            **  Check for collision
            */
            // p1 => Kentro sfairas
            p1[X] = gfxBall.pos[X]; p1[Y] = gfxBall.pos[Y]; p1[Z] = gfxBall.pos[Z];
            // p2 => Kentro papiou
            p2[X] = duck[i]->CoM[X]; p2[Y] = duck[i]->CoM[Y]; p2[Z] = duck[i]->CoM[Z];

            // Efoson egine collision, diagrafoume to papi kai enhmerwnoume tis plhrofories sygkroushs
            if (gfxCheckCollision(p1, p2, gfxBall.R, duck[i]->Rmax) == GL_TRUE) {
                gfxSim.ColPos[X] = p2[X];
                gfxSim.ColPos[Y] = p2[Y];
                gfxSim.ColPos[Z] = p2[Z] - duck[i]->Rmax;
                gfxSim.ColSize   = 2*duck[i]->Rmax;

                gfxUnloadObj(duck[i]);
                duck[i] = NULL;
                duckCounter--;
                gfxSim.drawCollision = GL_TRUE;
                continue;
            }

            /*
            **  Duck movement
            **
            **  Analoga me to state ths kinhshs tou papiou, elegxoume pou tha phgaine.
            **  An tha ebgaine "ektos oriwn", tote allazoume thn katastash tou accordingly
            */

            newPos[X] = duck[i]->CoM[X];
            newPos[Y] = duck[i]->CoM[Y];
            newPos[Z] = duck[i]->CoM[Z];

            if(duckState[i].state == RIGHT) {
                newPos[X] += gfxSim.v * gfxSim.dtDuck;
            }
            else if(duckState[i].state == RIGHT_TURN) {
                duckState[i].time += gfxSim.dtDuck;
                angVel = gfxSim.v / gfxSim.R;
                theta = 100 * angVel * duckState[i].time;

                duck[i]->dirAngle = 180 + theta;
                newPos[X] = duckState[i].turnPos[X] + gfxSim.R * sin(ANGLE2RAD(theta));
                newPos[Z] = duckState[i].turnPos[Z] + gfxSim.R * cos(ANGLE2RAD(theta)) - gfxSim.R;
            }
            else if(duckState[i].state == LEFT) {
                newPos[X] -= gfxSim.v * gfxSim.dtDuck;
            }
            else if(duckState[i].state == LEFT_TURN) {
                duckState[i].time += gfxSim.dtDuck;
                angVel = gfxSim.v / gfxSim.R;
                theta = 100 * angVel * duckState[i].time;

                duck[i]->dirAngle = theta;
                newPos[X] = duckState[i].turnPos[X] - gfxSim.R * sin(ANGLE2RAD(theta));
                newPos[Z] = duckState[i].turnPos[Z] - gfxSim.R * cos(ANGLE2RAD(theta)) + gfxSim.R;
            }

            /*
            **  Elegxoume an to papi tha synexisei ws exei h an prepei na allaksei katastash!
            */
            if(duckState[i].state == RIGHT && newPos[X] >= gfxSim.L) {
                duckState[i].state = RIGHT_TURN;
                duckState[i].time = 0;
                duckState[i].turnPos[X] = newPos[X];
                duckState[i].turnPos[Y] = newPos[Y];
                duckState[i].turnPos[Z] = newPos[Z];
            }
            else if(duckState[i].state == RIGHT_TURN && newPos[X] <= gfxSim.L ) {
                duckState[i].state = LEFT;
                duckState[i].time = 0;
            }
            else if(duckState[i].state == LEFT && newPos[X] <= -gfxSim.L) {
                duckState[i].state = LEFT_TURN;
                duckState[i].time = 0;
                duckState[i].turnPos[X] = newPos[X];
                duckState[i].turnPos[Y] = newPos[Y];
                duckState[i].turnPos[Z] = newPos[Z];
            }
            else if(duckState[i].state == LEFT_TURN && newPos[X] >= -gfxSim.L) {
                duckState[i].state = RIGHT;
                duckState[i].time = 0;
            }

            // Enhmerwnoume th nea 8esh tou papiou
            duck[i]->CoM[X] = newPos[X];
            duck[i]->CoM[Y] = newPos[Y];
            duck[i]->CoM[Z] = newPos[Z];
        }

        /*
        **  Ball movement
        **
        **  H mpala kounietai mono efoson leitourgei to simulation.
        **  Ektelei parabolikh kinhsh me bash tis sxetikes parametrous.
        **  Efoson pesei poly xamhla (h poly mprosta) tote epanatopo8eteitai se tyxaia 8esh.
        */
        if (gfxBall.enable == GL_TRUE)
        {
            #define gfxalpha ANGLE2RAD(gfxBall.angle)
            GLfloat newPos[3];

            // Time flows...
            gfxBall.time += gfxSim.dtBall;

            // Ypologizoume th twrinh 8esh
            newPos[Z] = gfxBall.startPos[Z] - gfxBall.speed * cos(gfxalpha) * gfxBall.time;
            newPos[Y] = gfxBall.startPos[Y] + gfxBall.speed * sin(gfxalpha) * gfxBall.time - 0.5 * gfxSim.g * gfxBall.time * gfxBall.time;

            gfxBall.pos[Y] = newPos[Y];
            gfxBall.pos[Z] = newPos[Z];

            // An h mpala epese poly xamhla h poly mprosta, reset it
            if ( ( gfxBall.pos[Y] < 0 ) || (gfxBall.pos[Z] < -gfxSim.R * 1.2) ) {
                gfxBall.startPos[X] = gfxBall.pos[X] = ((rand()/(float)RAND_MAX)*2*gfxSim.L)-gfxSim.L;
                gfxBall.startPos[Y] = gfxBall.pos[Y] = gfxSim.H + 1.2 + ((rand()/(float)RAND_MAX)*0.6);
                gfxBall.startPos[Z] = gfxBall.pos[Z] = gfxSim.D + ((rand()/(float)RAND_MAX)*2 - 1);
                gfxBall.time = 0;
                gfxBall.enable = GL_FALSE;
            }

            #undef gfxalpha
        }
    }

    glutPostRedisplay();
}

GLvoid simulationResize(int w, int h)
{ 
    if (h==0) h=1;
    glViewport(0,0,w,h); 

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();

    gluPerspective(60.0, (float)w/(float)h, 0.01, 1000.0);
    glutPostRedisplay();
}

GLvoid KeyboardHandler(unsigned char key, int x, int y)
{
    x=y=0;
    switch(key)
    {
        /* Quit */
        case 27:
        case 'Q':
        case 'q':
            for (GLint i = 0 ; i < NUM_DUCKS ; i++) {
                if ( gfxUnloadObj(duck[i]) != GFX_OK ) {
                    fprintf(stderr, "[Warning] Could not unload duck #%d!\n", i);
                }
            }
            exit(EXIT_SUCCESS);
            break;

        /* Toggle sim */
        case 'M':
        case 'm':
            gfxBoolInvert(&gfxSim.enable);
            break;

        /* Toggle ball */
        case 'H':
        case 'h':
            gfxBoolSetTrue(&gfxBall.enable);
            break;

        /* Strafe */
        case 'g':
        case 'G':
            if (gfxBall.enable == GL_FALSE && gfxBall.pos[X] >= -gfxSim.L) {
                gfxBall.pos[X]-=0.15;
            }
            break;
		case 'j':
        case 'J':
            if (gfxBall.enable == GL_FALSE && gfxBall.pos[X] <= gfxSim.L) {
                gfxBall.pos[X]+=0.15;
            }
            break;

        /* Velocity */
        case 'y':
        case 'Y':
            if (gfxBall.speed < 20)
                gfxBall.speed++;
            break;
        case 'v':
        case 'V':
            if (gfxBall.speed > 0)
                gfxBall.speed--;
            break;

        /* Angle */
        case 'U':
        case 'u':
            if (gfxBall.angle < 90)
                gfxBall.angle++;
            break;
        case 'b':
        case 'B':
            if (gfxBall.angle > -90)
                gfxBall.angle--;
            break;

        /* View controls */
        case 'W':
        case 'w':
            if (gfxSim.vRot < 40)
                gfxSim.vRot+=2;
            break;
        case 's':
        case 'S':
            if ( gfxSim.vRot > -18)
                gfxSim.vRot-=2;
            break;
        case 'a':
        case 'A':
            gfxSim.hRot+=2;
            break;
        case 'd':
        case 'D':
            gfxSim.hRot-=2;
            break;
        case 'r':
        case 'R':
            gfxSim.vRot = 16;
            gfxSim.hRot = 0;
            break;
        case 't':
        case 'T':
            if (gfxSim.sceneRotation == GL_FALSE){
                gfxSim.sceneRotation = GL_TRUE;
            }
            else {
                gfxSim.sceneRotation = GL_FALSE;
                gfxSim.rotationAngle = 0;
            }
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

GLvoid MouseHandler(int button, int state, int x, int y)
{
    x=y=0;
    if( (state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON) )
    {
        glutPostRedisplay();
    }
}

GLvoid ContextMenuHandler(int choice)
{
    switch (choice) {
        case 0:
        default:
            break;
    }
}

/* Perigrafh ths pleuras sto epipedo YZ enos podiou tou trapeziou */
GLvoid gfxTableLegYZ(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint nx)
{
    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    glNormal3i(nx, 0, 0);
    glVertex3f(x+dx, 0, z-dx);
    glNormal3i(nx, 0, 0);
    glVertex3f(x+dx, 0, z+dx);
    glNormal3i(nx, 0, 0);
    glVertex3f(x+dx, -yLength, z-dx);
    glNormal3i(nx, 0, 0);
    glVertex3f(x+dx, -yLength, z+dx);
    glEnd();
    glPopMatrix();
}

/* Perigrafh ths pleuras sto epipedo XY enos podiou tou trapeziou */
GLvoid gfxTableLegXY(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint nz)
{
    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    glNormal3i(0, 0, nz);
    glVertex3f(x-dx, 0, z+dx);
    glNormal3i(0, 0, nz);
    glVertex3f(x+dx, 0, z+dx);
    glNormal3i(0, 0, nz);
    glVertex3f(x-dx, -yLength, z+dx);
    glNormal3i(0, 0, nz);
    glVertex3f(x+dx, -yLength, z+dx);
    glEnd();
    glPopMatrix();
}

/* Perigrafh ths pleuras sto epipedo XZ enos podiou tou trapeziou */
GLvoid gfxTableLegXZ(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength, GLint ny)
{
    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    glNormal3i(0, ny, 0);
    glVertex3f(x-dx, -yLength, z-dx);
    glNormal3i(0, ny, 0);
    glVertex3f(x+dx, -yLength, z-dx);
    glNormal3i(0, ny, 0);
    glVertex3f(x-dx, -yLength, z+dx);
    glNormal3i(0, ny, 0);
    glVertex3f(x+dx, -yLength, z+dx);
    glEnd();
    glPopMatrix();
}

/* Peigrafh enos podiou tou trapeziou */
GLvoid gfxTableLeg(GLfloat x, GLfloat z, GLfloat dx, GLfloat yLength)
{
    glPushMatrix();
    gfxTableLegYZ(x, z, -dx, yLength, -1);
    gfxTableLegYZ(x, z,  dx, yLength, +1);
    gfxTableLegXY(x, z, -dx, yLength, -1);
    gfxTableLegXY(x, z,  dx, yLength, +1);
    gfxTableLegXZ(x, z,  dx, yLength, -1);
    glPopMatrix();
}
