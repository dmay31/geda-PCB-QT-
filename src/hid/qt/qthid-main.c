#include "global.h"
#include "gui.h"
#include "../hidint.h"


HID qhid_hid;

void hid_qt_init()
{

	  qhid_hid.struct_size              = sizeof (HID);
	  qhid_hid.name                     = "qt";
	  qhid_hid.description              = "Qt Interface";
	  qhid_hid.gui                      = 1;
	  qhid_hid.poly_after               = 1;

	  qhid_hid.parse_arguments          = qhid_parse_arguments;
	  qhid_hid.logv						= ghid_logv;


	  hid_register_hid (&qhid_hid);

}
