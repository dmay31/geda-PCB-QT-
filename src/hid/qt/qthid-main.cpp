#include "global.h"
#include "gui.h"
#include "../hidint.h"
#include "qthid.h"

HID qhid_hid;
static QtHID*	HID_obj;


void hid_qt_init()
{
	  HID_obj = new QtHID();
	  qhid_hid.struct_size              = sizeof (HID);
	  qhid_hid.name                     = "qt";
	  qhid_hid.description              = "Qt Interface";
	  qhid_hid.gui                      = 1;
	  qhid_hid.poly_after               = 1;

	  qhid_hid.parse_arguments          = qhid_parse_arguments;
	  qhid_hid.logv						= ghid_logv;
	  qhid_hid.do_export				= qt_do_export;
	  qhid_hid.make_gc					= HID_obj->make_gc();


	  hid_register_hid (&qhid_hid);

}
