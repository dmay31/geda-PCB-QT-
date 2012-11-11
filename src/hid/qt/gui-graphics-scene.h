/*
 * gui-graphics-scene.h
 *
 *  Created on: Nov 11, 2012
 *      Author: dale
 */

#ifndef GUI_GRAPHICS_SCENE_H_
#define GUI_GRAPHICS_SCENE_H_
#include "global.h"
#include <QGLWidget>


class GLWidget : public QGLWidget
{

 public:
	 GLWidget(QWidget *parent = 0);
	 ~GLWidget();

private:
	void paintGL(void);
	QColor qtPurple;
};



#endif /* GUI_GRAPHICS_SCENE_H_ */
