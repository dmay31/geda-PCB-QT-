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
#include <QScrollArea>
 #include <QTextEdit>


#include "global.h"
#include "gui.h"
#include "gui-top_window.h"
#include "gui-graphics-scene.h"
#include "qthid.h"
#include "pcb-printf.h"

extern QtHID*	HID_obj;
extern QApplication* App;

static void qt_build_top_window ( void );


void qhid_parse_arguments (int *argc, char ***argv)
{


  	ghid_config_init();
}


void qt_do_export ( HID_Attr_Val* A )
{
qt_build_top_window();

}

static void qt_build_top_window ( void )
{

	//QApplication App(0, NULL );
    Top_Window w;

    w.show();
    App->exec();
}

/* Constructor */
Top_Window::Top_Window()

{
	QMenu * MenuPtr;
    QTextEdit*	console = new QTextEdit;
    QListWidget* table = new QListWidget;
    QListWidgetItem* item = new QListWidgetItem;
    QAction * ViaButton;




    QDockWidget *leftDock = new QDockWidget( tr("Layers"), this );
    QDockWidget *dockConsole = new QDockWidget( tr("Console"), this);

    QVBoxLayout* buttonlayout = new QVBoxLayout;

    QToolBar* sideToolbar = new QToolBar;
    ViaButton = sideToolbar->addAction( QIcon::fromTheme("document-new"), "VIA", HID_obj, SLOT( valueChanged(bool)) );
    ViaButton->setCheckable( TRUE );

    this->addToolBar(Qt::LeftToolBarArea, sideToolbar);

    dockConsole->setAllowedAreas(Qt::RightDockWidgetArea);


    dockConsole->setWidget( console );
    leftDock->setWidget( table );


    console->textCursor().insertText("Welcome to gEDA! ");
    item->setText("Layer 1");
    item->setIcon(QIcon("/home/dale/PCB/pcb/src/hid/qt/package_dim.png"));
    table->addItem(item);


    addDockWidget(Qt::RightDockWidgetArea, dockConsole );
    addDockWidget(Qt::RightDockWidgetArea, leftDock );


    MyMenuBar = this->menuBar();

    MenuPtr = Menu[FILE_MENU];
    MenuPtr = MyMenuBar->addMenu(tr("&File"));
    MenuPtr->addAction("Save");
    MenuPtr->addAction("Save As...");

    MenuPtr = Menu[EDIT_MENU];
    MenuPtr = MyMenuBar->addMenu(tr("&Edit"));
    MenuPtr->addAction("Undo");
    MenuPtr->addAction("Redo");
    MenuPtr->addSeparator();
    MenuPtr->addAction("Copy");
    MenuPtr->addAction("Paste");
    MenuPtr->addAction("Cut");

    /* Add a Toolbar to the Main Window */
	MyToolBar = this->addToolBar("Main Toolbar");
    MyToolBar->setGeometry(0,0,200,20);
    MyToolBar->addAction( QIcon::fromTheme("document-new"), "New Layout" );
    MyToolBar->addAction( QIcon::fromTheme("document-open"), "Open File");
    MyToolBar->addAction( QIcon::fromTheme("document-save"), "Save File");
    MyToolBar->addSeparator();
    MyToolBar->addSeparator();


    this->setWindowTitle("gEDA PCB");
    this->setCentralWidget( HID_obj );


}


Top_Window::~Top_Window()
{

}


#include "gui-top_window.moc"
