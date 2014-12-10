
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "assert.h"

#include "qthid.hpp"

class QtHID;

void Qhid_fill_polygon( int n_coords, Coord *x, Coord *y, QtHID* hid );
void Qhid_fill_rect (Coord x1, Coord y1, Coord x2, Coord y2);

