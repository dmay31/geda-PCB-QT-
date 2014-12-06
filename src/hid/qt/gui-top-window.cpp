/*
 * gui-top-window.c
 *
 *  Created on: Oct 19, 2012
 *      Author: dale
 */

#include <QtCore>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QGLWidget>
#include <QtGui>

#include "global.h"
#include "gui.h"
#include "gui-graphics-scene.h"
#include "gui-top_window.h"
#include "pcb-printf.h"

extern "C" {
#include "gpcb-menu.h"
}
/* Variables */
static QApplication* App;
static QtHID*        qHID;
static HID           intfQT;
static Top_Window*   pTopWindow;

/**
 * @brief Initialize the QT interface
 */
void hid_qt_init( void )
{
/* New QT executable object */
App = new QApplication( 0, NULL );

/* New OpenGL QT object */
qHID = new QtHID(0);
qHID->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

/* Setup the interface object with the drawing Widget */
qHID->init_interface( &intfQT );

/* Top Window related callbacks for the PCB framework */
intfQT.do_export = Top_Window::build_top_window;

/* Register the interface callbacks with the PCB framework */
hid_register_hid ( &intfQT );
} /* hid_qt_init() */


/**
 * @brief Construct a new top window widget
 *
 * @param attributes regarding the profile of the Top Window
 */
void Top_Window::build_top_window ( HID_Attr_Val* A  )
{
/* Instantiate a new Top Window */
pTopWindow = new Top_Window;

/* Show the top window */
pTopWindow->show();

/* Run the application (No return!) */
App->exec();
}

/* Constructor */
Top_Window::Top_Window()
{
const Resource *r = 0, *bir;
const Resource *mr;


    QMenu * MenuPtr;
    QTextEdit*    console = new QTextEdit;
    QListWidget* table = new QListWidget;
    QListWidgetItem* item = new QListWidgetItem;
    QAction * ViaButton;
    QAction * RemoveButton;

    QDockWidget *leftDock = new QDockWidget( tr("Layers"), this );
    QDockWidget *dockConsole = new QDockWidget( tr("Console"), this);

    QVBoxLayout* buttonlayout = new QVBoxLayout;

    QToolBar* sideToolbar = new QToolBar;
    ViaButton    = sideToolbar->addAction( QIcon::fromTheme("document-new"), "VIA", qHID, SLOT( valueChanged(bool) ) );
    RemoveButton = sideToolbar->addAction( QIcon::fromTheme("delete"), "Remove", qHID, SLOT( valueChanged(bool) ) );

    ViaButton->setCheckable( TRUE );
    RemoveButton->setCheckable( TRUE );

    this->addToolBar(Qt::LeftToolBarArea, sideToolbar);

    dockConsole->setAllowedAreas(Qt::RightDockWidgetArea);

    dockConsole->setWidget( (QWidget*)console );
    leftDock->setWidget( table );


    console->textCursor().insertText("Welcome to gEDA!");
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

    bir = resource_parse (0, gpcb_menu_default);
    mr = resource_subres (bir, "Mouse");
    if (mr)
      load_mouse_resource (mr);


    this->setWindowTitle("gEDA PCB");
    this->setCentralWidget( qHID );


}


Top_Window::~Top_Window()
{

}


#include "gui-top_window.moc"
