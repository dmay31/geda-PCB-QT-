#include "qthid.h"

/*-----------------------------------------------
                       Variables
 ----------------------------------------------*/
QtHID*   QtHID::pMe;
HID*     QtHID::pIntf;
HID_DRAW QtHID::QtGraphics;

/*-----------------------------------------------
                       Functions
 ----------------------------------------------*/



/* Constructor */
QtHID::QtHID(QWidget *Parent)
{
/* Default Zoom Level */
zoom = 1;
tx = 0;
ty = 0;

/* Default Pixels Per X */
ppx = 1 / zoom;


setMouseTracking(true);
qtGreen = QColor::fromRgbF(0,0,0,0);

setFocusPolicy(Qt::StrongFocus);

}

/* Destructor */
QtHID::~QtHID()
{

}


void QtHID::initializeGL()
{
qglClearColor(qtGreen.dark());
}
/** @brief Instantiates a new HID object and registers it
 *         with the PCB framework.
 *
 *  @param Void.
 *  @return Void.
 */
void QtHID::init_interface( HID* intf )
{
intf->struct_size              = sizeof (HID);
intf->name                     = "qt";
intf->description              = "Qt Interface";
intf->gui                      = 1;
intf->poly_after               = 1;

intf->parse_arguments          = parse_arguments;
intf->logv                     = logger;
intf->graphics                 = &QtGraphics;
intf->graphics->make_gc        = make_gc;
intf->graphics->set_color      = set_color;
intf->graphics->set_line_cap   = set_line_cap;
intf->graphics->draw_line      = draw_a_line;
intf->notify_crosshair_change  = update_widget;
intf->set_crosshair            = set_crosshair;
intf->graphics->set_draw_xor   = set_draw_xor;
intf->graphics->set_line_width = set_line_width;

/* Store pointer to the HID object */
QtHID::pIntf = intf;

QtHID::pMe = this;
}
/**
 * @brief  Build a new Graphic Context object
 *         which will hold the contextual state
 *         for the HID exporter
 *
 * @param  Void.
 * @return Pointer to the GC object
 */
hidGC QtHID::make_gc( void )
{
/* Variables */
hidGC gc;

/* Allocate memory for GC */
gc = (hidGC)malloc( sizeof(struct hid_gc_struct) );

/* Setup GC */
gc->me_pointer = QtHID::pIntf;
gc->colorname = Settings.BackgroundColor;
gc->alpha_mult = 1.0;
return gc;
} /* make_gc() */

/**
 * @brief Parse the command line arguments
 *
 * @param Command line flags
 * @return Void.
 */
void QtHID::parse_arguments( int *argc, char ***argv )
{
ghid_config_init();
} /* parse_arguments() */

/**
 * @brief Prints formated text to a logger
 *        object that lives in the main
 *        window application
 *
 * @param Format string
 * @param variable list of data to print in formated
 *        text
 * @return Void.
 */
void QtHID::logger( const char *fmt, va_list args )
{
//TODO
} /* logger() */

/**
 * @brief Set the color for the graphics context
 *
 * @param The graphic context object
 * @param The color name
 * @return Void.
 */
void QtHID::set_color(hidGC gc, const char *name)
{
gc->colorname = name;
} /* set_color() */

/**
 * @brief Set the line style
 *
 * @param Grapics Context
 * @param Line Style
 * @return Void.
 */
void QtHID::set_line_cap( hidGC gc, EndCapStyle style )
{
gc->cap = style;
} /* set_line_cap() */

void QtHID::draw_a_line( hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2 )
{
QtHID::pMe->draw_line( gc->cap, gc->width, x1, y1, x2, y2, 1 );
}

void QtHID::update_widget( bool change_complete )
{
QtHID::pMe->update();
}

void QtHID::set_crosshair( int x, int y, int action )
{
QtHID::pMe->setcrosshairs( x, y);
}

void QtHID::set_draw_xor( hidGC gc, int xor_ )
{
//TODO
}

/**
 * @Brief Draw a circular cap on a blunt end
 *
 * @param Width of the end
 * @param X center location
 * @param Y center location
 * @param Angle of the blunt end
 * @param Scale factor
 */
void QtHID::draw_cap (Coord width, Coord x, Coord y, Angle angle, double scale)
{
#define MIN_TRIANGLES_PER_CAP 3
#define MAX_TRIANGLES_PER_CAP 90
float last_capx, last_capy;
float capx, capy;
float radius = width / 2.;
int slices;
int i;

/* Calculate the number of slices for this circle */
slices = calc_slices (radius / scale, M_PI);

/* Boundary Checking */
if (slices < MIN_TRIANGLES_PER_CAP)
    {
    slices = MIN_TRIANGLES_PER_CAP;
    }

/* Boundary Checking */
if (slices > MAX_TRIANGLES_PER_CAP)
    {
    slices = MAX_TRIANGLES_PER_CAP;
    }

/* Check the buffer is large enough */
if( !check_triangle_space (&buffer, slices) )
    {
    /* Very bad error */
    exit( 1 );
    }

/* Get the starting x,y coordinates */
last_capx =  radius * cosf (angle * M_PI / 180.) + x;
last_capy = -radius * sinf (angle * M_PI / 180.) + y;

for (i = 0; i < slices; i++)
    {
  /* Get slice[i+1] x and y */
    capx =  radius * cosf (angle * M_PI / 180. + ((float)(i + 1)) * M_PI / (float)slices) + x;
    capy = -radius * sinf (angle * M_PI / 180. + ((float)(i + 1)) * M_PI / (float)slices) + y;

    /* Add triangle to the buffer */
    add_triangle (&buffer, last_capx, last_capy, capx, capy, x, y);

    /* Store the last coordinate */
    last_capx = capx;
    last_capy = capy;
    }
} /* draw_cap() */

bool QtHID::check_triangle_space( triangle_buffer *buffer, int count )
{
/* Local variables */
bool ret = false;

/* Check that count will fit if buffer is empty */
if( count <= TRIANGLE_ARRAY_SIZE )
    {
    /* Flush buffer in more room is needed */
    if( count > ( TRIANGLE_ARRAY_SIZE - buffer->triangle_count ) )
        {
        flush_triangles( buffer );
        }
    /* return no error */
    ret = true;
    }

return( ret );
}


void QtHID::set_line_width( hidGC gc, Coord width )
{
gc->width = width;
}

#define MAX_PIXELS_ARC_TO_CHORD 0.5
#define MIN_SLICES 6
int QtHID::calc_slices (float pix_radius, float sweep_angle )
{
  float slices;

  if (pix_radius <= MAX_PIXELS_ARC_TO_CHORD)
    return MIN_SLICES;

  slices = sweep_angle / acosf (1 - MAX_PIXELS_ARC_TO_CHORD / pix_radius) / 2.;
  return (int)ceilf (slices);
}

#define MIN_SLICES_PER_ARC 6
#define MAX_SLICES_PER_ARC 360

/**
 * @Brief Draw an arc with with set width, coord, and inner and outer diameter
 *
 * @param Width - Width of the Band around the arc
 * @param X position for the center of the circle
 * @parma Y position for the cneter of the circle
 * @param Radius of the circle in the horizontal direction
 * @param Radius of the circle in the vertical direction ( not used )
 * @param Start Angle
 * @param Delta Angle - The degrees the arc will sweep through
 * @param Scale
 *
 * @return Void.
*/
void QtHID::draw_arc
    (
    Coord       width,           // Width of the band
    Coord       x,               // x position, center of circle
    Coord       y,               // y position, center of circle
    Coord       rx,              // radius of arc, to the outside of the band
    Coord       ry,              // Unused!
    Angle       start_angle,     // Start angle
    Angle       delta_angle,     // Degrees to sweep the arc
    double      scale            // Current scale factor
    )
{
/* Local Variables */
float last_inner_x, last_inner_y;
float last_outer_x, last_outer_y;
float inner_x, inner_y;
float outer_x, outer_y;
float inner_r;
float outer_r;
float cos_ang, sin_ang;
float start_angle_rad;
float delta_angle_rad;
float angle_incr_rad;
int slices;
int i;
int hairline = 0;

/* Check for hair line arc */
if (width == 0.0)
    {
    hairline = 1;
    }

/* Sanity check the width against scale */
if (width < scale)
    {
    width = scale;
    }

/* Calculate inner and outer radius */
inner_r = rx - width / 2.;
outer_r = rx + width / 2.;

/* Covert negative delta angle */
if (delta_angle < 0)
    {
    start_angle += delta_angle;
    delta_angle = - delta_angle;
    }

/* Get starting and ending angle in Radians */
start_angle_rad = start_angle * M_PI / 180.;
delta_angle_rad = delta_angle * M_PI / 180.;

slices = calc_slices ( outer_r / scale, delta_angle_rad);

/* Boundary checking */
if (slices < MIN_SLICES_PER_ARC)
  slices = MIN_SLICES_PER_ARC;

/* Boundary checking */
if (slices > MAX_SLICES_PER_ARC)
  slices = MAX_SLICES_PER_ARC;

/* Check for room and flush if needed */
if( !check_triangle_space (&buffer, 2 * slices) )
    {
    /* Very bad error */
    exit( 1 );
    }

/* Radians per slice */
angle_incr_rad = delta_angle_rad / (float)slices;

/* Get the x/r and y/r ratios */
cos_ang = cosf (start_angle_rad);
sin_ang = sinf (start_angle_rad);

/* Get X and Y positions for inner arc*/
last_inner_x = -inner_r * cos_ang + x;
last_inner_y = inner_r * sin_ang + y;

/* Get X and Y positions for outer arc */
last_outer_x = -outer_r * cos_ang + x;
last_outer_y = outer_r * sin_ang + y;

/* iterate over each slice, drawing a block with two triangles */
for (i = 1; i <= slices; i++)
    {
    /* Get the x/r and y/r ratios for each 'spoke' */
    cos_ang = cosf (start_angle_rad + ((float)(i)) * angle_incr_rad);
    sin_ang = sinf (start_angle_rad + ((float)(i)) * angle_incr_rad);

    /* Get the X and Y for inner arc of slice[i] */
    inner_x = -inner_r * cos_ang + x;
    inner_y = inner_r * sin_ang + y;

    /* Get the X and Y for outer arc of slice[i] */
    outer_x = -outer_r * cos_ang + x;
    outer_y = outer_r * sin_ang + y;

    /* Triangle Layout Diagram             */
    /*    inner(x,y)                       */
    /*     ________                        */
    /*    |       /| outer(x,y)            */
    /*    |      / |                       */
    /*    |     /  |                       */
    /*    |    /   |                       */
    /*    |   /    |  1 Slice              */
    /*    |  /     |                       */
    /*    | /      |                       */
    /*    |/_______| last_outer(x,y)       */
    /*    last_inner(x,y)_                 */

    /* Add lower triangle */
    add_triangle ( &buffer,
                   last_inner_x, last_inner_y,
                   last_outer_x, last_outer_y,
                   outer_x, outer_y
                 );

    /* Add upper triangle */
    add_triangle ( &buffer,
                   last_inner_x, last_inner_y,
                   inner_x, inner_y,
                   outer_x, outer_y
                 );

    /* Save current points for next iteration */
    last_inner_x = inner_x;
    last_inner_y = inner_y;
    last_outer_x = outer_x;
    last_outer_y = outer_y;
    }

/* Don't bother capping hair line widths */
if( !hairline )
    {
  /* Draw a semicircle cap at the start */
    draw_cap( width,
              x + rx * -cosf (start_angle_rad),
              y + rx *  sinf (start_angle_rad),
              start_angle, scale
            );

    /* Draw a semicircle cap at the end */
    draw_cap ( width,
               x + rx * -cosf (start_angle_rad + delta_angle_rad),
               y + rx *  sinf (start_angle_rad + delta_angle_rad),
               start_angle + delta_angle + 180., scale
             );
    }
} /* QtHID::draw_arc() */

/**
 * @Brief Flushed the triangle buffer to the openGL system so they can be rendered.
 *
 * @param Pointer an array of Triangles
 * @return Void.
 */
void QtHID::flush_triangles( triangle_buffer* buffer )
{
/* Don't flush if the buffer is empty! */
if (buffer->triangle_count != 0)
    {
    /* Set GL state to vertex array */
    glEnableClientState (GL_VERTEX_ARRAY);

    /* Point the GL system to the buffer */
    glVertexPointer (3, GL_FLOAT, 0,buffer->triangle_array);

    /* Draw the array as triangles */
    glDrawArrays (GL_TRIANGLES, 0, buffer->triangle_count * 3);

    glScalef( this->zoom, this->zoom, 0 );

    /* Disable the vertex array */
    glDisableClientState (GL_VERTEX_ARRAY);

    /* Clean up the buffer tracking variables */
    buffer->triangle_count = 0;
    buffer->coord_comp_count = 0;
    }
} /* flush_triangles() */

void QtHID::draw_line(int cap, Coord width, Coord x1, Coord y1, Coord x2, Coord y2, double scale)
{
      double angle;
      double deltax, deltay, length;
      float wdx, wdy;
      int circular_caps = 0;
      int hairline = 0;

      if (width == 0.0)
        hairline = 1;

      if (width < scale)
        width = scale;

      deltax = x2 - x1;
      deltay = y2 - y1;

      length = sqrt (deltax * deltax + deltay * deltay);

      if (length == 0) {
        /* Assume the orientation of the line is horizontal */
        wdx = -width / 2.;
        wdy = 0;
        length = 1.;
        deltax = 1.;
        deltay = 0.;
      } else {
        wdy = deltax * width / 2. / length;
        wdx = -deltay * width / 2. / length;
      }

      angle = -180. / M_PI * atan2 (deltay, deltax);

      switch (cap) {
        case Trace_Cap:
        case Round_Cap:
          circular_caps = 1;
          break;

        case Square_Cap:
        case Beveled_Cap:
          x1 -= deltax * width / 2. / length;
          y1 -= deltay * width / 2. / length;
          x2 += deltax * width / 2. / length;
          y2 += deltay * width / 2. / length;
          break;
      }

      //hidgl_ensure_triangle_space (&buffer, 2);
      this->add_triangle(&buffer, x1 - wdx, y1 - wdy,
                                   x2 - wdx, y2 - wdy,
                                   x2 + wdx, y2 + wdy);
      add_triangle (&buffer, x1 - wdx, y1 - wdy,
                                   x2 + wdx, y2 + wdy,
                                   x1 + wdx, y1 + wdy);

    /* Don't bother capping hairlines */
    if (circular_caps && !hairline)	    {
        draw_cap (width, x1, y1, angle + 90., scale);
        draw_cap (width, x2, y2, angle - 90., scale);
      }


}

void QtHID::add_triangle(triangle_buffer *buffer,
        float x1, float y1,
        float x2, float y2,
        float x3, float y3)
{
            add_triangle_3D (buffer, x1, y1, 0,
                                     x2, y2, 0,
                                     x3, y3, 0);
}

void QtHID::add_triangle_3D (triangle_buffer *buffer,
                       float x1, float y1, float z1,
                       float x2, GLfloat y2, float z2,
                       float x3, float y3, float z3)
{
  buffer->triangle_array [buffer->coord_comp_count++] = x1;
  buffer->triangle_array [buffer->coord_comp_count++] = y1;
  buffer->triangle_array [buffer->coord_comp_count++] = z1;
  buffer->triangle_array [buffer->coord_comp_count++] = x2;
  buffer->triangle_array [buffer->coord_comp_count++] = y2;
  buffer->triangle_array [buffer->coord_comp_count++] = z2;
  buffer->triangle_array [buffer->coord_comp_count++] = x3;
  buffer->triangle_array [buffer->coord_comp_count++] = y3;
  buffer->triangle_array [buffer->coord_comp_count++] = z3;
  buffer->triangle_count++;
}
void QtHID::mouseMoveEvent ( QMouseEvent * e )
{
bool moved;
Coord x, y;

/* Grab new mouse coordinates */
x = (Coord)e->x();
y = (Coord)e->y();

/* Notify framework of change */
moved = MoveCrosshairAbsolute(x, y);
gui->notify_crosshair_change(true);

} /* mouseMoveEvent() */

void QtHID::wheelEvent ( QWheelEvent * e )
{
float Glx;
float Gly;

#define    ZOOM_PER_WHEEL_CLICK        ( 0.1 )
int delta;
int sign;
float zoom_change;
float zoom_prcnt;

/* Get the GL coordinate under the current mouse position before the zoom */
Glx = ( e->x() - tx ) / zoom ;
Gly = ( e->y() - ty ) / zoom ;

/* Get the zoom direction */
delta = e->delta();
sign = ( delta / abs(delta) );

/* Set the zoom change */
zoom_change = ZOOM_PER_WHEEL_CLICK * sign;

/* Calculate the percent change in zoom */
zoom_prcnt = 1 - ( zoom_change + zoom  * ppx );

/* Calculate how far the x & y components of the GL point will move */
tx += Glx * zoom_prcnt;;
ty += Gly * zoom_prcnt;

/* Store the new zoom level */
zoom += zoom_change;
ppx = 1 / zoom;

/* Update the screen */
this->updateGL();
} /* wheelEvent() */

void QtHID::mousePressEvent( QMouseEvent* e )
{
printf("Button %d\n", e->button() );
}

void QtHID::keyPressEvent( QKeyEvent *e )
{
QPoint *	point;

point = NULL;

switch( e->key() )
    {
    case Qt::Key_Up:
        {
        point = new QPoint(qport.x, qport.y - 1);
        break;
        }

    case Qt::Key_Down:
        {
        point = new QPoint(qport.x, qport.y + 1);
        break;
        }

    case Qt::Key_Right:
        {
        point = new QPoint(qport.x + 1, qport.y);
        break;
        }

    case Qt::Key_Left:
        {
        point = new QPoint(qport.x - 1, qport.y);
        break;
        }

    default:
        break;
    }
if( NULL != point )
    {
    this->cursor().setPos( mapToGlobal(*point) );
    delete point;
    }

this->update();
} /* keyPressEvent() */


/* This is the main repaint function */
void QtHID::paintGL(void)
{
BoxType region;


glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//DrawMark();
//flush_triangles(&buffer);
glColor3f( 1.0f, 1.0f, 1.0f );
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();

glOrtho (0, this->width(), this->height(), 0, 0, 100);

glMatrixMode (GL_MODELVIEW);

glTranslatef( tx, ty, 0.0f );
glScalef (zoom, zoom, 0 );

region.X1 = 0;
region.Y1 = 0;
region.X2 = this->width();
region.Y2 = this->height();


hidgl_draw_grid ( &region );
draw_arc(6, 300, 300, 10, 10, 0, 360, 1 );
this->draw_arc( 30, 200, 200, 40, 0, 0, 360, 1 );
this->draw_arc( 30, 400, 200, 40, 0, 0, 360, 1 );
this->draw_line( Round_Cap, 5, 300,300,500,300, 1 );
//DrawAttached();
this->flush_triangles( &buffer );

Drawcrosshairs();
}

void QtHID::Drawcrosshairs(void)
{
double x, y, z;

x = qport.x;
y = qport.y;
z = 0;


glEnable (GL_COLOR_LOGIC_OP);
glLogicOp (GL_XOR);

//glColor3f (cross_color.red / 65535.,
//          cross_color.green / 65535.,
//          cross_color.blue / 65535.);
//glViewport (0, 0, 955, 710);
glColor3f( 1.0f, 1.0f, 1.0f );
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();

glOrtho (0, this->width(), this->height(), 0, 0, 100);

glMatrixMode (GL_MODELVIEW);
glTranslatef (0.0f, 0.0f, -3);
glLoadIdentity ();

glBegin (GL_LINES);

//x = x/this->width();
//y = y/this->height();

draw_right_cross (x, y, z);

if (Crosshair.shape == Union_Jack_Crosshair_Shape)
  draw_slanted_cross (x, y, z);
if (Crosshair.shape == Dozen_Crosshair_Shape)
  draw_dozen_cross (x, y, z);

glEnd ();

glDisable (GL_COLOR_LOGIC_OP);

}


void QtHID::setcrosshairs( int x, int y )
{
qport.x = x;
qport.y = y;

}

void QtHID::draw_right_cross (float x, float y,int z)
{
/*
  x = (2*x)-1;
  y= -1*(2*y-1);

  glVertex3f ((2*x)-1, 1, z);
  glVertex3f ((2*x)-1, -1, z);
  glVertex3f (1, -1*(2*y-1), z);
  glVertex3f (-1, -1*(2*y-1), z);
  */

  glVertex3i (x, 0, z);
  glVertex3i (x, PCB->MaxHeight, z);
  glVertex3i (0, y, z);
  glVertex3i (PCB->MaxWidth, y, z);
}


void QtHID::draw_slanted_cross (int x, int y, int z)
{
  int x0, y0, x1, y1;

  x0 = x + (PCB->MaxHeight - y);
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x - y;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + (PCB->MaxWidth - x);
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - x;
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);

  x0 = x - (PCB->MaxHeight - y);
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x + y;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + x;
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - (PCB->MaxWidth - x);
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);
}

void QtHID::draw_dozen_cross (int x, int y, int z)
{
  int x0, y0, x1, y1;
  double tan60 = sqrt (3);

  x0 = x + (PCB->MaxHeight - y) / tan60;
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x - y / tan60;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + (PCB->MaxWidth - x) * tan60;
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - x * tan60;
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);

  x0 = x + (PCB->MaxHeight - y) * tan60;
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x - y * tan60;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + (PCB->MaxWidth - x) / tan60;
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - x / tan60;
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);

  x0 = x - (PCB->MaxHeight - y) / tan60;
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x + y / tan60;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + x * tan60;
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - (PCB->MaxWidth - x) * tan60;
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);

  x0 = x - (PCB->MaxHeight - y) * tan60;
  x0 = MAX(0, MIN (x0, PCB->MaxWidth));
  x1 = x + y * tan60;
  x1 = MAX(0, MIN (x1, PCB->MaxWidth));
  y0 = y + x / tan60;
  y0 = MAX(0, MIN (y0, PCB->MaxHeight));
  y1 = y - (PCB->MaxWidth - x) / tan60;
  y1 = MAX(0, MIN (y1, PCB->MaxHeight));
  glVertex3i (x0, y0, z);
  glVertex3i (x1, y1, z);
}

void QtHID::resizeGL(int width, int height)
{
    int side = qMin(width, height);

    glViewport(0, 0, width, height);

  //  glMatrixMode(GL_PROJECTION);
  //  glLoadIdentity();
#ifdef QT_OPENGL_ES_1
  //  glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
  //  glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
  //  glMatrixMode(GL_MODELVIEW);
}

void QtHID::hidgl_draw_grid (BoxType *drawn_area)
{
  static GLfloat *points = 0;
  static int npoints = 0;
  Coord x1, y1, x2, y2, n, i;
  double x, y;

  if (!Settings.DrawGrid)
 //   return;

      glEnable (GL_COLOR_LOGIC_OP);
      glLogicOp (GL_XOR);
      glColor3f( 1.0f, 1.0f, 1.0f );

  x1 = GridFit (MAX (0, drawn_area->X1), 25/*PCB->Grid*/ , PCB->GridOffsetX);
  y1 = GridFit (MAX (0, drawn_area->Y1), 25/*PCB->Grid*/ , PCB->GridOffsetY);
  x2 = GridFit (MIN (PCB->MaxWidth, drawn_area->X2), 25/*PCB->Grid*/ , PCB->GridOffsetX);
  y2 = GridFit (MIN (PCB->MaxHeight, drawn_area->Y2), 25/*PCB->Grid*/ , PCB->GridOffsetY);

  if (x1 > x2)
    {
      Coord tmp = x1;
      x1 = x2;
      x2 = tmp;
    }

  if (y1 > y2)
    {
      Coord tmp = y1;
      y1 = y2;
      y2 = tmp;
    }

  n = (int) ((x2 - x1) / /*PCB->Grid*/ 25 + 0.5) + 1;
  if (n > npoints)
    {
      npoints = n + 10;
      points = (GLfloat*)realloc (points, npoints * 3 * sizeof (GLfloat));
    }

  glEnableClientState (GL_VERTEX_ARRAY);
  glVertexPointer (3, GL_FLOAT, 0, points);

  n = 0;
  for (x = x1; x <= x2; x += /*PCB->Grid*/25)
    {
      points[3 * n + 0] = x;
      points[3 * n + 2] = 0;
      n++;
    }
  for (y = y1; y <= y2; y += /*PCB->Grid*/25)
    {
      for (i = 0; i < n; i++)
      {
        points[3 * i + 1] = y;
      }

      glDrawArrays (GL_POINTS, 0, n);
    }



  glDisableClientState (GL_VERTEX_ARRAY);
  glDisable (GL_COLOR_LOGIC_OP);
}

Coord QtHID::GridFit (Coord x, Coord grid_spacing, Coord grid_offset)
{
  x -= grid_offset;
  x = grid_spacing * round ((double) x / grid_spacing);
  x += grid_offset;
  return x;
}

void QtHID::valueChanged(bool value)
{
    printf("one clicked %d\n", value);
}

#include "qthid.moc"
