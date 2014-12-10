#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "kicadImport.h"

#define FALSE   0
#define TRUE    1

#define CHILD_GROW_SZ  5
#define BUFFER_SZ      1000

#define MODULE "module"
#define LAYER  "layer"
#define LAYERS "layers"
#define TEDIT  "tedit"
#define DESC   "descr"
#define FPTEXT "fp_text"
#define TAGS   "tags"
#define AT     "at"
#define EFFECTS "effects"
#define SIZE    "size"
#define THICKNESS "thickness"
#define START   "start"
#define END     "end"
#define WIDTH   "width"
#define FPLINE  "fp_line"
#define FONT    "font"
#define PAD     "pad"
#define DRILL   "drill"
#define MODEL   "model"
#define SCALE   "scale"
#define ROTATE  "rotate"
#define XYZ     "xyz"
#define ATTR    "attr"

enum{
	QUOTE_NONE   =  0,
	QUOTE_OPEN   =  2,
	QUOTE_CLOSED =  1
};

enum
    {
	FPTEXT_TYPE_REFERENCE = 0,
	FPTEXT_TYPE_VALUE     = 1,
	FPTEXT_TYPE_USER      = 2,

	FPTEXT_TYPE_INVALID
    };

typedef struct
    {
	int     type;
	char    verb[100];
    } typeTable;

typedef struct _xyz
    {
	float      x;
	float      y;
	float      z;
    } xyz_type;

typedef struct _KICAD_TYPE_fptext
    {
	uint8_t    type;
	char*      text;
    }KICAD_TYPE_fptext_type;


typedef struct _symbol
    {
	bool       opening;
	bool       closing;
	char  *    symbol;
    } symbol_type;

/********************************
 * Variables
 *******************************/
static item_type s_items;
static int       item_cnt = 0;
static typeTable  types[] = {
		                    { KICAD_TYPE_module,    MODULE    },
                            { KICAD_TYPE_fptext,    FPTEXT    },
							{ layer,     LAYER     },
							{ tedit,     TEDIT     },
							{ descr,     DESC      },
							{ tags,      TAGS      },
							{ at,        AT        },
							{ effects,   EFFECTS   },
							{ KICAD_TYPE_size,      SIZE      },
							{ thickness, THICKNESS },
							{ start,     START     },
							{ end,       END       },
							{ KICAD_TYPE_width,     WIDTH     },
							{ fpline,    FPLINE    },
							{ font,      FONT      },
							{ KICAD_TYPE_pad,       PAD       },
							{ drill,     DRILL     },
							{ model,     MODEL     },
							{ scale,     SCALE     },
							{ rotate,    ROTATE    },
							{ xyz,       XYZ       },
							{ layers,    LAYERS    },
							{ KICAD_TYPE_attr,      ATTR      },
                            };


symbol_type get_next_symbol( int fd, bool * eof )
{
symbol_type next_symbol;
static int  i = 0;
char        readBuf[BUFFER_SZ];
static char buf[BUFFER_SZ];
int         iStart;
char *      symbol;
bool        exit = FALSE;
bool        moreData = FALSE;
int         in_quotes = QUOTE_NONE;

memset( &next_symbol, 0x0, sizeof( symbol_type ) );
iStart = i;

while( !exit )
    {
	// assume not symbol is as large as the entire buffer
	if( i == 0 )
		{
		read( fd, buf, BUFFER_SZ );
		}
	else if( moreData )
	    {
		int len = BUFFER_SZ - iStart;

		// iStart is an index which is equivalent to the number indexes before this position
		read( fd, readBuf, iStart );

		//Copy remaining old buffer, and then add in the new data
		memmove( buf, buf+iStart, len );
		memcpy( buf + len, readBuf, iStart );
		moreData = FALSE;
		i = len;
		iStart = 0;
	    }

	while( 1 )
		{
		if( i >= sizeof( buf ) )
		    {
			moreData = TRUE;
			break;
		    }

		if( buf[i] == '(' && in_quotes != QUOTE_OPEN )
			{
			next_symbol.opening = TRUE;
			i++;
			iStart = i;
			continue;
			}
		else if( buf[i] == ')' && in_quotes != QUOTE_OPEN )
			{
			int len = ( i - iStart ) + 1 - in_quotes;

			next_symbol.closing = TRUE;
			symbol = (char*)malloc( len );
			memset( symbol, 0x0, len );
			memcpy( symbol, &buf[iStart], len - 1 );
			next_symbol.symbol = symbol;
			exit = TRUE;
			i++;
			break;
			}

        if( buf[i] == '"')
            {
        	if( in_quotes != QUOTE_OPEN ) //( iStart <= i )
        	    {
    		    in_quotes = QUOTE_OPEN;
    			i++;
    		    iStart = i;
        	    }
        	else
        	    {
        		in_quotes = QUOTE_CLOSED;
        		i++;
        		//iStart = i;
        	    }
            }
		else if( buf[i] == '\n' )
		    {
			if( iStart < i )
			    {
				int len = ( i - iStart ) + 1 - in_quotes;

				symbol = (char*)malloc( len );
				memset( symbol, 0x0, len );
				memcpy( symbol, &buf[iStart], len - 1 );
				next_symbol.symbol = symbol;
	            exit = TRUE;
				i++;
				break;
			    }
			else
			    {
			    i++;
			    iStart = i;
			    }
		    }
		else if( buf[i] != ' ' )
			{
			i++;
			}
		else if( in_quotes == QUOTE_OPEN )
		    {
			i++;
		    }
		else if( iStart == i )
		    {
			i++;
			iStart = i;
		    }
		else
			{
			int len = ( i - iStart ) + 1 - in_quotes;

			symbol = (char*)malloc( len );
			memset( symbol, 0x0, len );
			memcpy( symbol, &buf[iStart], len - 1 );
			next_symbol.symbol = symbol;
            exit = TRUE;
			i++;
			break;

			}
		}
    }

return( next_symbol );
}

static int get_fptext_type( char* symbol )
    {
	if( !strcmp( symbol, "reference") )
	    {
		return( FPTEXT_TYPE_REFERENCE );
	    }
	else if( !strcmp( symbol, "value") )
	    {
		return( FPTEXT_TYPE_VALUE );
	    }
	else if( !strcmp( symbol, "user") )
	    {
		return( FPTEXT_TYPE_USER );
	    }
	else
	    {
		return( FPTEXT_TYPE_INVALID );
	    }
    }

static int get_pad_type( char* symbol )
{
if( !strcmp( symbol, "thru_hole" ) )
    {
	return( PAD_TYPE_THRUHOLE );
    }
if( !strcmp( symbol, "smd" ) )
    {
	return( PAD_TYPE_SMD );
    }
if( !strcmp( symbol, "connect" ) )
    {
	return( PAD_TYPE_CONNECT );
    }
if( !strcmp( symbol, "np_thru_hole" ) )
    {
	return( PAD_TYPE_NP_THRUHOLE );
    }
else
    {
	return( PAD_TYPE_INVALID );
    }
}

static int get_pad_shape( char* symbol )
{
if( !strcmp( symbol, "circle" ) )
    {
	return( PAD_SHAPE_CIRCLE );
    }
if( !strcmp( symbol, "rect" ) )
    {
	return( PAD_SHAPE_RECT );
    }
if( !strcmp( symbol, "oval" ) )
    {
	return( PAD_SHAPE_OVAL );
    }
if( !strcmp( symbol, "trapezoid" ) )
    {
	return( PAD_SHAPE_TRAPEZOID );
    }
else
    {
	return( PAD_SHAPE_INVALID );
    }
}

static int get_type( char* symbol )
{
int i;
int ret;

for( i = 0; i < type_count; i++ )
    {
	if( !strcmp( types[i].verb, symbol) )
	    {
		ret = types[i].type;
		break;
	    }
    }

if( i == type_count )
    {
	printf("Could not find symbol type: %s\n", symbol );
    }

return( ret );
}

static char * get_type_str( int type )
{
int i;
char * str = NULL;

for( i = 0; i < type_count; i++ )
    {
	if( type == types[i].type )
	    {
		str = types[i].verb;
		break;
	    }
    }
}


kicadFootPrintDataType * importFile( char * path )
{
int fd;
symbol_type next_symbol;
item_type * pitem = &s_items;
_item *     temp;
bool eof = FALSE;

memset( pitem, 0x0, sizeof(item_type) );

fd = open( path, O_RDONLY );

while( !eof )
    {
    next_symbol = get_next_symbol( fd, &eof );
    printf("%s\n", next_symbol.symbol );

    if( 0 == strcmp( next_symbol.symbol, "pad" ) )
        {
    	printf("%s\n", next_symbol.symbol );
        }

	if( next_symbol.opening )
		{
		// No children array as been malloc'd yet
		if( NULL == pitem->children )
		    {
			//Family size should be zero too.
			pitem->children = (item_type**)malloc( CHILD_GROW_SZ * sizeof(item_type*) );
			memset( pitem->children, 0x00, CHILD_GROW_SZ * sizeof(item_type*) );
			pitem->family_sz = CHILD_GROW_SZ;
			pitem->child_idx = 0;
		    }
		else if( ( pitem->child_idx + 1 ) >= pitem->family_sz )
		    {
			pitem->children = (item_type**)realloc( pitem->children, ( pitem->family_sz + CHILD_GROW_SZ ) * sizeof(item_type*) );
			pitem->family_sz += CHILD_GROW_SZ;
		    }

		// If this is not the very first item - which 1= module
		if( item_cnt > 0 )
		    {
			pitem->child_idx;

			//malloc a new child in the children array
		    pitem->children[pitem->child_idx] = (_item*)malloc( sizeof(item_type) );
			memset( pitem->children[pitem->child_idx], 0x0, sizeof(item_type) );
			pitem->children[pitem->child_idx]->parent = pitem;
    		pitem->child_idx++;
            pitem = pitem->children[pitem->child_idx-1];
		    }

		//We have a new main symbol
		pitem->type = get_type( next_symbol.symbol );
		item_cnt++;
		}
	else
	    {
		switch( pitem->type )
		    {
		    case scale:
		    case font:
		    case effects:
		    case rotate:
		    	break;

		    case KICAD_TYPE_module:
		    	// Module only has one stand alone parameter. it's name
		    	pitem->data = malloc( strlen( next_symbol.symbol + 1 ) );
		    	strcpy( (char*)pitem->data, next_symbol.symbol );

		    	if( next_symbol.closing )
		    	    {
		    		// This should be the end of the file!
		    		return( &s_items );
		    	    }
		    	break;


		    case at:
		    case start:
		    case end:
		    case KICAD_TYPE_size:
		        {
		    	if( pitem->data == NULL )
		    	    {
		    		pitem->data = malloc( sizeof(at_type) );
		    		memset( pitem->data, 0x0, sizeof(at_type) );
		    	    }

		    	switch( pitem->data_cnt )
		    	    {
		    	    case 0:
		    	    	((at_type*)pitem->data)->x = atof( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;
		    	    case 1:
		    	    	((at_type*)pitem->data)->y = atof( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;

		    	    default:
		    	    	//Assert
		    	    	break;
		    	    }
		        }
		    	break;

		    case xyz:
	            {
		    	xyz_type * pdata = (xyz_type*)pitem->data;

		    	if( pdata == NULL )
		    	    {
		    		pdata = (xyz_type*)malloc( sizeof(xyz_type) );
		    		memset( pdata, 0x0, sizeof(xyz_type) );
		    	    }

		    	switch( pitem->data_cnt )
		    	    {
		    	    case 0:
		    	    	pdata->x = atof( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;
		    	    case 1:
		    	    	pdata->y = atof( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;
		    	    case 2:
		    	    	pdata->z = atof( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;

		    	    default:
		    	    	//Assert
		    	    	break;
		    	    }
		            }
		    	break;

		    case KICAD_TYPE_pad:
		        {
                pad_type * pdata = (pad_type*)pitem->data;

                if( pitem->data == NULL )
                    {
                	pitem->data = (pad_type*)malloc( sizeof(pad_type) );
		    		memset( pitem->data, 0x0, sizeof( pad_type) );
                    }

                switch( pitem->data_cnt )
                    {
                    case 0:
                    	// Name
                    	pitem->data_cnt++;
                    	break;
                    case 1:
                    	((pad_type*)pitem->data)->pad_type = get_pad_type( next_symbol.symbol );
                    	pitem->data_cnt++;
                    	break;

                    case 2:
                    	((pad_type*)pitem->data)->pad_shape = get_pad_shape( next_symbol.symbol );
                    	pitem->data_cnt++;
                    	break;

                    default:
                    	//Assert
                    	break;

                    }
		        }
		        break;

		    case KICAD_TYPE_fptext:
		        {
		        KICAD_TYPE_fptext_type * pdata = (KICAD_TYPE_fptext_type*)pitem->data;

		    	if( pdata == NULL )
		    	    {
		    		pdata = (KICAD_TYPE_fptext_type*)malloc( sizeof(KICAD_TYPE_fptext_type) );
		    		memset( pdata, 0x0, sizeof( KICAD_TYPE_fptext_type) );
		    	    }
		    	switch( pitem->data_cnt )
		    	    {
		    	    case 0:
		    	    	pdata->type = get_fptext_type( next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;
		    	    case 1:
		    	    	pdata->text = (char*)malloc( strlen( next_symbol.symbol ) + 1 );
		    	    	sprintf( pdata->text, "%s", next_symbol.symbol );
		    	    	pitem->data_cnt++;
		    	    	break;

		    	    default:
		    	    	//Assert
		    	    	break;
		    	    }
		        }
		    	break;

		    case KICAD_TYPE_width:
		    case drill:
		    	pitem->data = malloc( sizeof(float) );
		    	*((float*)pitem->data) = atof( next_symbol.symbol );
		    	break;

		    case layer:
		    case tedit:
		    case descr:
		    case tags:
		    case model:
		    	// These items have only 1 string for a data type
		    	pitem->data = malloc( strlen( next_symbol.symbol +1 ) );
		    	strcpy( (char*)pitem->data, next_symbol.symbol );
		    	break;

		    default:
		    	break;
		    }

    	if( next_symbol.closing )
    	    {
    		pitem = pitem->parent;
    		printf("Up a level to type: %s\n", get_type_str( pitem->type ) );
    	    }
	    }
    }

return( NULL );
}
