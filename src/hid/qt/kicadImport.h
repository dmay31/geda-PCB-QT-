/*
 * kicadImport.h
 *
 *  Created on: Dec 15, 2018
 *      Author: dale
 */

#ifndef SRC_HID_QT_KICADIMPORT_H_
#define SRC_HID_QT_KICADIMPORT_H_

enum{
	KICAD_TYPE_module,
	KICAD_TYPE_fptext,
	tedit,
	KICAD_TYPE_pad,
	line,
	layer,
	descr,
	tags,
	at,
	effects,
	KICAD_TYPE_size,
	thickness,
	start,
	end,
	KICAD_TYPE_width,
	fpline,
	font,
	drill,
	model,
	scale,
	rotate,
	xyz,
	layers,
	KICAD_TYPE_attr,

	type_count
};

enum pad_info_type
    {
	PAD_TYPE_THRUHOLE    = 0,
	PAD_TYPE_SMD         = 1,
	PAD_TYPE_CONNECT     = 2,
	PAD_TYPE_NP_THRUHOLE = 3,

	PAD_TYPE_INVALID     = 4,

	PAD_SHAPE_CIRCLE     = 5,
	PAD_SHAPE_RECT       = 6,
	PAD_SHAPE_OVAL       = 7,
	PAD_SHAPE_TRAPEZOID  = 8,

	PAD_SHAPE_INVALID    = 9
    };

typedef struct _at
    {
	float      x;
	float      y;
    } at_type, start_type, end_type, KICAD_pos_type;

typedef struct _pad_type
	{
	char *  name;
	int     pad_type;
	int     pad_shape;
	} pad_type, KICAD_pad_type;

typedef struct _item
    {
	int        type;
	int        data_cnt;
	int        child_idx;
	int        family_sz;
	void *     data;
	_item **   children;
	_item *    parent;
    } item_type, kicadFootPrintDataType;

kicadFootPrintDataType * importFile( char * path );


#endif /* SRC_HID_QT_KICADIMPORT_H_ */
