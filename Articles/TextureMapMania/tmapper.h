// TMAPPER.H - Header file for TMAPPER.CPP, so external programs can link

// DEFINES //////////////////////////////////////////////////////////////////

// defines for fixed point math
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000

// defines for texture mapper triangular analysis
#define TRI_TYPE_NONE           0
#define TRI_TYPE_FLAT_TOP       1 
#define TRI_TYPE_FLAT_BOTTOM	2
#define TRI_TYPE_FLAT_MASK      3
#define TRI_TYPE_GENERAL        4
#define INTERP_LHS              0
#define INTERP_RHS              1
#define MAX_VERTICES_PER_POLY   6

// MACROS ///////////////////////////////////////////////////////////////////

#define SIGN(x) ((x) > 0 ? (1) : (-1))
#define SWAP(a,b,temp) {temp = a; a=b; b=temp;}

// TYPES ///////////////////////////////////////////////////////////////////

// basic types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

// CLASSES /////////////////////////////////////////////////////////////////

// general 3D vertex class
class VERTEX3D
{
public:

float x,y,z;
float u,v,i;

float length(void) { return(sqrt(x*x + y*y + z*z)); }

}; typedef VERTEX3D *VERTEX3D_PTR;

///////////////////////////////////////////////////////////////////////////////

// general integer vertex class
class VERTEXI3D
{
public:

int x,y,z;
int u,v,i;

float length(void) { return(sqrt(x*x + y*y + z*z)); }

}; typedef VERTEXI3D *VERTEXI3D_PTR;

/////////////////////////////////////////////////////////////////////////////////

// general face class, note that 90% of this stuff isn't used, but it
// gives you an idea of what the average 3D engine might keep track of...
class FACE3D
{
public:

int      state;      // the state of the face, visible, clipped, culled etc.
int      attr;       // attributes of face, shading, properties etc.
int      num_vertices; // numer of points that make up polygon
int      color;      // color of poly if it's flat shaded
UCHAR    *texture;   // pointer to the texture information
float    nlength;    // length of the normal
int      avg_z;      // average z of vertices, used for simple sorting
VERTEX3D  vlist[MAX_VERTICES_PER_POLY];  // general vertex list
VERTEXI3D tlist[3];                      // triangle vertex list int int form
FACE3D   *prev;      // pointer to previous face in ADS 
FACE3D   *next;      // pointer to next face in ADS

}; typedef FACE3D *FACE3D_PTR;