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
#include <QComboBox>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QActionGroup>
#include <QString>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPen>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QGLFormat>
#include "library_window.hpp"

#include "global.h"
#include "gui.h"
#include "gui-top_window.h"
#include "pcb-printf.h"
#include "misc.h"
#include "TreeModel.hpp"

#include "kicadImport.h"

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
static char command[] ="";
static int argc = 1;
char ** argv = (char**)&command;
QHBoxLayout * mainLayout;
QGLFormat glFormat;


/* New QT executable object */
App = new QApplication( argc, argv );

glFormat.setVersion( 4, 3 );
glFormat.setProfile( QGLFormat::CoreProfile );
glFormat.setSampleBuffers( true );

/* New OpenGL QT object */
qHID = new QtHID(0);

mainLayout = new QHBoxLayout;
qHID->setLayout( mainLayout );

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

this->resize( QDesktopWidget().availableGeometry(this).size() *.8);

    QMenu * MenuPtr;
    QTextEdit*    console = new QTextEdit;
    QListWidget* table = new QListWidget;
    QTreeView * tree = new QTreeView(this);
    QTableWidgetItem* item;
    QTableWidgetItem*    item2;
    QAction * ViaButton;
    QAction * RemoveButton;
    QAction * LineButton;
    QAction * LineWidthButton;
    QAction * PolygonButton;
    QAction * TextButton;
    QAction * GridOnOff;
    QAction * SnapToGrid;
    QFont     serifFont("Roman", 10, QFont::Bold);
    QPixmap pixmap(100,100);
    int x;
    pixmap.fill(QColor("red"));
    QTextEdit * text = new QTextEdit;


    myStatusBar = new QStatusBar;
    this->setStatusBar( myStatusBar );
    myStatusBar->showMessage(tr("Ready"));
    myStatusBar->addPermanentWidget( new QLabel("Right", this));

    QDockWidget *leftDock = new QDockWidget( tr("Project Outline"), this );
    QDockWidget *dockConsole = new QDockWidget( tr("Console"), this);

    QVBoxLayout* buttonlayout = new QVBoxLayout;

    QToolBar* sideToolbar = new QToolBar;
    QActionGroup * actionGroup = new QActionGroup(this);
    QObject::connect( actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionEvent(QAction*)));

    /* Via Button */
    ViaButton    = sideToolbar->addAction( tr("VIA") );
    ViaButton->setIcon((QIcon(":/via.svg")));
    ViaButton->setObjectName( QString("Via"));
    ViaButton->setCheckable( TRUE );
    ViaButton->setActionGroup( actionGroup );

    /* Remove Button */
    RemoveButton = sideToolbar->addAction( tr("Remove") );
    RemoveButton->setIcon( QIcon(":/erase_x.png") );
    RemoveButton->setObjectName( QString("Remove"));
    RemoveButton->setCheckable( TRUE );
    RemoveButton->setActionGroup( actionGroup );

    /* Line Button */
    LineButton   = sideToolbar->addAction( tr("Line")  );
    LineButton->setObjectName( QString("Line"));
    LineButton->setIcon((QIcon(":/add_tracks.svg")));
    LineButton->setCheckable( TRUE );
    LineButton->setActionGroup( actionGroup );

    /* Width Track Button */
    LineWidthButton   = sideToolbar->addAction( tr("Line Width")  );
    LineWidthButton->setObjectName( QString("Line Width"));
    LineWidthButton->setIcon((QIcon(":/width_track.svg")));
    LineWidthButton->setCheckable( TRUE );
    LineWidthButton->setActionGroup( actionGroup );

    /* Polygon Button */
    PolygonButton = sideToolbar->addAction( QIcon::fromTheme("Polygon"), "Polygon"  );
    PolygonButton->setObjectName( QString("Polygon"));
    PolygonButton->setIcon( QIcon(":/add_zone.svg"));
    PolygonButton->setCheckable( TRUE );
    PolygonButton->setActionGroup( actionGroup );

    TextButton = sideToolbar->addAction( QIcon::fromTheme("Polygon"), "Text"  );
    TextButton->setObjectName( QString("Text"));
    TextButton->setIcon( QIcon(":/text.svg"));
    TextButton->setCheckable( TRUE );
    TextButton->setActionGroup( actionGroup );

    this->addToolBar(Qt::LeftToolBarArea, sideToolbar);

    dockConsole->setAllowedAreas(Qt::RightDockWidgetArea);

    dockConsole->setWidget( (QWidget*)console );



    console->textCursor().insertText("Welcome to gEDA!");

    //table->setRowCount( 10 );
    //table->setColumnCount( 2 );
    //table->verticalHeader()->setVisible(FALSE);
    //table->horizontalHeader()->setVisible(FALSE);
    //table->setShowGrid(FALSE);
    //table->horizontalHeader()->setResizeMode( QHeaderView::Stretch);

    //QObject::connect( table, SIGNAL(cellClicked( int, int)), this, SLOT( LayerEvent( int, int)));

   /* for( x=0; x<10; x++ )
        {
        item = new QTableWidgetItem;
        item2 = new QTableWidgetItem;
        item2->setIcon(QIcon(":/EyeOn.png"));
        item->setText("Layer 1");
        item->setFont( serifFont );
        item->setIcon(QIcon(pixmap));
        table->setItem( x, 0, item );
        table->setItem( x, 1, item2 );

        table->setItem( x, 1, item2 );
        }
*/
    addDockWidget(Qt::RightDockWidgetArea, dockConsole );
    addDockWidget(Qt::RightDockWidgetArea, leftDock );
    leftDock->setGeometry(0,0,200,200);


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

    MenuPtr = Menu[OPTIONS_MENU];
    MenuPtr = MyMenuBar->addMenu(tr("&View"));
    GridOnOff = new QAction(tr("Visible Grid"), this );
    GridOnOff->setStatusTip(tr("Turn Grid On and Off"));
    GridOnOff->setCheckable( TRUE );
    connect( GridOnOff, SIGNAL(triggered()), this, SLOT(GridSetting()));

    SnapToGrid = new QAction(tr("Snap To Grid"), this );
    SnapToGrid->setStatusTip(tr("Snap to Grid?"));
    SnapToGrid->setCheckable( TRUE );
    SnapToGrid->setChecked( TRUE );
    connect( SnapToGrid, SIGNAL(triggered()), this, SLOT(GridSnap()));

    MenuPtr->addAction(GridOnOff);

    /* Add a Toolbar to the Main Window */
    MyToolBar = this->addToolBar("Main Toolbar");
    MyToolBar->setGeometry(0,0,200,20);
    MyToolBar->addAction( QIcon::fromTheme("document-new"), "New Layout" );
    MyToolBar->addAction( QIcon::fromTheme("document-open"), "Open File");
    MyToolBar->addAction( QIcon::fromTheme("document-save"), "Save File");
    MyToolBar->addSeparator();

    prj_outline = new TreeModel("Test");
    tree->setModel( prj_outline );
    leftDock->setWidget( tree );

    connect( tree, SIGNAL( clicked(QModelIndex)), this, SLOT( OnclickedTree(QModelIndex)) );
    connect( tree, SIGNAL( doubleClicked( QModelIndex )), this, SLOT( OnDoubleclickedTree( QModelIndex)) );

    MyToolBar->addSeparator();

    bir = resource_parse (0, gpcb_menu_default);
    mr = resource_subres (bir, "Mouse");
    if (mr)
      load_mouse_resource (mr);

    QSize test = qHID->sizeHint();
    QSize test2 = text->sizeHint();
    this->setWindowTitle("gEDA PCB");
    this->setCentralWidget( qHID );


}


Top_Window::~Top_Window()
{

}

void Top_Window::GridSetting(void)
{
hid_parse_actions("Display(Grid)");
}

void Top_Window::GridSnap(void)
{

}

void Top_Window::OnclickedTree( QModelIndex index )
{
int x;

TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

if( item->isLayer == TRUE )
	{
	this->prj_outline->resetActive();
	item->activeLayer = TRUE;


	ChangeGroupVisibility( index.row() , TRUE, TRUE );



	printf("Tree Clicked  row: %u  column: %u \n", index.row(), index.column() );
	}
}

void Top_Window::OnDoubleclickedTree( QModelIndex index )
{
printf("Tree Double Clicked\n");
}

void Top_Window::actionEvent( QAction* action )
{
if( action->objectName() == QString("Via") )
    {
    if( Settings.Mode == VIA_MODE )
        {
        action->setChecked( FALSE );
        SetMode( NO_MODE );
        myStatusBar->showMessage(tr("Ready"));
        }
    else
        {
        SetMode( VIA_MODE );
        myStatusBar->showMessage(tr("Via Mode"));
        }
    }
else if( action->objectName() == QString("Line") )
    {
    if( Settings.Mode == LINE_MODE )
        {
        action->setChecked( FALSE );
        SetMode( NO_MODE );
        myStatusBar->showMessage(tr("Ready"));
        }
    else
        {
        SetMode( LINE_MODE );
        myStatusBar->showMessage(tr("Line Mode"));
        }
    }
else if( action->objectName() == QString("Remove") )
    {
    if( Settings.Mode == REMOVE_MODE )
        {
        SetMode( NO_MODE );
        action->setChecked( FALSE );
        myStatusBar->showMessage(tr("Ready"));
        }
    else
        {
        SetMode( REMOVE_MODE );
        myStatusBar->showMessage(tr("Delete"));
        }
    }
else if( action->objectName() == QString("Polygon") )
    {
    if( Settings.Mode == POLYGON_MODE )
        {
        SetMode( NO_MODE );
        action->setChecked( FALSE );
        myStatusBar->showMessage(tr("Ready"));
        }
    else
        {
        SetMode( POLYGON_MODE );
        myStatusBar->showMessage(tr("Polygon"));
        }
    }
else if( action->objectName() == QString("Text") )
    {
	if( Settings.Mode == TEXT_MODE )
		{
		SetMode( NO_MODE );
		action->setChecked( FALSE );
		myStatusBar->showMessage(tr("Ready"));
		}
	else
		{
		//kicadFootPrintDataType * FootprintData;
		//FootprintData = importFile();

		//qHID->drawKicadFootPrint( FootprintData );

		library_window * my_window = new library_window();

		QObject::connect( my_window, SIGNAL(footPrintSelection( QString )), this, SLOT( loadFootPrint( QString )));
		my_window->exec();

		SetMode( TEXT_MODE );
		myStatusBar->showMessage(tr("Text"));
		}
    }
else
    {
    SetMode( NO_MODE );
    myStatusBar->showMessage(tr("Ready"));
    }

printf( "The button is: %s\n", action->objectName().toUtf8().constData() );
}

void Top_Window::LayerEvent( int row, int column )
{
printf( "LayerEvent %d, %d\n", row, column );

if( column == 1)
    {
    ChangeGroupVisibility (row, TRUE, true);
    }


qHID->update();
}

void Top_Window::loadFootPrint( QString path )
{
	kicadFootPrintDataType * FootprintData;

	printf("Top Window load footprint!\n");

	FootprintData = importFile( (char*)path.toUtf8().constData() );
	qHID->drawKicadFootPrint( FootprintData );
}

#include "gui-top_window.moc"
