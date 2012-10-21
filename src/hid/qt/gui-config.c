/*
 * gui-config.c
 *
 *  Created on: Oct 20, 2012
 *      Author: dale
 */

#include "global.h"
#include "../hidint.h"
#include "../../hid.h"

void
ghid_config_init (void)
{
  HID_AttrNode *ha;
  HID_Attribute *a;
//  ConfigAttribute *ca, dummy_attribute;
//  ConfigColor *cc;


//  ghidgui->n_mode_button_columns = 3;
//  ghidgui->small_label_markup = TRUE;
//  ghidgui->history_size = 5;
//  dup_string (&color_file, "");

  for (ha = hid_attr_nodes; ha; ha = ha->next)
    {
      for (a = ha->attributes; a < ha->attributes + ha->n; ++a)
	{
	  if (!a->value)
	    continue;
//	  if ((ca = lookup_config_attribute (a->name, TRUE)) == NULL)
//	    ca = &dummy_attribute;
//	  ca->value = a->value;	/* Typically &Setting.xxx */
//	  ca->type = CONFIG_Unused;
	  switch (a->type)
	    {
	    case HID_Boolean:
	      *(char *) a->value = a->default_val.int_value;
//	      ca->type = CONFIG_Boolean;
	      break;
	    case HID_Integer:
	      *(int *) a->value = a->default_val.int_value;
//	      ca->type = CONFIG_Integer;
	      break;
	    case HID_Coord:
	      *(Coord *) a->value = a->default_val.coord_value;
//	      ca->type = CONFIG_Coord;
	      break;
	    case HID_Real:
	      *(double *) a->value = a->default_val.real_value;
//	      ca->type = CONFIG_Real;
	      break;

	    case HID_String:
	      if (!a->name)
		break;
	      *(char **) a->value = g_strdup (a->default_val.str_value);
//	      ca->type = CONFIG_String;

//	      len = strlen (a->name);
//	      if (len < 7 || strstr (a->name, "color") == NULL)
		break;

//	      cc = g_new0 (ConfigColor, 1);
//	      cc->attributes = a;

//	      if (!strncmp (a->name, "layer-color", 11))
//		cc->type = LAYER_COLOR;
//	      else if (!strncmp (a->name, "layer-selected-color", 20))
//		cc->type = LAYER_SELECTED_COLOR;
//	      else if (!strncmp (a->name + len - 14, "selected-color", 14))
//		cc->type = MISC_SELECTED_COLOR;
//	      else
//		cc->type = MISC_COLOR;

//	      config_color_list = g_list_append (config_color_list, cc);
	      break;

	    case HID_Enum:
	    case HID_Unit:
	      *(int *) a->value = a->default_val.int_value;
	      break;

	    case HID_Label:
	    case HID_Mixed:
	    case HID_Path:
	      break;
	    default:
	      abort ();
	    }
	  }
  }
}
