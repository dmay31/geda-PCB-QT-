/*
 * gui.h
 *
 *  Created on: Oct 19, 2012
 *      Author: dale
 */

#ifndef GUI_H_
#define GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hid.h"
extern void hid_register_hid (HID * hid);

void hid_qt_init(void);
void qhid_parse_arguments (int *argc, char ***argv);
void ghid_logv (const char *fmt, va_list args);
void ghid_config_init (void);
void qt_do_export ( HID_Attr_Val* A );

#ifdef __cplusplus
}
#endif

#endif /* GUI_H_ */
