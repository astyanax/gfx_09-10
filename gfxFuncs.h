#ifndef _GFXFUNCS_H_
#define _GFXFUNCS_H_

#include "glut.h"

// Validity Macros
#define GFX_OK       0
#define GFX_INVALID  -1

// Errors
#define GFX_NOTICE   -1
#define GFX_MEM      -2
#define GFX_FOPEN    -3
#define GFX_FCLOSE   -4
#define GFX_NULL     -5

// Other
enum {X=0, Y, Z};
#define VERTEX_ARRAY_START 1
#define ANGLE2RAD(x) ((x) * 3.14156 / 180.0f)

/*
**  Krata tis 3 syntetagmenes (x,y,z) mias koryfhs
*/
typedef struct gfx_sVertex
{
    GLfloat x,y,z;
} gfxVertex;

/*
**  Krata tis 3 syntetagmenes (x,y,z) mias ka8etou
*/
typedef struct gfx_sNormal
{
    GLfloat x,y,z;
} gfxNormal;

/*
**  Ena trigwno perigrafetai apo tis 3 koryfes tou kai tis antistoixes normals
*/
typedef struct gfx_sTriangle
{
    /* TODO: Pi8anes plhrofories: Vertex, Normal, TexCoord, Facet */
    GLint vertex[3];
    GLint normal[3];
} gfxTriangle;

/*
**  Ena montelo apoteleitai apo trigwna (Mias kai to duck.obj den perieixe epifaneies/groups,
**  h ylopoihsh mas den ta yposthrikse. H eisagwgh tous pantws einai arketa eukolh.)
**
**  Ta trigwna perigrafontai apo koryfes kai normals. To melos "vertex" krata th lista twn vertices,
**  kai to melos "normal" thn lista twn normals. Etsi, to "triangle" krata ta indeces twn sxetikwn
**  koryfwn kai normals, opote th plhroforia thn antloume apo tous pinakes "vertex" kai "normal".
**
**  Sto duck.obj perigrafontai mono trigwna, epomenws problef8hke apokleistika h eisagwgh trigwnwn.
**
**  Einai eukolh h epektash gia fortwsh polygonwn:
**    Estw px to polygwno me koryfes {0,1,2,3,4}.
**    Auto tha metatrapei sta trigwna {0,1,2}, {0,2,3}, {0,3,4}. Dhladh, kata thn eisagwgh newn koryfwn
**    enos polygwnou, ka8e neo trigwno exei ws prwth kai deuterh koryfh thn 1h kai 3h koryfh tou
**    prohgoumenou trigwnou, enw ws trith koryfh tha exei th "nea" koryfh.
**
**  Telos, tha htan protimotero to gfxModel na krata apokleistika tis plhrofories enos montelou,
**  kai oxi plhrofories enos object pou xrhsimopoiei auto (p.x. "CoM", "Rmax", "dirAngle".
**  Gia logous RAD kai eukolias omws, ka8ws kai afou monadikh apaithsh einai h fortwsh tou papiou,
**  protimh8hke auth h proseggush.
**
*/
typedef struct gfx_sModel
{
    GLfloat CoM[3];     // H 8esh tou kentrou barous/mazas tou montelou
    GLfloat Rmax;       // H megisth apostash koryfhs apo to kentro tou montelou
    GLfloat dirAngle;   // Gwnia pou orizei thn kateu8unsh tou antikeimenou
    char name[32];      // To onoma tou arxeiou pou fortw8hke

    GLuint    numVertices;      // Plh8os vertices (mege8os pinaka "vertex")
    gfxVertex * vertex;         // O pinakas pou krataei tis syntetagmenes twn koryufwn

    GLuint    numNormals;       // Plh8os normals (mege8os pinaka "normal")
    gfxNormal * normal;         // O pinakas pou krataei tis syntetagmenes twn ka8etwn

    GLuint      numTriangles;   // Plh8os trigwnwn (mege8os pinaka "triangle")
    gfxTriangle * triangle;     // O pinakas twn trigwnwn
} gfxModel;

/* Prototypes */
gfxModel * gfxLoadObj(char * filename);
GLint gfxUnloadObj(gfxModel * model);
GLint gfxRenderObj(gfxModel * model);
GLint gfxNormalizeToUnit(gfxModel * model, GLfloat scaleFactor);
GLfloat gfxAbsf(GLfloat num);
GLvoid gfxBoolInvert(GLint * b);
GLvoid gfxBoolSetTrue(GLint * b);
GLvoid gfxBoolSetFalse(GLint * b);
GLvoid gfxDrawCollision(GLfloat r);
GLint gfxCheckCollision(GLfloat point1[3], GLfloat point2[3], GLfloat r1, GLfloat r2);

#endif
