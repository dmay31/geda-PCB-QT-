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
#include "gui.h"
#include <QApplication>
#include <QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QGLPixelBuffer>
#include <QObject>

/*-----------------------------------------------
  	        PreCompiler Defines
 ----------------------------------------------*/
#define TRIANGLE_ARRAY_SIZE 5461

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

typedef struct {
  float triangle_array [3 * 3 * TRIANGLE_ARRAY_SIZE];
  unsigned int triangle_count;
  unsigned int coord_comp_count;
} triangle_buffer;

/*-----------------------------------------------
  	  	      Class Definitions
 ----------------------------------------------*/
class Geometry;
class Cube;

class DrawPort
{
public:
	int x;
	int y;
};

class QtHID : public QGLWidget
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
	void init_interface( HID* intf );
	void initializeGL();
	void flush_triangles (triangle_buffer *buffer);
	void draw_line(int cap, Coord width, Coord x1, Coord y1, Coord x2, Coord y2, double scale);
	void add_triangle_3D (triangle_buffer *buffer,
	                       GLfloat x1, GLfloat y1, GLfloat z1,
	                       GLfloat x2, GLfloat y2, GLfloat z2,
	                       GLfloat x3, GLfloat y3, GLfloat z3);
	void add_triangle(triangle_buffer *buffer,
	        float x1, float y1,
	        float x2, float y2,
	        float x3, float y3);
	void setcrosshairs( int x, int y );

public slots:
	void valueChanged(bool);

private:
	/* Static variables */

	static HID_DRAW QtGraphics;
	static QApplication*  	App; //The main QT Window Application

	triangle_buffer buffer;
	QColor qtGreen;
	DrawPort qport;

	/* Private Variables */



	/* Private Functions */
    static hidGC    make_gc( void );
	static void     parse_arguments( int *argc, char ***argv );
	static void     logger( const char *fmt, va_list args );
	static void     set_color( hidGC gc, const char *name );
	static void     set_line_cap( hidGC gc, EndCapStyle style );
    static void     draw_a_line( hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2 );
    static void     update_widget( bool change_complete );
    static void     set_crosshair( int x, int y, int action );
    static void     set_draw_xor(hidGC gc, int xor_);
    static void     set_line_width( hidGC gc, Coord width );


	void mouseMoveEvent ( QMouseEvent * e );
	//void mousePressEvent( QMouseEvent* ev );
	void paintGL(void);
	void Drawcrosshairs(void);
	void draw_slanted_cross (int x, int y, int z);
	void draw_dozen_cross (int x, int y, int z);
	void draw_right_cross (float x, float y, int z);
	void hidgl_draw_grid (BoxType *drawn_area);
	Coord GridFit (Coord x, Coord grid_spacing, Coord grid_offset);
	void resizeGL(int width, int height);

	Geometry *geom;
	Cube *cube;
	QGLPixelBuffer *pbuffer;


};


#endif /* QTHID_H_ */
