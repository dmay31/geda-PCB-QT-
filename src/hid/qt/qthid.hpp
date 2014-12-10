/*
 * qthid.h
 *
 *  Created on: Nov 16, 2012
 *      Author: dale
 */

#ifndef QTHID_H_
#define QTHID_H_

#include "global.h"
#include "data.h"
#include "crosshair.h"
#include "error.h"
#include "hid_draw.h"
#include "hid/common/hid_resource.h"
#include "hid/common/draw_helpers.h"
#include "gui.h"
#include "set.h"
#include "action.h"
#include "misc.h"
#include "create.h"
#include <QApplication>
#include <QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QGLPixelBuffer>
#include <QColor>
#include <QtCore>
#include <QTimer>
#include <QObject>
#include <QMenu>
#include <QOpenGLWidget>
#include <QGLPixelBuffer>
#include <QMap>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QGLWidget>
#include <QGLBuffer>

#include "gl_helpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

#include "freetype/ftoutln.h"
#include "kicadImport.h"

/*-----------------------------------------------
            PreCompiler Defines
 ----------------------------------------------*/
#define TRIANGLE_ARRAY_SIZE 15461

/*-----------------------------------------------
         Types ( For C interface )
 ----------------------------------------------*/
typedef struct hid_gc_struct
{
  HID *me_pointer;
  const char *colorname;
  double alpha_mult;
  Coord width;
  int cap, join;
};

typedef struct
    {
    float triangle_array [3 * 4 * TRIANGLE_ARRAY_SIZE];
    unsigned int triangle_count;
    unsigned int coord_comp_count;
    } triangle_buffer;

typedef void (*GLUTessCallback)();

/*-----------------------------------------------
              Class Definitions
 ----------------------------------------------*/
class Geometry;
class Cube;

class GuiTimer : public QObject
{
Q_OBJECT

public:
    GuiTimer();
    ~GuiTimer();

    void (*Callback_func) (hidval user_data_);
    hidval user_data;
    QTimer*  pTimerObj;

public slots:
    void timeout();

};

class DrawPort
{
public:
  int x;
  int y;
  QColor *   Offlimit_color;
  QColor *   Background_color;

};

class QtHID : public QGLWidget, protected QOpenGLFunctions
{
Q_OBJECT

public:
  /* Constructor(s) */
   QtHID(QWidget *Parent = 0);

   /* Destructor */
  ~QtHID();
  static QtHID*           pMe;
  static HID*             pIntf;

  /* Other Public Functions */
  void                 init_interface        ( HID* intf );
  void                 ADD_triangle          ( float x1, float y1, float x2, float y2, float x3, float y3 );



  void setcrosshairs( int x, int y );
  void setXOR( hidGC gc, int xor_ );

  void drawKicadFootPrint( kicadFootPrintDataType * data );

private:
  typedef struct
      {
	  GLint     type;
	  int       start_idx;
	  int       len;
      } array_info_type;

  typedef struct
      {
      FT_Vector from;
      FT_Pos    advance;
      float *   vertices;
      double *  v_;
      int       index;
      int       fake_index;
      float     zoom;
      GLUtesselator * tess;
      bool      contour_open;
      int       contours[10];
      int       contour_cnt;
      glm::mat4 Mtranslate;
      glm::mat4 MtranslateBack;
      glm::mat4 Mrotate;
      array_info_type   types[200];
      GLfloat   stashed_coords[2 * 3];
      int       stashed_idx;
      int       stashed_vertices;
      int               types_idx;
      }fontmap_type;

    /* Static Private Variables */
    static HID_DRAW QtGraphics;


    /* Private Variables */
    triangle_buffer     triangleVertxBuffer;
    QColor              qtGreen;
    DrawPort            qport;
    Geometry *          geom;
    Cube *              cube;
    QGLPixelBuffer *    pbuffer;
    //QOpenGLFunctions    GLFunctions;
    float               zoom;
    float               ppx;
    float               tx;
    float               ty;
    bool                shift_pressed;
    bool                cntrl_pressed;


    /* Static Private Member Functions */
    static hidGC    make_gc            ( void );
    static void     parse_arguments    ( int *argc, char ***argv );
    static void     logger             ( const char *fmt, va_list args );
    static void     set_color          ( hidGC gc, const char *name );
    static void     set_line_cap       ( hidGC gc, EndCapStyle style );
    static void     draw_a_line        ( hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2 );
    static void     draw_text          (hidGC gc, TextType *, Coord);
    static void     update_widget      ( bool change_complete );
    static void     set_crosshair      ( int x, int y, int action );
    static void     set_draw_xor       ( hidGC gc, int xor_ );
    static void     set_line_width     ( hidGC gc, Coord width );
    static int      stub_set_layer     (const char *name_, int group_, int _empty);
    static void     stub_end_layer     ( void );
    static void     draw_a_rect        ( hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2 );
    static void     destory_gc         ( hidGC gc );
    static int      shift_is_pressed   ( void );
    static int      ctrl_is_pressed    ( void );
    static void     invalidate_lr      ( int left_, int right_, int top_, int bottom_ );
    static void     invalidate_all     ( void );
    static void     intf_fill_circle   ( hidGC gc, Coord cx, Coord cy, Coord radius );
    static void     intf_draw_arc      ( hidGC gc, Coord cx, Coord cy, Coord xradius, Coord yradius, Angle start_angle, Angle delta_angle );
    static void     intf_fill_polygon  ( hidGC gc, int n_coords, Coord *x, Coord *y );
    static void     intf_fill_rect     (hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2);
    static hidval   add_timer          ( void (*func) (hidval user_data_), unsigned long milliseconds_, hidval user_data_ );

    /* Private member Functions */
    void            wheelEvent         ( QWheelEvent * e );
    void            mouseMoveEvent     ( QMouseEvent * e );
    void            mousePressEvent    ( QMouseEvent* e );
    void            keyPressEvent      ( QKeyEvent *e );
    void            load_fonts         ( void );

    static void     vertexCallback ( double* vertex, fontmap_type* map );
    static void     combineCallback ( GLdouble coords[3], GLdouble* vertex_data[4], GLfloat weight[4], void** out_data );
    static void     endCallback ( fontmap_type* map );
    static void     beginCallback ( GLenum which, fontmap_type* map );
    static void     errorCallback ( GLenum error_code );



    /* Drawing functions */
    void            initializeGL       ();
    void            paintGL            ( void );
    bool            check_triangle_space( triangle_buffer *buffer, int count );
    int             calc_slices        (float pix_radius, float sweep_angle );
    void            Drawcrosshairs     ( void );
    void            draw_arc           ( Coord width, Coord x, Coord y, Coord rx, Coord ry, Angle start_angle, Angle delta_angle, float scale );
    void            draw_slanted_cross (int x, int y, int z );
    void            draw_dozen_cross   ( int x, int y, int z );
    void            draw_cap           ( Coord width, Coord x, Coord y, Angle angle, double scale );
    void            draw_line          ( int cap, Coord width, Coord x1, Coord y1, Coord x2, Coord y2, double scale );
    void            draw_glyph         ( hidGC gc, TextType * text );
    void            draw_right_cross   ( float x, float y, int z );
    void            hidgl_draw_grid    ( BoxType *drawn_area );
    Coord           GridFit            ( Coord x, Coord grid_spacing, Coord grid_offset );
    void            resizeGL           ( int width, int height );
    void            add_triangle       ( triangle_buffer *buffer, float x1, float y1, float x2, float y2, float x3, float y3 );
    void            add_triangle_3D    ( triangle_buffer *buffer, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3 );
    void            flush_triangles    ( triangle_buffer *buffer );
    void            draw_rect          ( Coord x1, Coord y1, Coord x2, Coord y2 );
    void            fill_circle        ( Coord vx, Coord vy, Coord vr, double scale );
    void            setColors          ( double r, double b, double g, double a );

    static int  move_to( const FT_Vector* to, void* user );
    static int  line_to( const FT_Vector* to, void* user );
    static int  conic_to( const FT_Vector* control, const FT_Vector* to, void* user );
    static int  cubic_to( const FT_Vector* contro11, const FT_Vector* control2, const FT_Vector* to, void* user );



    bool BGLint;
    QOpenGLShaderProgram  Shader;
    QOpenGLVertexArrayObject VertexArray;
    QGLBuffer mTrianglesVBO;
    QGLBuffer mVBO;
    QGLBuffer mCrossHairsVBO;

    QOpenGLVertexArrayObject VAO;
    const GLuint WIDTH = 8000, HEIGHT = 8000;
    QMap<GLubyte,fontmap_type> fonts;
    glm::mat4            Model, View, Projection, Scale;
    glm::mat4            Translation;
    int                  colorIdx;

private slots:
    void ShowContextMenu( const QPoint & );
};


#endif /* QTHID_H_ */
