
#include <QApplication>
#include <QtCore>
#include "global.h"
#include "hid_draw.h"
#include "gui.h"
#include "../hidint.h"
#include "qthid.h"


HID qhid_hid;
QtHID*	HID_obj;
QApplication*  App; //( 0, NULL);

static HID_DRAW qhid_graphics;

hid_gc_struct* qt_make_gc( void );
void qt_set_color(hidGC gc, const char *name);
void qt_set_line_cap(hidGC gc, EndCapStyle style);
void qt_set_line_width(hidGC gc, Coord width);
void qt_draw_line (hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2);
void qt_update_widget( bool change_complete );
void qt_set_crosshair( int x, int y, int action );
void qt_set_draw_xor(hidGC gc, int xor_);

void hid_qt_init()
{
	  App = new QApplication( 0, NULL );

	  HID_obj = new QtHID(0);

	  HID_obj->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);


	  qhid_hid.struct_size              = sizeof (HID);
	  qhid_hid.name                     = "qt";
	  qhid_hid.description              = "Qt Interface";
	  qhid_hid.gui                      = 1;
	  qhid_hid.poly_after               = 1;

	  qhid_hid.parse_arguments          = qhid_parse_arguments;
	  qhid_hid.logv						= ghid_logv;
	  qhid_hid.do_export				= qt_do_export;
	  qhid_hid.graphics 				= &qhid_graphics;
	  qhid_hid.graphics->make_gc		= qt_make_gc;
      qhid_hid.graphics->set_color		= qt_set_color;
	  qhid_hid.graphics->set_line_cap	= qt_set_line_cap;
	  //qhid_hid.graphcis->set_line_width			= qt_set_line_width;
      qhid_hid.graphics->draw_line		= qt_draw_line;
	  qhid_hid.notify_crosshair_change	= qt_update_widget;
	  qhid_hid.set_crosshair			= qt_set_crosshair;
	  qhid_hid.graphics->set_draw_xor   = qt_set_draw_xor;
	  qhid_hid.graphics->set_line_width = qt_set_line_width;


	  hid_register_hid (&qhid_hid);

}


hidGC qt_make_gc( void )
{
hidGC gc;
gc = (hidGC)malloc( sizeof(struct hid_gc_struct) );
gc->me_pointer = &qhid_hid;
gc->colorname = Settings.BackgroundColor;
gc->alpha_mult = 1.0;
return gc;
}

void qt_set_color(hidGC gc, const char *name)
{
gc->colorname = name;
}

void qt_set_line_cap(hidGC gc, EndCapStyle style)
{
gc->cap = style;
}

void qt_set_line_width(hidGC gc, Coord width)
{
gc->width = width;
}

void qt_draw_line (hidGC gc, Coord x1, Coord y1, Coord x2, Coord y2)
{
 // USE_GC (gc);
HID_obj->draw_line (gc->cap, gc->width, x1, y1, x2, y2, 1 );
}

void qt_update_widget( bool change_complete )
{
	HID_obj->update();
}

void qt_set_crosshair( int x, int y, int action )
{
	HID_obj->setcrosshairs( x, y );
}

void
ghid_logv (const char *fmt, va_list args)
{
//char *msg = pcb_vprintf (fmt, args);
}

void qt_set_draw_xor(hidGC gc, int xor_)
{
// TODO
}


