#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <windows.h>

#include "gfxFuncs.h"

/* 
**  Ypologismos apolyths timhs
*/
GLfloat gfxAbsf(GLfloat num)
{
    return (num >= 0 ? num : -(num));
}

/* 
**  Fortwsh montelou (periorizetai stis prodiagrafes tou duck.obj)
*/
gfxModel * gfxLoadObj(char * filename)
{
    FILE * fp;
    char buffer[512];
    char * token;

    gfxModel * model;

    /*
    **   Error checking
    */
    if ( (fp = fopen(filename, "r")) == NULL ) {
        fprintf(stderr, "[Warning] Could not load file %s\n", filename);
        return NULL;
    }

    /*
    **   Initialize new model
    */
    if ( (model = (gfxModel *)malloc(sizeof(gfxModel))) == NULL ) {
        fprintf(stderr, "[Error] Not enough memory to load resource\n");
        return NULL;
    }

    strncpy(model->name, filename, 32);

    model->numTriangles = model->numVertices = model->numNormals = 0;

    model->triangle = NULL;
    model->vertex   = NULL;
    model->normal   = NULL;

    for (int i=0;i<3;i++)
        model->CoM[i] = 0.0f;

    memset(buffer, '\0', sizeof(buffer));

    /*
    **   Scan the file for needed data/statistics
    */
    while(fscanf(fp, "%s", buffer) != EOF) {
        switch ( buffer[0] )
        {
            // Vertex
        case 'v':
            /* To "v" dhlwnei koryfh kai to "vn" ka8eto, ypoloipa de mas endiaferoun */
            if ( buffer[1] == '\0') {
                model->numVertices++;
            }
            else if ( buffer[1] == 'n' ) {
                model->numNormals++;
            }
            else {
                fprintf(stderr, "[Warning] Unknown token found while loading \"%s\".\n", filename);
            }
            fgets(buffer, sizeof(buffer), fp);
            break;
            // Face
        case 'f':
            fgets(buffer, sizeof(buffer), fp);
            token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            while ( (token = strtok(NULL, " ")) != NULL ) {
                model->numTriangles++;
            }
            break;
            // Anything else (texture, facet, material etc) is ignored
        default:
            fgets(buffer, sizeof(buffer), fp);
            break;
        }
    }

    fprintf(stderr, "[Info] Read %d vertices, %d normals and %d triangles from file \"%s\"\n", model->numVertices, model->numNormals, model->numTriangles, filename);

    /*
    **   Prepare model for loading
    */
    model->triangle = (gfxTriangle *)malloc( ((model->numTriangles)) * sizeof(gfxTriangle));
    model->vertex = (gfxVertex *)malloc( ((model->numVertices)+1) * sizeof(gfxVertex));

    /* To montelo exei opwsdhpote polygwna kai koryfes, oxi omws aparaithta normals,
    epomenws elegxoume an yparxoun normals outws wste na apofygoume "mhdeniko" allocation */
    if (model->numNormals > 0)
        model->normal = (gfxNormal *)malloc( ((model->numNormals)+1) * sizeof(gfxNormal));

    /*
    **   Load the needed information
    */
    rewind(fp);

    // Boh8htikes dhlwseis gia eksoikonomish xwrou
    #define mvertex model->vertex
    #define mnormal model->normal
    #define mtriangle model->triangle

    GLuint vIndex, nIndex, tIndex, v, n;
    vIndex = nIndex = v = n = VERTEX_ARRAY_START;
    tIndex = 0;

    /*
    **  To arxiko vertex einai "invalid", koinws de prepei na xrhsimopoih8ei.
    **  Auto ginetai gia dikia mas dieukolynsh, wste na einai aplousterh h antistoixish
    **  twn trigonwn me ta vertices tous (h ari8mhsh twn opoiwn arxizei apo to 1).
    **  Kalytera dhladh na spatalh8ei mia 8esh mnhmhs para na ekteloume diarkws ena -1.
    */

    while(fscanf(fp, "%s", buffer) != EOF) {
        switch ( buffer[0] )
        {
            // Vertex
            case 'v':
                /* To "v" dhlwnei koryfh kai to "vn" ka8eto, ypoloipa de mas endiaferoun */
                if ( buffer[1] == '\0') {
                    fscanf(fp, "%f %f %f", &mvertex[vIndex].x, &mvertex[vIndex].y, &mvertex[vIndex].z);
                    vIndex++;
                }
                else if ( buffer[1] == 'n' ) {
                    fscanf(fp, "%f %f %f", &mnormal[nIndex].x, &mnormal[nIndex].y, &mnormal[nIndex].z);
                    nIndex++;
                }
                else {
                    fgets(buffer, sizeof(buffer), fp);
                }
                break;
            // Face
            case 'f':
                /* TODO: Generalize for polygons, it's enough for duck.obj which consists only of triangles */
                fgets(buffer, sizeof(buffer), fp);
                token = strtok(buffer, " ");
                // v//n
                if (sscanf(token, "%d//%d", &v, &n) == 2 ) {
                    mtriangle[tIndex].vertex[0] = v;
                    mtriangle[tIndex].normal[0] = n;
                    token = strtok(NULL, " ");
                    sscanf(token, "%d//%d", &mtriangle[tIndex].vertex[1], &mtriangle[tIndex].normal[1]);
                    token = strtok(NULL, " ");
                    sscanf(token, "%d//%d", &mtriangle[tIndex].vertex[2], &mtriangle[tIndex].normal[2]);

                    tIndex++;
                }
                // v/t/n
                else if (sscanf(token, "%d/%*d/%d", &v, &n) == 3 ) {
                    mtriangle[tIndex].vertex[0] = v;
                    mtriangle[tIndex].normal[0] = n;
                    token = strtok(NULL, " ");
                    sscanf(token, "%d/%*d/%d", &mtriangle[tIndex].vertex[1], &mtriangle[tIndex].normal[1]);
                    token = strtok(NULL, " ");
                    sscanf(token, "%d/%*d/%d", &mtriangle[tIndex].vertex[2], &mtriangle[tIndex].normal[2]);

                    tIndex++;
                }
                // v/t
                else if (sscanf(token, "%d/%*d", &v) == 2 ) {
                    mtriangle[tIndex].vertex[0] = v;
                    token = strtok(NULL, " ");
                    sscanf(token, "%d/%*d", &mtriangle[tIndex].vertex[1]);
                    token = strtok(NULL, " ");
                    sscanf(token, "%d/%*d", &mtriangle[tIndex].vertex[2]);

                    for (int i=0;i<3;i++)
                        mtriangle[tIndex].normal[i] = GFX_INVALID;

                    tIndex++;
                }
                // v
                else {
                    sscanf(token, "%d", &v);
                    mtriangle[tIndex].vertex[0] = v;
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &mtriangle[tIndex].vertex[1]);
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &mtriangle[tIndex].vertex[2]);

                    for (int i=0;i<3;i++)
                        mtriangle[tIndex].normal[i] = GFX_INVALID;

                    tIndex++;
                }
                break;
            // Ignore anything else
            default:
                fgets(buffer, sizeof(buffer), fp);
                break;
        }
    }

    /*
    **   Oloklhrw8hke epityxws to fortwma
    */
    #undef mvertex
    #undef mnormal
    #undef mtriangle

    if (fclose(fp) != 0) {
        fprintf(stderr, "[Warning] Could not close file \"%s\".\n", filename);
    }

    return model;
}

/* 
**  Diagrafh montelou
*/
GLint gfxUnloadObj(gfxModel * model)
{
    char name[32];

    if ( model == NULL ) {
        fprintf(stderr, "[Notice] Tried to unload an already-unloaded model.\n");
        return GFX_NOTICE;
    }

    if (model->vertex != NULL)
        free(model->vertex);

    if (model->triangle != NULL)
        free(model->triangle);

    if (model->normal != NULL)
        free(model->normal);

    strncpy(name, model->name, 32);

    free(model);

    fprintf(stderr, "[Status] Model %s has been unloaded.\n", name);

    return GFX_OK;
}

/* 
**  Apeikonish montelou
*/
GLint gfxRenderObj(gfxModel * model)
{
    #define mtr model->triangle[tIndex]

    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (GLuint tIndex=0 ; tIndex < model->numTriangles ; tIndex++) {
        /* Efoson den yparxoun normals, den prepei na symperilif8oun ston ypologismo oi invalid times */
        if (model->normal[mtr.normal[X]].x != GFX_INVALID)
            glNormal3f(model->normal[mtr.normal[X]].x, model->normal[mtr.normal[X]].y, model->normal[mtr.normal[X]].z);
        glVertex3f(model->vertex[mtr.vertex[X]].x, model->vertex[mtr.vertex[X]].y, model->vertex[mtr.vertex[X]].z);
        if (model->normal[mtr.normal[Y]].x != GFX_INVALID)
            glNormal3f(model->normal[mtr.normal[Y]].x, model->normal[mtr.normal[Y]].y, model->normal[mtr.normal[Y]].z);
        glVertex3f(model->vertex[mtr.vertex[Y]].x, model->vertex[mtr.vertex[Y]].y, model->vertex[mtr.vertex[Y]].z);
        if (model->normal[mtr.normal[Z]].x != GFX_INVALID)
            glNormal3f(model->normal[mtr.normal[Z]].x, model->normal[mtr.normal[Z]].y, model->normal[mtr.normal[Z]].z);
        glVertex3f(model->vertex[mtr.vertex[Z]].x, model->vertex[mtr.vertex[Z]].y, model->vertex[mtr.vertex[Z]].z);
    }
    glEnd();
    glPopMatrix();

    #undef mtr

    return GFX_OK;
}

/* 
**  Scaling tou montelou arxika se "kouti" mege8ous monadas, kai meta scaling kata "scaleFactor"
**  Leitourgei ws NormalizeToUnit me scaleFactor = 1
*/
GLint gfxNormalizeToUnit(gfxModel * model, GLfloat scaleFactor)
{
    #define mvertex model->vertex
    GLfloat com[3];             // Kentro mazas tou montelou
    GLfloat finalScaleFactor;   // scaleFactor/Rmax
    GLfloat Rmax=0, r;          // (Megisth) apostash koryfhs apo com

    // arxikopoihsh
    com[X] = com[Y] = com[Z] = 0;

    /* Efoson mas exei dw8ei swsto montelo */
    if (model != NULL && model->vertex != NULL)
    {
        for (GLuint vIndex = VERTEX_ARRAY_START ; vIndex <= model->numVertices ; vIndex++)
        {
            com[X] += mvertex[vIndex].x;
            com[Y] += mvertex[vIndex].y;
            com[Z] += mvertex[vIndex].z;

            /* Eukleidia apostash shmeiwn (x,y,z) kai (0,0,0) se 3D xwro */
            r = sqrt(mvertex[vIndex].x*mvertex[vIndex].x +
                     mvertex[vIndex].y*mvertex[vIndex].y +
                     mvertex[vIndex].z*mvertex[vIndex].z);

            if (Rmax < r)
                Rmax = r;
        }

        finalScaleFactor = scaleFactor / Rmax;

        /*
        **  Metakinoume ola ta vertices wste to kentro mazas na einai sto (0,0,0)
        **  kai tautoxrona kanoume to katallhlo scaling.
        */
        for (GLuint vIndex = 1 ; vIndex <= model->numVertices ; vIndex++){
            mvertex[vIndex].x -= model->CoM[X];
            mvertex[vIndex].y -= model->CoM[Y];
            mvertex[vIndex].z -= model->CoM[Z];
            mvertex[vIndex].x *= finalScaleFactor;
            mvertex[vIndex].y *= finalScaleFactor;
            mvertex[vIndex].z *= finalScaleFactor;
        }

        // Afou metakinhsame oles tis koryfes, kairos na ananewsoume tis times
        model->CoM[X]=0; model->CoM[Y]=0; model->CoM[Z]=0;
        model->Rmax = Rmax*scaleFactor;
    }
    else {
        fprintf(stderr, "[Warning] Normalization failed due to invalid input.\n");
        return GFX_NULL;
    }

    return GFX_OK;
}

// Invert boolean value
GLvoid gfxBoolInvert(GLint * b)
{
    if (*b == GL_TRUE)
        *b = GL_FALSE;
    else
        *b = GL_TRUE;
}

// Set boolean value to true
GLvoid gfxBoolSetTrue(GLint * b)
{
    *b = GL_TRUE;
}

// Set boolean value to false
GLvoid gfxBoolSetFalse(GLint * b)
{
    *b = GL_FALSE;
}

/*
**  Elegxoume an ginetai sfairiko collision metaksy 2 shmeiwn
*/
GLint gfxCheckCollision(GLfloat point1[3], GLfloat point2[3], GLfloat r1, GLfloat r2)
{
    GLfloat d;

    /* Ypologizoume thn apostash twn 2 shmeiwn */
    d = sqrt(pow(point2[X]-point1[X], 2) + pow(point2[Y]-point1[Y], 2) + pow(point2[Z]-point1[Z], 2));

    // As exoume kai ligo intersection stis bounding spheres
    // (0.8 anti gia 1, epi8ymoume epikalypsh gia elafrws dyskolotero game :P)
    if (d > 0.8*(r1+r2) )
        return GL_FALSE;
    return GL_TRUE;    
}

/*
**  Rendering tou efe sugrkoushs (h 8esh prepei na proka8oristei)
*/
GLvoid gfxDrawCollision(GLfloat r)
{
    GLfloat xst, yst;
    GLfloat colour[4];

    /* Swzoume to twrino xrwma */
    glGetFloatv(GL_CURRENT_COLOR, colour);

    /* Zwgrafizoume to "HIT!" */
    glPushMatrix();
    glLineWidth(2);
    glColor3f(255, 0, 0);
    glTranslatef(-1, -0.2, 0.05);
    glScalef(0.012, 0.01, 0.01);
    glutStrokeCharacter(GLUT_STROKE_ROMAN,'H');
    glutStrokeCharacter(GLUT_STROKE_ROMAN,'I');
    glutStrokeCharacter(GLUT_STROKE_ROMAN,'T');
    glutStrokeCharacter(GLUT_STROKE_ROMAN,'!');
    glPopMatrix();

    /* Orizoume to ladi xrwma */
    glColor3f(0.7, 0.7, 0.5);

    /* Zwgrafizoume to asteri */
    glPushMatrix();
    glBegin(GL_POLYGON);
    for (GLint i=0; i<360; i+=30)
    {
        xst=1.2*r*cos(ANGLE2RAD(i));
        yst=1.2*r*sin(ANGLE2RAD(i));
        glNormal3i(0, 0, 1);
        glVertex2f(xst,yst);
        xst=2*r*cos(ANGLE2RAD(i+15));
        yst=2*r*sin(ANGLE2RAD(i+15));
        glNormal3i(0, 0, 1);
        glVertex2f(xst,yst);
    }	
    glEnd();
    glPopMatrix();

    /* Epistrefoume sto palio xrwma */
    glColor4fv(colour);
}
