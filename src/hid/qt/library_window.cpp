/*
 * library_window.cpp
 *
 *  Created on: Dec 31, 2018
 *      Author: dale
 */


#include "library_window.hpp"
#include <dirent.h>
#include <sys/types.h>

library_window::library_window()
{
	DIR * footprintDir, * footprintDir2;
	struct dirent *de, *de2;

	QTreeWidget *treeWidget = new QTreeWidget();
	treeWidget->setColumnCount(1);
	treeWidget->headerItem()->setText( 0, "FootPrints" );
	QList<QTreeWidgetItem *> items;

	footprintDir = opendir( "/home/dale/Projects/geda-PCB-QT-/kicad-footprints");

	  if (footprintDir == NULL)  // opendir returns NULL if couldn't open directory
	    {
	        printf("Could not open current directory" );
	        return;
	    }

	while ((de = readdir(footprintDir)) != NULL)
    	{
		QTreeWidgetItem * newItem, * newItem2;
		char *            ext;
		char              libName[255];
		char              newPath[255];

		ext = strstr( de->d_name, ".pretty" );
		if( NULL != ext && de->d_type == DT_DIR )
		    {
			memset( libName, 0x00, 255 );
		    memcpy( libName, de->d_name, ext-de->d_name );

		    newItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(libName)));
            items.append( newItem );

            sprintf( newPath, "/home/dale/Projects/geda-PCB-QT-/kicad-footprints/%s", de->d_name );
            footprintDir2 = opendir( newPath );
        	while ((de2 = readdir(footprintDir2)) != NULL)
        	    {
        		ext = strstr( de2->d_name, ".kicad_mod" );

        		if( NULL != ext )
        		    {
        			memset( libName, 0x00, 255 );
        			memcpy( libName, de2->d_name, ext-de2->d_name );
        		    newItem2 = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(libName)));
        		    newItem2->setData( 0, Qt::UserRole, QVariant( QString().sprintf("%s/%s",newPath, de2->d_name ) ) );
        		    newItem->addChild( newItem2 );
        		    }
        	    }
		    }
    	}

	treeWidget->insertTopLevelItems(0, items);

	QObject::connect( treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), this, SLOT(footPrintSelected(QTreeWidgetItem *, int)));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget( treeWidget );

    setWindowTitle(tr("Tab Dialog"));
}

library_window::~library_window()
{

}

void library_window::footPrintSelected(QTreeWidgetItem * item, int col )
{

printf("You click on a footprint! %s \n", item->data(0, Qt::UserRole).toString().toUtf8().constData() );

emit footPrintSelection( item->data(0, Qt::UserRole).toString() );
}
