/*
 * gui-top-window.c
 *
 *  Created on: Oct 19, 2012
 *      Author: dale
 */

#include <QtGui>
#include <QToolBar>
#include <QPainter>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGLWidget>

#include "global.h"
#include "gui.h"
#include "gui-top_window.h"
#include "gui-graphics-scene.h"
static void qt_build_top_window ( void );


void qhid_parse_arguments (int *argc, char ***argv)
{


  	ghid_config_init();
}


void qt_do_export ( void )
{
qt_build_top_window();

}

static void qt_build_top_window ( void )
{
    QApplication a(0, NULL );
    Top_Window w;

    w.show();
    a.exec();
}

/* Constructor */
Top_Window::Top_Window()

{

	GLWidget * gl_w = new GLWidget;
    QPixmap pixmap( "folder.png" );


    QGraphicsLineItem * line = new QGraphicsLineItem(10, 10, 500, 500);



    /* Add a Toolbar to the Main Window */
	MyToolBar = this->addToolBar("Main Toolbar");
    MyToolBar->setGeometry(0,0,200,20);
    MyToolBar->addAction( QIcon(pixmap), "Open File" );
    MyToolBar->addSeparator();
    MyToolBar->addSeparator();

	this->setCentralWidget( gl_w );


}


Top_Window::~Top_Window()
{

}
