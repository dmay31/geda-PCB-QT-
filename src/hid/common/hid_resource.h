
#ifndef PCB_HID_COMMON_HID_RESOURCE_H
#define PCH_HID_COMMON_HID_RESOURCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "resource.h"

#define M_Shift   1
#define M_Ctrl    2
#define M_Mod(n)  (1<<(n+1))
#define M_Alt     M_Mod(1)
#define M_Multi   M_Mod(2)
#define M_Release (~((unsigned)-1>>1)) // set the top bit

void load_mouse_resource (const Resource *res);
void do_mouse_action (int button, int mods);
void do_key_action( int key );

#ifdef __cplusplus
}
#endif

#endif
