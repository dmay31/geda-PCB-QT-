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

void qhid_parse_arguments (int *argc, char ***argv);
void ghid_logv (const char *fmt, va_list args);
void ghid_config_init (void);
void qt_do_export ( void );

#ifdef __cplusplus
}
#endif

#endif /* GUI_H_ */
