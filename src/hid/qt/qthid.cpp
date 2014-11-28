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
setMouseTracking(true);
qtGreen = QColor::fromRgbF(0,0,0,0);

}

/* Destructor */
QtHID::~QtHID()
{

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
//TODO
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

void QtHID::set_line_width( hidGC gc, Coord width )
{
gc->width = width;
}

void QtHID::flush_triangles(triangle_buffer* buffer)
	{
	 // if (buffer->triangle_count == 0)
	 //   return;


      glEnableClientState (GL_VERTEX_ARRAY);
      glVertexPointer (3, GL_FLOAT, 0,buffer->triangle_array);
	  glDrawArrays (GL_TRIANGLES, 0, buffer->triangle_count * 3);

	  glDisableClientState (GL_VERTEX_ARRAY);



	  buffer->triangle_count = 0;
	  buffer->coord_comp_count = 0;
	}

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

//	  /* Don't bother capping hairlines */
//	  if (circular_caps && !hairline)
//	    {
//	      draw_cap (width, x1, y1, angle + 90., scale);
//	      draw_cap (width, x2, y2, angle - 90., scale);
//	    }


}

void QtHID::add_triangle(triangle_buffer *buffer,
        float x1, float y1,
        float x2, float y2,
        float x3, float y3)
{
	        add_triangle_3D (buffer, x1, y1, 1,
	                                 x2, y2, 1,
	                                 x3, y3, 1);
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

	x = (Coord)e->x();
	y = (Coord)e->y();

	moved = MoveCrosshairAbsolute(x, y);
	gui->notify_crosshair_change(true);

}

/* This is the main repaint function */
void QtHID::paintGL(void)
{
BoxType region;

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//DrawMark();
//flush_triangles(&buffer);

region.X1 = 0;
region.Y1 = 0;
region.X2 = this->width();
region.Y2 = this->height();

hidgl_draw_grid ( &region );

Drawcrosshairs();


}

void QtHID::initializeGL()
{
qglClearColor(qtGreen.dark());
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
printf(" %f    %f     %d     %d \n", x, y, this->width(), this->height() );
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
