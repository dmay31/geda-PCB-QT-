/*
 * qthid.h
 *
 *  Created on: Nov 16, 2012
 *      Author: dale
 */

#ifndef QTHID_H_
#define QTHID_H_

/* Graphics Context Class */
class GraphicContext
{
public:
	GraphicContext();
	~GraphicContext();

private:
	HID *me_pointer;
	const char *colorname;
	double alpha_mult;
	Coord width;
	int cap, join;
};







class QtHID
{
public:
	GraphicContext*		GC;
	void make_gc(void);

};


#endif /* QTHID_H_ */
