
#include "gl_helpers.h"
#include "stdlib.h"

#define MAX_COMBINED_MALLOCS 2500
#define MAX_COMBINED_VERTICES 2500

static void *       combined_to_free [MAX_COMBINED_MALLOCS];
static int          combined_num_to_free = 0;
static GLenum       tessVertexType;
static int          stashed_vertices;
static int          triangle_comp_idx;
static QtHID*       pHID;

/* Prototypes */
static void myBegin   ( GLenum type );
static void myVertex  ( GLdouble *vertex_data );
static void myError   (GLenum errno);
static void myCombine ( GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **dataOut );


void Qhid_fill_rect (Coord x1, Coord y1, Coord x2, Coord y2)
{
  //hidgl_ensure_triangle_space (&buffer, 2);
  pHID->ADD_triangle ( x1, y1, x1, y2, x2, y2);
  pHID->ADD_triangle ( x2, y1, x2, y2, x1, y1);
}

void Qhid_fill_polygon (int n_coords, Coord *x, Coord *y, QtHID* hid )
{
int            i;
GLUtesselator *tobj;
double        *vertices;

pHID = hid;

assert (n_coords > 0);

vertices = (double*)malloc (sizeof(double) * n_coords * 3);

tobj = gluNewTess ();
gluTessCallback(tobj, GLU_TESS_BEGIN,   (_GLUfuncptr)&myBegin);
gluTessCallback(tobj, GLU_TESS_VERTEX,  (_GLUfuncptr)&myVertex);
gluTessCallback(tobj, GLU_TESS_COMBINE, (_GLUfuncptr)myCombine);
gluTessCallback(tobj, GLU_TESS_ERROR,   (_GLUfuncptr)myError);

gluTessBeginPolygon (tobj, NULL);
gluTessBeginContour (tobj);

for (i = 0; i < n_coords; i++)
  {
    vertices [0 + i * 3] = x[i];
    vertices [1 + i * 3] = y[i];
    vertices [2 + i * 3] = 0.;
    gluTessVertex (tobj, &vertices [i * 3], &vertices [i * 3]);
  }

gluTessEndContour (tobj);
gluTessEndPolygon (tobj);
gluDeleteTess (tobj);

//myFreeCombined ();
free (vertices);
}

/* Called at the start of every Triangle */
static void myBegin( GLenum type )
{
  tessVertexType = type;
  stashed_vertices = 0;
  triangle_comp_idx = 0;
}

static void myVertex ( GLdouble *vertex_data )
{
static GLfloat triangle_vertices [2 * 3];

if (tessVertexType == GL_TRIANGLE_STRIP ||
    tessVertexType == GL_TRIANGLE_FAN)
  {
    if (stashed_vertices < 2)
      {
        triangle_vertices [triangle_comp_idx ++] = vertex_data [0];
        triangle_vertices [triangle_comp_idx ++] = vertex_data [1];
        stashed_vertices ++;
      }
    else
      {
        pHID->ADD_triangle (
                            triangle_vertices [0], triangle_vertices [1],
                            triangle_vertices [2], triangle_vertices [3],
                            vertex_data [0], vertex_data [1]);

        if (tessVertexType == GL_TRIANGLE_STRIP)
          {
            /* STRIP saves the last two vertices for re-use in the next triangle */
            triangle_vertices [0] = triangle_vertices [2];
            triangle_vertices [1] = triangle_vertices [3];
          }
        /* Both FAN and STRIP save the last vertex for re-use in the next triangle */
        triangle_vertices [2] = vertex_data [0];
        triangle_vertices [3] = vertex_data [1];
      }
  }
else if (tessVertexType == GL_TRIANGLES)
  {
    triangle_vertices [triangle_comp_idx ++] = vertex_data [0];
    triangle_vertices [triangle_comp_idx ++] = vertex_data [1];
    stashed_vertices ++;
    if (stashed_vertices == 3)
      {
        pHID->ADD_triangle(
                            triangle_vertices [0], triangle_vertices [1],
                            triangle_vertices [2], triangle_vertices [3],
                            triangle_vertices [4], triangle_vertices [5]);
        triangle_comp_idx = 0;
        stashed_vertices = 0;
      }
  }
else
  printf ("Vertex received with unknown type\n");
}

static void myError (GLenum errno)
{
  printf ("gluTess error: %s\n", gluErrorString (errno));
}

static void myCombine ( GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **dataOut )
{
  static GLdouble combined_vertices [3 * MAX_COMBINED_VERTICES];
  static int num_combined_vertices = 0;

  GLdouble *new_vertex;

  if (num_combined_vertices < MAX_COMBINED_VERTICES)
    {
      new_vertex = &combined_vertices [3 * num_combined_vertices];
      num_combined_vertices ++;
    }
  else
    {
      new_vertex = (GLdouble*)malloc (3 * sizeof (GLdouble));

      if (combined_num_to_free < MAX_COMBINED_MALLOCS)
        combined_to_free [combined_num_to_free ++] = new_vertex;
      else
        printf ("myCombine leaking %lu bytes of memory\n", 3 * sizeof (GLdouble));
    }

  new_vertex[0] = coords[0];
  new_vertex[1] = coords[1];
  new_vertex[2] = coords[2];

  *dataOut = new_vertex;
}
