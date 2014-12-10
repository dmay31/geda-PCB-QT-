#include "qthid.hpp"

#include "assert.h"
extern "C" {
#include "polygon.h"
}

#include <stdlib.h>
#include <QOpenGLBuffer>
#include <QGLFormat>
#include <QGLWidget>
#include <QGLBuffer>

#include "ft2build.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include FT_FREETYPE_H

/*-----------------------------------------------
                       Variables
 ----------------------------------------------*/
QtHID*   QtHID::pMe;
HID*     QtHID::pIntf;
HID_DRAW QtHID::QtGraphics;

/*-----------------------------------------------
                       Functions
 ----------------------------------------------*/

GuiTimer::GuiTimer()
{
this->pTimerObj = NULL;
this->Callback_func = NULL;
}

GuiTimer::~GuiTimer()
{
printf("Timer Destroyed\n");
}

void GuiTimer::timeout()
{
if( this->Callback_func != NULL )
    {
    printf("Timer Expired\n");
    this->Callback_func( this->user_data );
    delete this->pTimerObj;
    delete this;
    }
}

/* Constructor */
QtHID::QtHID(QWidget *Parent)
{
int error;
bool err;

QSizePolicy size( QSizePolicy::Expanding, QSizePolicy::Expanding );
/* Default Zoom Level */
ppx = 6000;
tx = 0;
ty = 0;

BGLint = false;

cntrl_pressed = false;
shift_pressed = false;
pbuffer = NULL;
cube = NULL;
geom = NULL;

/* Default GL Coordinates per pixel */
zoom = 1/ppx;

/* Setup some default colors */
qport.Offlimit_color = new QColor("Black");
qport.Background_color = new QColor("Black");

setMouseTracking(true);
setFocusPolicy(Qt::StrongFocus);
setGeometry(0,0,200,200);

this->setContextMenuPolicy(Qt::CustomContextMenu);

connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));

/* A Singleton Design pattern */
if( NULL != QtHID::pMe )
    {
    delete[] QtHID::pMe;
    }

QtHID::pMe = this;

}

/* Destructor */
QtHID::~QtHID()
{
delete[] qport.Offlimit_color;
delete[] qport.Background_color;
}

/** @brief Publicly Add triangles to the OpenGL object
 *
 * @param 3 vertex points
 * @return Void.
 */
void QtHID::ADD_triangle (float x1, float y1, float x2, float y2, float x3, float y3 )
{
/* Add it to the main Buffer */
//this->check_triangle_space( &this->buffer, 1 );
this->add_triangle( &triangleVertxBuffer, x1,y1,x2,y2,x3,y3 );
}


void QtHID::initializeGL()
{

bool                 err;
int                  model, view, projection, scale, translation;
QGLFormat glFormat = QGLWidget::format();
float                a,b,c,d;
GLint MaxPatchVertices = 0;
glm::vec4            color;

glFormat.setSampleBuffers( true );

Projection = glm::ortho( 0.0f, (float)this->width(), (float)this->height(), 0.0f, 0.0f, 100.0f );
Scale = glm::scale(glm::mat4(zoom), glm::vec3(zoom));

initializeOpenGLFunctions();

// Set OpenGL options
glEnable (GL_COLOR_LOGIC_OP);
glLogicOp (GL_XOR);

err = Shader.addShaderFromSourceFile( QOpenGLShader::Vertex, ":/vertex.glsl" );
err = Shader.addShaderFromSourceFile( QOpenGLShader::Fragment, ":/fragment.glsl" );
err = Shader.link();

if ( !glFormat.sampleBuffers() )
qWarning() << "Could not enable sample buffers";

// Set the clear color to black
//glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

load_fonts();

//VAO.create();
//VAO.bind();
mTrianglesVBO.create();
mTrianglesVBO.setUsagePattern( QGLBuffer::StaticDraw );
//if ( !mTrianglesVBO.bind() )
{
//qWarning() << "Could not bind vertex buffer to the context";
//return;
}
//mTrianglesVBO.allocate( vertices, sizeof(vertices) );
mTrianglesVBO.allocate( 3 * 4 * TRIANGLE_ARRAY_SIZE * sizeof(float));

mVBO.create();
mVBO.setUsagePattern( QGLBuffer::StaticDraw );
mVBO.bind();
if ( !mVBO.bind() )
{
qWarning() << "Could not bind vertex buffer2 to the context";
return;
}

mVBO.allocate( 3 * 4 * TRIANGLE_ARRAY_SIZE * sizeof(float));

mCrossHairsVBO.create();
mCrossHairsVBO.setUsagePattern( QGLBuffer::StaticDraw );
mCrossHairsVBO.bind();
if ( !mCrossHairsVBO.bind() )
{
qWarning() << "Could not bind vertex buffer3 to the context";
return;
}

mCrossHairsVBO.allocate( 3 * 4 * TRIANGLE_ARRAY_SIZE * sizeof(float));

// Bind the shader program so that we can associate variables from
// our application to the shaders
if ( !Shader.bind() )
{
qWarning() << "Could not bind shader program to context";
return;
}
Translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) );

translation = glGetUniformLocation( Shader.programId(), "Translation");
glUniformMatrix4fv( translation, 1, GL_FALSE, glm::value_ptr(Translation) );

model = glGetUniformLocation( Shader.programId(), "Model");
glUniformMatrix4fv( model, 1, GL_FALSE, glm::value_ptr(Model));

view = glGetUniformLocation( Shader.programId(), "View" );
glUniformMatrix4fv( view, 1, GL_FALSE, glm::value_ptr(View));

projection = glGetUniformLocation( Shader.programId(), "Projection" );
glUniformMatrix4fv( projection, 1, GL_FALSE, glm::value_ptr(Projection));

scale = glGetUniformLocation( Shader.programId(), "Scale" );
glUniformMatrix4fv( scale, 1, GL_FALSE, glm::value_ptr(Scale));


color = glm::vec4(1.0f, 0.5f, 1.2f,0 );
colorIdx = glGetUniformLocation( Shader.programId(), "Color" );
glUniform4f( colorIdx, 1.0f, 0.5f, 1.2f, 0 );

// Enable the "vertex" attribute to bind it to our currently bound
// vertex buffer.
Shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
Shader.enableAttributeArray( "vertex" );
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
glEnableVertexAttribArray(0);


glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);

Shader.setPatchVertexCount(10);
//VAO.release();

//Shader.enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);

BGLint = true;
}
/** @brief Instantiates a new HID object and registers it
 *         with the PCB framework.
 *
 *  @param Void.
 *  @return Void.
 */
void QtHID::init_interface( HID* intf )
{
intf->struct_size                  = sizeof (HID);
intf->name                         = "qt";
intf->description                  = "Qt Interface";
intf->gui                          = 1;
intf->poly_after                   = 1;

intf->parse_arguments              = parse_arguments;
intf->logv                         = logger;
intf->set_layer                    = stub_set_layer;
intf->end_layer                    = stub_end_layer;
intf->add_timer                    = add_timer;
intf->shift_is_pressed             = shift_is_pressed;
intf->control_is_pressed           = ctrl_is_pressed;
intf->invalidate_lr                = invalidate_lr;
intf->invalidate_all               = invalidate_all;
intf->graphics                     = &QtGraphics;
intf->graphics->make_gc            = make_gc;
intf->graphics->destroy_gc         = destory_gc;
intf->graphics->set_color          = set_color;
intf->graphics->set_line_cap       = set_line_cap;
intf->graphics->draw_line          = draw_a_line;
intf->graphics->draw_rect          = draw_a_rect;
intf->graphics->draw_arc           = intf_draw_arc;
intf->graphics->draw_pcb_text      = draw_text;
intf->graphics->fill_circle        = intf_fill_circle;
intf->graphics->fill_polygon       = intf_fill_polygon;
intf->graphics->fill_rect          = intf_fill_rect;
intf->notify_crosshair_change      = update_widget;
intf->set_crosshair                = set_crosshair;
intf->graphics->set_draw_xor       = set_draw_xor;
intf->graphics->set_line_width     = set_line_width;

/* Setup some common draw helper functions */
common_draw_helpers_init( intf->graphics );

/* Store pointer to the HID object */
QtHID::pIntf = intf;

}

void QtHID::intf_fill_rect(hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2)
{
	//Qhid_fill_rect( x1, y1, x2, y2); //Can't call this!!
	QtHID::pMe->ADD_triangle ( x1, y1, x1, y2, x2, y2);
	QtHID::pMe->ADD_triangle ( x2, y1, x2, y2, x1, y1);
}

void QtHID::intf_fill_polygon(hidGC gc, int n_coords, Coord *x, Coord *y)
{
QtHID::set_color( gc, gc->colorname );
Qhid_fill_polygon( n_coords, x, y, QtHID::pMe );
}

hidval QtHID::add_timer(void (*func) (hidval user_data_), unsigned long milliseconds_, hidval user_data_)
{
GuiTimer * Timer = new GuiTimer;
hidval    ret;

Timer->Callback_func = func;
Timer->user_data = user_data_;
Timer->pTimerObj = new QTimer( Timer );

connect( Timer->pTimerObj, SIGNAL(timeout()), Timer, SLOT(timeout()) );
Timer->pTimerObj->start( (int)milliseconds_ );

ret.ptr = Timer;

return( ret );

}

void QtHID::intf_draw_arc(hidGC gc, Coord cx, Coord cy, Coord xradius, Coord yradius, Angle start_angle, Angle delta_angle)
{
QtHID::set_color( gc, gc->colorname );
QtHID::pMe->draw_arc( gc->width, cx, cy, xradius, yradius, start_angle, delta_angle, QtHID::pMe->ppx );
}
void QtHID::intf_fill_circle (hidGC gc, Coord cx, Coord cy, Coord radius )
{
QtHID::set_color( gc, gc->colorname );
QtHID::pMe->fill_circle( cx, cy, radius, 1 );
}

void QtHID::invalidate_lr( int left_, int right_, int top_, int bottom_ )
{
QRect region(left_, top_, right_ - left_, bottom_ - top_ );
QtHID::pMe->update( region );
}

void QtHID::invalidate_all( void )
{
QtHID::pMe->update();
}

int QtHID::shift_is_pressed( void )
{
return( QtHID::pMe->shift_pressed );
}

int QtHID::ctrl_is_pressed( void )
{
return( QtHID::pMe->cntrl_pressed );
}

void QtHID::destory_gc( hidGC gc )
{
free( gc );
}

void QtHID::draw_a_rect( hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2 )
{
QtHID::set_color( gc, gc->colorname );
QtHID::pMe->draw_rect( x1, y1, x2, y2 );
}

void QtHID::draw_rect( Coord x1, Coord y1, Coord x2, Coord y2 )
{
glBegin (GL_LINE_LOOP);
glVertex3f (x1, y1, 1);
glVertex3f (x1, y2, 1);
glVertex3f (x2, y2, 1);
glVertex3f (x2, y1, 1);
glEnd ();
}

void QtHID::stub_end_layer( void )
{
//TODO
}

int QtHID::stub_set_layer( const char *name_, int group_, int _empty )
{
return( true );
//TODO
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
double r,g,b,a;

gc->colorname = name;

if (strcmp (gc->colorname, "drill") == 0)
    {
    r = QtHID::pMe->qport.Offlimit_color->redF();
    g = QtHID::pMe->qport.Offlimit_color->greenF();
    b = QtHID::pMe->qport.Offlimit_color->blueF();
    a = 0.85;
    }
else if (strcmp (gc->colorname, "erase") == 0)
    {
    r = QtHID::pMe->qport.Background_color->redF();
    g = QtHID::pMe->qport.Background_color->greenF();
    b = QtHID::pMe->qport.Background_color->blueF();
    a = 1.0;
    }
else
    {
    QColor   Color( gc->colorname );
    Color.getRgbF( &r, &g, &b, &a );
    }

QtHID::pMe->flush_triangles(&QtHID::pMe->triangleVertxBuffer);
QtHID::pMe->setColors( r, g, b, a );

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
QtHID::set_color( gc, gc->colorname );
QtHID::pMe->draw_line( gc->cap, gc->width, x1, y1, x2, y2, QtHID::pMe->ppx );
}

void QtHID::draw_text(hidGC gc, TextType * text, Coord silkLine )
{
QtHID::pMe->draw_glyph( gc, text );
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
QtHID::pMe->setXOR( gc, xor_ );
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
if( !check_triangle_space (&triangleVertxBuffer, slices) )
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
    add_triangle (&triangleVertxBuffer, last_capx, last_capy, capx, capy, x, y);

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

void QtHID::fill_circle (Coord vx, Coord vy, Coord vr, double scale)
{
#define MIN_TRIANGLES_PER_CIRCLE 6
#define MAX_TRIANGLES_PER_CIRCLE 360
  float last_x, last_y;
  float radius = vr;
  int slices;
  int i;

  slices = calc_slices (vr / scale, 2 * M_PI);

  if (slices < MIN_TRIANGLES_PER_CIRCLE)
    slices = MIN_TRIANGLES_PER_CIRCLE;

  if (slices > MAX_TRIANGLES_PER_CIRCLE)
    slices = MAX_TRIANGLES_PER_CIRCLE;

if( !check_triangle_space (&triangleVertxBuffer, slices) )
  {
  /* Very bad error */
  exit( 1 );
  }

  last_x = vx + vr;
  last_y = vy;

  for (i = 0; i < slices; i++) {
    float x, y;
    x = radius * cosf (((float)(i + 1)) * 2. * M_PI / (float)slices) + vx;
    y = radius * sinf (((float)(i + 1)) * 2. * M_PI / (float)slices) + vy;
    add_triangle (&triangleVertxBuffer, vx, vy, last_x, last_y, x, y);
    last_x = x;
    last_y = y;
  }
}

void QtHID::draw_glyph( hidGC gc, TextType * text )
{

fontmap_type letter;
int          c;
char *       p = text->TextString;
glm::vec4    Point[3];
FT_Pos       spacing = 0;


while( p!=NULL && *p )
    {
	letter = fonts[ *p ];

    for( c = 0; c < letter.index; c+=9 )
        {
    	Point[0] = glm::translate(glm::mat4(1.0f), glm::vec3(text->X + spacing, text->Y, 0) ) * glm::vec4( letter.vertices[c], letter.vertices[c+1], letter.vertices[c+2], 1.0f );
    	Point[1] = glm::translate(glm::mat4(1.0f), glm::vec3(text->X + spacing, text->Y, 0) ) * glm::vec4( letter.vertices[c+3], letter.vertices[c+4], letter.vertices[c+5], 1.0f );
    	Point[2] = glm::translate(glm::mat4(1.0f), glm::vec3(text->X + spacing, text->Y, 0) ) * glm::vec4( letter.vertices[c+6], letter.vertices[c+7], letter.vertices[c+8], 1.0f );

    	this->add_triangle( &triangleVertxBuffer, Point[0].x, Point[0].y, Point[1].x, Point[1].y, Point[2].x, Point[2].y );
        }

    spacing += letter.advance;
    p++;
    }
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
    float       scale            // Current scale factor
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

start_angle = 0;

printf("draw arc: %d,%d\n", x, y );

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
if( !check_triangle_space (&triangleVertxBuffer, 2 * slices) )
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
    add_triangle ( &triangleVertxBuffer,
                   last_inner_x, last_inner_y,
                   last_outer_x, last_outer_y,
                   outer_x, outer_y
                 );

    /* Add upper triangle */
    add_triangle ( &triangleVertxBuffer,
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
	if( BGLint )
	    {

		float vertices[] = {
		     10000.5f, 110.f, 0.0f,
		     10000.5f,    1000000.5f, 0.0f,
		     1000000.0f,  1000000.5f, 0.0f
		};

		if( buffer->coord_comp_count > 0)
		{
			printf("DALE - %d\n", buffer->coord_comp_count );
		}

        mTrianglesVBO.write( 0, buffer->triangle_array, buffer->coord_comp_count * sizeof(float) );
		glDrawArrays(GL_TRIANGLES, 0,  buffer->triangle_count * 3 );

		/* Clean up the buffer tracking variables */
		buffer->triangle_count = 0;
		buffer->coord_comp_count = 0;

	    }
} /* flush_triangles() */

void QtHID::setColors( double r, double b, double g, double a )
{
	if( BGLint )
	    {
	    glUniform4f( colorIdx, r, g, b, a );
	    }
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
      this->add_triangle(&triangleVertxBuffer, x1 - wdx, y1 - wdy,
                                   x2 - wdx, y2 - wdy,
                                   x2 + wdx, y2 + wdy);
      add_triangle (&triangleVertxBuffer, x1 - wdx, y1 - wdy,
                                   x2 + wdx, y2 + wdy,
                                   x1 + wdx, y1 + wdy);

    /* Don't bother capping hairlines */
    if (circular_caps && !hairline)        {
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
  //buffer->triangle_array [buffer->coord_comp_count++] = 1;
  buffer->triangle_array [buffer->coord_comp_count++] = x2;
  buffer->triangle_array [buffer->coord_comp_count++] = y2;
  buffer->triangle_array [buffer->coord_comp_count++] = z2;
  //buffer->triangle_array [buffer->coord_comp_count++] = 1;
  buffer->triangle_array [buffer->coord_comp_count++] = x3;
  buffer->triangle_array [buffer->coord_comp_count++] = y3;
  buffer->triangle_array [buffer->coord_comp_count++] = z3;
  //buffer->triangle_array [buffer->coord_comp_count++] = 1;
  buffer->triangle_count++;
}
void QtHID::mouseMoveEvent ( QMouseEvent * e )
{
bool moved;
Coord x, y;

/* Grab any key modifiers */
shift_pressed = ( e->modifiers() & Qt::ShiftModifier ) ? TRUE : FALSE;
cntrl_pressed = ( e->modifiers() & Qt::ShiftModifier ) ? TRUE : FALSE;

/* Grab new mouse coordinates */
x = (Coord)( ( e->x() - tx ) / zoom ) ;
y = (Coord)( ( e->y() - ty ) / zoom );

SetCrosshairRange( x - 10000000, y-10000000, PCB->MaxWidth, PCB->MaxHeight );


/* Notify framework of change */
AdjustAttachedObjects();
moved = MoveCrosshairAbsolute(x, y);
gui->notify_crosshair_change(true);

} /* mouseMoveEvent() */

void QtHID::wheelEvent ( QWheelEvent * e )
{
float Glx;
float Gly;

#define    ZOOM_PER_WHEEL_CLICK        ( zoom * 0.1 )
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
this->update();
} /* wheelEvent() */

void QtHID::mousePressEvent( QMouseEvent* e )
{
printf("Button %d\n", e->button() );

do_mouse_action( e->button(), 0 );
}

void QtHID::keyPressEvent( QKeyEvent *e )
{
QPoint *    point;

point = NULL;

QString myChar = e->text();
do_key_action( myChar.toUtf8()[0] );

switch( e->key() )
    {
    case Qt::Key_Escape:
        hid_parse_actions("Mode(Escape)");
        break;

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

    case Qt::Key_Insert:
    	hid_parse_actions("LoadFrom(ElementToBuffer,'./newlib/2_pin_thru-hole_packages/1W_Carbon_Resistor')");
    	break;

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
char buf[ 100 ];
int  scale, translation;

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

Projection = glm::ortho( 0.0f, (float)this->width(), (float)this->height(), 0.0f, 0.0f, 100.0f );
Scale = glm::scale(glm::mat4(zoom), glm::vec3(zoom));

glUniformMatrix4fv( glGetUniformLocation( Shader.programId(), "Projection" ), 1, GL_FALSE, glm::value_ptr(Projection));

//DrawMark();
//flush_triangles(&buffer);


/* Scale and move the point under the cursur back to the origin spot */
Translation = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0) );

translation = glGetUniformLocation( Shader.programId(), "Translation");
scale       = glGetUniformLocation( Shader.programId(), "Scale" );

glUniformMatrix4fv( translation, 1, GL_FALSE, glm::value_ptr(Translation) );
glUniformMatrix4fv( scale, 1, GL_FALSE, glm::value_ptr(Scale));


/* Calculate the viewable region */
region.X1 = -tx * ppx;
region.Y1 = -ty * ppx;
region.X2 = ( this->width() - tx ) * ppx;
region.Y2 = ( this->height() -ty ) * ppx;

/* Draw anything attached to the crosshairs */
DrawAttached();
this->flush_triangles( &triangleVertxBuffer );


/* Draw all elements within the view */
hid_expose_callback( this->pIntf, &region, 0);
this->flush_triangles( &triangleVertxBuffer );

setColors( 1, 0, 0, 1 );
/* Draw the grid */
hidgl_draw_grid ( &region );

/* Draw the crosshairs */
Drawcrosshairs();
} /* paintGL() */

void QtHID::Drawcrosshairs(void)
{
GLfloat x, y, z;
float Glx, Gly;

x = qport.x;
y = qport.y;
z = 0.0;

static GLfloat points[12];

Glx = ( tx ) / zoom ;
Gly = ( ty ) / zoom ;

printf("Glx: %f  and Gly: %f \n", tx, ty );

points[0] = x;
points[1] = PCB->MaxHeight;
points[2] = z;

points[3] = x;
points[4] = 0.0f - Gly;
points[5] = z;

points[6] = 0.0f - Glx;
points[7] = y;
points[8] = z;

points[9] = PCB->MaxWidth;
points[10] = y;
points[11] = z;


//glEnable (GL_COLOR_LOGIC_OP);
//glLogicOp (GL_XOR);

//glColor3f (cross_color.red / 65535.,
//          cross_color.green / 65535.,
//          cross_color.blue / 65535.);
//glViewport (0, 0, 955, 710);


//glColor3f( 1.0f, 1.0f, 1.0f );
//glMatrixMode (GL_PROJECTION);
//glLoadIdentity ();
//
//glOrtho (0, this->width(), this->height(), 0, 0, 100);
//
//glMatrixMode (GL_MODELVIEW);
//glTranslatef (0.0f, 0.0f, -3);
//glLoadIdentity ();
//
//glBegin (GL_LINES);
//
//draw_right_cross (x, y, z);
//
//if (Crosshair.shape == Union_Jack_Crosshair_Shape)
//  draw_slanted_cross (x, y, z);
//if (Crosshair.shape == Dozen_Crosshair_Shape)
//  draw_dozen_cross (x, y, z);
//
//glEnd ();

//glDisable (GL_COLOR_LOGIC_OP);

mCrossHairsVBO.write(0, points, 12 * sizeof (GLfloat) );
glDrawArrays(GL_LINES, 0, 4 );

}


void QtHID::setcrosshairs( int x, int y )
{
/* Convert the OpenGL point to a qt point */
//qport.x = ( x * zoom ) + tx;
//qport.y = ( y * zoom ) + ty;
qport.x = x;
qport.y = y;
printf("setcrosshairs(): %d, %d\n", x, y );
}

void QtHID::setXOR( hidGC gc, int xor_ )
{
if( !BGLint )
	{
	return;
	}

if( xor_)
    {
	glEnable (GL_COLOR_LOGIC_OP);
	glLogicOp (GL_XOR);
    }
else
    {
	glDisable (GL_COLOR_LOGIC_OP);
    }
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
  GLfloat *points = 0;
  int w, h;
  Coord x1, y1, x2, y2, n;
  double x, y;

 // if (!Settings.DrawGrid)
 //   return;

  if( 15 > ( PCB->Grid * zoom ) )
      {
	  // Too many points to be useful, don't draw grid
	  return;
      }

  glEnable (GL_COLOR_LOGIC_OP);
  glLogicOp (GL_XOR);
  glColor3f( 1.0f, 1.0f, 1.0f );

  x1 = GridFit( drawn_area->X1, PCB->Grid, PCB->GridOffsetX);
  y1 = GridFit( drawn_area->Y1, PCB->Grid, PCB->GridOffsetY);


  x2 = drawn_area->X2;
  y2 = drawn_area->Y2;

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


  w = (int) ((x2 - x1) / PCB->Grid + 0.5) + 1;
  h = (int) ((y2-y1) / PCB->Grid + 0.5) + 1;

  points = (GLfloat*)malloc(w*h*3*sizeof(GLfloat) );

  n = 0;

  //Loop over each row
  for (y = y1; y <= y2; y += PCB->Grid)
    {
	 //Loop over each column in this row iteration
	 for (x = x1; x <= x2; x += PCB->Grid)
	    {
	      points[3 * n + 0] = (GLfloat)x;  // x changes for each column
	      points[3 * n + 1] = (GLfloat)y;  // y is constant for this row
	      points[3 * n + 2] = 0;  // z is always zero
	      n++;
	    }
    }
    mVBO.write( 0, points, n * 3 * sizeof (GLfloat) );
	glDrawArrays(GL_POINTS, 0, n );

	glDisable (GL_COLOR_LOGIC_OP);
	free(points);
}

Coord QtHID::GridFit (Coord x, Coord grid_spacing, Coord grid_offset)
{
  x -= grid_offset;
  x = grid_spacing * round ((double) x / grid_spacing);
  x += grid_offset;
  return x;
}

void QtHID::ShowContextMenu( const QPoint &pos )
{
QMenu contextMenu;
QAction * pA;
int x;

for( x = 0; x < max_copper_layer; x++ )
	{
	pA = new QAction( PCB->Data->Layer[x].Name, this );
	contextMenu.addAction( pA );
	}

contextMenu.exec(mapToGlobal(pos));
}

int QtHID::move_to( const FT_Vector* to, void* user )
{
	fontmap_type * map = (fontmap_type*)user;

	if( map->contour_open )
	    {
		gluTessEndContour( map->tess );
	    }

	gluTessBeginContour( map->tess );
	map->contour_open = true;

	/* We consider a move_to to be a new contour */
	map->contours[ map->contour_cnt ] = map->index;
	map->contour_cnt++;
	map->from = *to;
    return(0);
}

/**
 * @brief Line To
 *
 * Callback for Freetypes when decomposing a font. This function
 * is used to draw Linear Bézier curves
 *
 * \to    (x,y) position for the new point, an offset from last point
 * \user  pointer to user data which in this case is to our local
 *        font_map type. Used to get the from location
 *
 * \return returns 0 if the decomposition succeeds.
 */
int QtHID::line_to( const FT_Vector* to, void* user )
{
	fontmap_type * map = (fontmap_type*)user;
	FT_Vector first_point;
	glm::vec4 rFirstPoint;
	glm::vec4 rCurvePoint;
	double t;
	float x,y;

	t = 0.0f;

	// Get the starting position and scale it for the zoom level
	first_point.x = to->x * map->zoom;
	first_point.y = to->y * map->zoom;

	// Translate back to the origin and rotate it to the correct orientation, then return to the original position
	rFirstPoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( first_point.x, first_point.y, 0.0f, 1.0f );

	//linear interpolation for a linear Bézier curve
	for( t = 0.0f; t < 1.0f; t+=0.1f )
	    {
		// Get equal spaced points between start and end points
		// Algebraically rearranged from x = (Xf-Xs)*p + Xs but no need to check for underflow
		// See https://en.wikipedia.org/wiki/B%C3%A9zier_curve
		x = ( ( map->from.x * (1-t) ) + ( t * to->x ) ) * map->zoom;
		y = ( ( map->from.y * (1-t) ) + ( t * to->y ) ) * map->zoom;

		// Rotate each point to the correct orientation
	    rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

	    // Save each point (along with start and end ) in dynamic memory array
	    map->v_ = (double*)malloc( sizeof(double) * 3 );
	    map->v_[0] = (double)rCurvePoint.x;
	    map->v_[1] = (double)rCurvePoint.y;
	    map->v_[2] = (double)0;

	    // Add the point to our polygon contour for tessellation
		gluTessVertex( map->tess, map->v_, map->v_ );

		// Will save the last point as the new first point
		// TODO: Probably only need to do this once.
		rFirstPoint = rCurvePoint;
	    }

    // Get the last point on the line
	// TODO: Can't this be done inside the loop!
	t = 1;
	x = ( map->from.x * (1-t) + t * to->x ) * map->zoom;
	y = ( map->from.y * (1-t) + t * to->y ) * map->zoom;

	rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

	map->v_ = (double*)malloc( sizeof(double) * 3 );

	map->v_[0] = (double)rCurvePoint.x;
	map->v_[1] = (double)rCurvePoint.y;
	map->v_[2] = 0;

	gluTessVertex( map->tess, map->v_, map->v_ );

	map->from = *to;

	//Return Success always!
    return(0);
}

/**
 * @brief Line To
 *
 * Callback for Freetypes when decomposing a font. This function
 * is used to draw 2nd order Bézier curves
 *
 * \control   control point for Bezier curve
 * \to        (x,y) position for the new point, an offset from last point
 * \user      pointer to user data which in this case is to our local
 *            font_map type. Used to get the from location
 *
 * \return returns 0 if the decomposition succeeds.
 */
int QtHID::conic_to( const FT_Vector* control, const FT_Vector* to, void* user )
{
	fontmap_type * map = (fontmap_type*)user;
	float t;
	float x, y;
	float fx, fy;
	glm::vec4 rFirstPoint;
	glm::vec4 rCurvePoint;

	// Get the starting point scaled correctly
	fx = to->x * map->zoom;
    fy = to->y * map->zoom;

    // First the first true point rotated to the correct orientation
    rFirstPoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( fx, fy, 0.0f, 1.0f );

    // Linear interpolation of the 2nd order Bézier curve
	for( t = 0; t < 1.0f; t+=0.1f )
	    {
		x = map->zoom * ( map->from.x * pow( (1-t),2 ) + control->x * 2 * (1-t) * t + to->x * pow( t,2 ) );
		y = map->zoom * ( map->from.y * pow( (1-t),2 ) + control->y * 2 * (1-t) * t + to->y * pow( t,2 ) );

        // for each point rotate it to the correct orientation
		rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

		// Save each point (along with start and end ) in dynamic memory array
		map->v_ = (double*)malloc( sizeof(double) * 3 );
		map->v_[0] = (double)rCurvePoint.x;
		map->v_[1] = (double)rCurvePoint.y;
		map->v_[2] = 0;

		// Add the point to our polygon contour for tessellation
		gluTessVertex( map->tess, map->v_, map->v_ );

	    // Get the last point on the line
		// TODO: Can't this be done inside the loop!
		rFirstPoint = rCurvePoint;
	    }

    // Get the last point on the line
	// TODO: Can't this be done inside the loop!
	t = 1;
	x = map->zoom * ( map->from.x * pow( (1-t),2 ) + control->x * 2 * (1-t) * t + to->x * pow( t,2 ) );
	y = map->zoom * ( map->from.y * pow( (1-t),2 ) + control->y * 2 * (1-t) * t + to->y * pow( t,2 ) );

	rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

	map->v_ = (double*)malloc( sizeof(double) * 3 );

	map->v_[0] = (double)rCurvePoint.x;
	map->v_[1] = (double)rCurvePoint.y;
	map->v_[2] = 0;

	gluTessVertex( map->tess, map->v_, map->v_ );

	map->from = *to;

	// return success
    return(0);
}

int QtHID::cubic_to( const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user )
{

	fontmap_type * map = (fontmap_type*)user;
	float t;
	float x, y;
	float fx, fy;
	glm::vec4 rFirstPoint;
	glm::vec4 rCurvePoint;

	// Get the starting point scaled correctly
	fx = to->x * map->zoom;
    fy = to->y * map->zoom;

    // First the first true point rotated to the correct orientation
    rFirstPoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( fx, fy, 0.0f, 1.0f );

    // Linear interpolation of the 2nd order Bézier curve
	for( t = 0; t < 1.0f; t+=0.1f )
	    {
		x = map->from.x * pow( (1-t), 3 ) + control1->x * 3 * pow( (1-t), 2 ) * t + control2->x * 3 * (1-t) * pow( t, 2 ) + to->x * pow( t,3 );
		y = map->from.y * pow( (1-t), 3 ) + control1->y * 3 * pow( (1-t), 2 ) * t + control2->y * 3 * (1-t) * pow( t, 2 ) + to->y * pow( t,3 );

        // for each point rotate it to the correct orientation
		rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

		// Save each point (along with start and end ) in dynamic memory array
		map->v_ = (double*)malloc( sizeof(double) * 3 );
		map->v_[0] = (double)rCurvePoint.x;
		map->v_[1] = (double)rCurvePoint.y;
		map->v_[2] = 0;

		// Add the point to our polygon contour for tessellation
		gluTessVertex( map->tess, map->v_, map->v_ );

	    // Get the last point on the line
		// TODO: Can't this be done inside the loop!
		rFirstPoint = rCurvePoint;
	    }

    // Get the last point on the line
	// TODO: Can't this be done inside the loop!
	t = 1;
	x = map->from.x * pow( (1-t), 3 ) + control1->x * 3 * pow( (1-t), 2 ) * t + control2->x * 3 * (1-t) * pow( t, 2 ) + to->x * pow( t,3 );
	y = map->from.y * pow( (1-t), 3 ) + control1->y * 3 * pow( (1-t), 2 ) * t + control2->y * 3 * (1-t) * pow( t, 2 ) + to->y * pow( t,3 );

	rCurvePoint = map->MtranslateBack * map->Mrotate * map->Mtranslate * glm::vec4( x, y, 0.0f, 1.0f );

	map->v_ = (double*)malloc( sizeof(double) * 3 );

	map->v_[0] = (double)rCurvePoint.x;
	map->v_[1] = (double)rCurvePoint.y;
	map->v_[2] = 0;

	gluTessVertex( map->tess, map->v_, map->v_ );

	map->from = *to;

	// return success
    return(0);
}

/**
 * @brief Load Font data
 *
 * Load's Font pixel data into memory. The memory is the Qmap font varaible
 * for this class.
 */
void QtHID::load_fonts( void )
{
FT_Library ft;
static FT_Face    face;
FT_Outline_Funcs  functions;
fontmap_type      fontmap;
FT_Glyph_Metrics  metrics;
FT_Vector         center;
int               err;
glm::vec3         rotation;
glm::quat         MyQuaternion;

/* Init FreeTypes */
err = FT_Init_FreeType(&ft);

/* Load a font file */
//err = FT_New_Face( ft, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0, &face );
err = FT_New_Face( ft, "/usr/share/fonts/truetype/OCRA.ttf", 0, &face );
//err = FT_New_Face( ft, "/usr/share/fonts/truetype/fonts-guru-extra/Saab.ttf", 0, &face );

/* Set the Pixel size for the font loaded */
FT_Set_Pixel_Sizes(face, 0, 128);

functions.move_to = move_to;
functions.line_to = line_to;
functions.conic_to = conic_to;
functions.cubic_to = cubic_to;

functions.shift = 0;
functions.delta = 0;


// Disable byte-alignment restriction
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

for( GLubyte c = 0; c < 128; c++ )
    {
	err = FT_Load_Char( face, c, FT_LOAD_RENDER);
	if( err )
	    {
        printf("ERROR::FREETYPE: Failed to load Glyph");
        continue;
	    }

	memset( &fontmap, 0, sizeof(fontmap_type) );
	/* How do we calculate the size of vertices needed? */
	fontmap.vertices = (float*)malloc( 8000 * sizeof( float) );
	fontmap.zoom = this->zoom;
	fontmap.zoom = 30;
	metrics = face->glyph->metrics;
	center.x = ( ( metrics.horiBearingX + metrics.width ) /2 ) * fontmap.zoom;
	center.y = 0;//( ( metrics.height ) /2 ) * fontmap.zoom;
	fontmap.Mrotate = glm::rotate( glm::mat4(1.0f), glm::radians(180.f), glm::vec3(1.0f, 0.0f, 0.0f) );
	fontmap.Mtranslate = glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, 0) );
	fontmap.MtranslateBack = glm::translate(glm::mat4(1.0f), glm::vec3(center.x, center.y, 0) );
	fontmap.contour_open = false;
	fontmap.advance = metrics.horiAdvance * fontmap.zoom;

	fontmap.types_idx = 0;
	memset( fontmap.types, 0, sizeof(fontmap.types) );
	fontmap.fake_index = 0;

	fontmap.tess = gluNewTess();
	gluTessProperty( fontmap.tess, GLU_TESS_WINDING_RULE,
	                   GLU_TESS_WINDING_POSITIVE);

    gluTessCallback( fontmap.tess, GLU_TESS_VERTEX_DATA, (GLUTessCallback)vertexCallback );
    gluTessCallback( fontmap.tess, GLU_TESS_BEGIN_DATA, (GLUTessCallback)beginCallback );
    gluTessCallback( fontmap.tess, GLU_TESS_END_DATA, (GLUTessCallback)endCallback );
    gluTessCallback( fontmap.tess, GLU_TESS_COMBINE_DATA, (GLUTessCallback)combineCallback );
    gluTessCallback( fontmap.tess, GLU_TESS_ERROR, (GLUTessCallback)errorCallback );


    gluTessBeginPolygon( fontmap.tess, &fontmap );

    // Decompose each font Glyph into bezier curves
	FT_Outline_Decompose( &face->glyph->outline, &functions, &fontmap );

	if( fontmap.contour_open )
	    {
		gluTessEndContour( fontmap.tess );
    	}

	gluTessEndPolygon( fontmap.tess );

	fontmap.contours[ fontmap.contour_cnt ] = fontmap.index;
	fontmap.contour_cnt++;

	fonts.insert( c, fontmap );
    }

}
void QtHID::errorCallback ( GLenum error_code )
{
printf( "%s", gluErrorString( error_code ) );
}
void QtHID::beginCallback ( GLenum which, fontmap_type* map )
{
map->types[map->types_idx].type = which;
map->types[map->types_idx].start_idx = map->index;
map->stashed_idx = 0;
map->stashed_vertices = 0;
printf( "begin() type: %d, index: %d\n", which, map->index );
}
void QtHID::endCallback ( fontmap_type* map )
{
map->types[map->types_idx].len = map->index - map->types[map->types_idx].start_idx;
map->types_idx++;
printf( "end() index: %d\n", map->index );
}
void QtHID::combineCallback ( GLdouble coords[3], GLdouble* vertex_data[4],
				 GLfloat weight[4], void** dataOut )
{
    (void)vertex_data;
    (void)weight;
    //    std::cerr << "called combine" << std::endl;
    double* v;

    v = (double*)malloc( sizeof(double) * 3);
    memcpy( v, coords, sizeof(double) *3 );

    *dataOut = v;
}
void QtHID::vertexCallback ( double* vertex, fontmap_type* map  )
{
	if( map->types[map->types_idx].type == GL_TRIANGLE_STRIP ||
		map->types[map->types_idx].type == GL_TRIANGLE_FAN)
	  {
	    if (map->stashed_vertices < 2)
	      {
	        map->stashed_coords[map->stashed_idx++] = vertex[0];
	        map->stashed_coords[map->stashed_idx++] = vertex[1];
	        map->stashed_coords[map->stashed_idx++] = vertex[2];
	        map->stashed_vertices ++;
	      }
	    else
	      {
	    	map->vertices[ map->index++ ] = map->stashed_coords[0];
	    	map->vertices[ map->index++ ] = map->stashed_coords[1];
	    	map->vertices[ map->index++ ] = map->stashed_coords[2];
	    	map->vertices[ map->index++ ] = map->stashed_coords[3];
	    	map->vertices[ map->index++ ] = map->stashed_coords[4];
	    	map->vertices[ map->index++ ] = map->stashed_coords[5];
	    	map->vertices[ map->index++ ] = vertex[0];
	    	map->vertices[ map->index++ ] = vertex[1];
	    	map->vertices[ map->index++ ] = vertex[2];

	        if (map->types[ map->types_idx].type == GL_TRIANGLE_STRIP)
	          {
	            /* STRIP saves the last two vertices for re-use in the next triangle */
	        	map->stashed_coords[0] = map->stashed_coords[3];
	        	map->stashed_coords[1] = map->stashed_coords[4];
	        	map->stashed_coords[2] = map->stashed_coords[5];
	          }

	        /* Both FAN and STRIP save the last vertex for re-use in the next triangle */
        	map->stashed_coords[3] = vertex[0];
        	map->stashed_coords[4] = vertex[1];
        	map->stashed_coords[5] = vertex[2];
	      }
	  }
	else if (map->types[ map->types_idx].type == GL_TRIANGLES)
	  {
	        map->stashed_idx = 0;
	        map->stashed_vertices = 0;
	        map->vertices[ map->index++ ] = vertex[0];
	        map->vertices[ map->index++ ] = vertex[1];
	        map->vertices[ map->index++ ] = vertex[2];
	  }
}

void QtHID::drawKicadFootPrint( kicadFootPrintDataType * data )
{
int i,b;
item_type * item;
item_type * citem;

for( i = 0; i<data->child_idx; i++ )
    {
    item = data->children[i];
    printf("Type is: %d\n", item->type);

    if( item->type == fpline )
        {
    	int start_x, start_y, end_x, end_y, width, line_flags;
    	line_flags |= CLEARLINEFLAG;

    	for( b=0; b<item->child_idx; b++)
    	    {
    		citem = item->children[b];
    		switch( citem->type )
    		    {
    		    case start:
    		    	KICAD_pos_type * pos;
    		    	pos = (KICAD_pos_type*)citem->data;

    		    	//convert from mm to opengl units
    		    	start_x = pos->x * 1000000;
    		    	start_y = pos->y * 1000000;
    		    	break;

    		    case end:
    		    	pos = (KICAD_pos_type*)citem->data;

    		    	//convert from mm to opengl units
    		    	end_x = pos->x * 1000000;
    		    	end_y = pos->y * 1000000;
    		    	break;

    		    case KICAD_TYPE_width:
    		    	width = *((float*)citem->data) * 1000000;
    		    	break;

    		    default:
    		    	break;
    		    }
    	    }

    	CreateDrawnLineOnLayer( CURRENT, start_x, start_y, end_x, end_y, width, 2 * Settings.Keepaway, MakeFlags (line_flags) );
        }
    else if( item->type == KICAD_TYPE_pad )
        {
    	KICAD_pad_type *  pad_type;
    	int x,y,thickness,drillSize, width, heigth;

    	pad_type = (KICAD_pad_type*)item->data;

    	if( pad_type->pad_type == PAD_TYPE_THRUHOLE )
    	    {
    		for( b=0; b<item->child_idx; b++ )
    		    {
    			citem = item->children[b];
    			switch( citem->type )
    		  	    {
    			    case at:
    			    	KICAD_pos_type * pos;
    			    	pos = (KICAD_pos_type*)citem->data;
    			    	x = pos->x * 1000000;
    			    	y = pos->y * 1000000;
    			    	break;

    			    case KICAD_TYPE_size:
    			    	pos = (KICAD_pos_type*)citem->data;
    			    	thickness = pos->x * 1000000;
    			    	break;

    			    case drill:
    			    	drillSize = *((float*)citem->data) * 1000000;
    			    	break;

    			    default:
    			    	break;
    			    }
    	    	}

    		if( pad_type->pad_shape == PAD_SHAPE_CIRCLE )
    		    {
    		    CreateNewVia( PCB->Data, x, y, thickness, 2* Settings.Keepaway, 0, drillSize, NULL, MakeFlags(0) );
    		    }
    		else if( pad_type->pad_shape == PAD_SHAPE_RECT )
    		    {
    			CreateNewVia( PCB->Data, x, y, thickness, 2* Settings.Keepaway, 0, drillSize, NULL, MakeFlags(SQUAREFLAG) );
    		    }
    	    }
    	else if( pad_type->pad_type == PAD_TYPE_SMD )
    	    {
    		for( b=0; b<item->child_idx; b++ )
				{
				citem = item->children[b];
				switch( citem->type )
					{
					case at:
						KICAD_pos_type * pos;
						pos = (KICAD_pos_type*)citem->data;
						x = pos->x * 1000000;
						y = pos->y * 1000000;
						break;

					case KICAD_TYPE_size:
						pos = (KICAD_pos_type*)citem->data;
						width = pos->x * 1000000;
						heigth = pos->y * 1000000;
						break;

					case drill:
						drillSize = *((float*)citem->data) * 1000000;
						break;

					default:
						break;
					}
				}
    		if( pad_type->pad_shape == PAD_SHAPE_RECT )
    		    {
    			PolygonType * polygon;

    			polygon = CreateNewPolygonFromRectangle( CURRENT, x - (width/2), y - (heigth/2), x + (width/2), y + (heigth/2), MakeFlags(0) );
    			InitClip (PCB->Data, CURRENT, polygon);
    		    }
    	    }
        }
    }
}

#include "qthid.moc"
