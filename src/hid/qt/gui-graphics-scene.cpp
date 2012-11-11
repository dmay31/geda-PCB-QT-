/*
 * gui-graphics-scene.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: dale
 */

#include <QtGui>
#include <QGraphicsScene>
#include <QRubberBand>
#include "qcolor.h"

#include "gui-graphics-scene.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent )
{
qtPurple = QColor::fromRgb( 255, 255,255 );

}

GLWidget::~GLWidget()
{

}

void GLWidget::paintGL(void)
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
qglClearColor(qtPurple.dark());

}
