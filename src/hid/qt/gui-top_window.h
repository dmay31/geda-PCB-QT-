/*
 * gui-top_window.h
 *
 *  Created on: Nov 10, 2012
 *      Author: dale
 */

#ifndef GUI_TOP_WINDOW_H_
#define GUI_TOP_WINDOW_H_

#include <QtGui>
#include <QComboBox>
#include <QMainWindow>
#include <QTextEdit>
#include <QTreeView>
#include <QTableWidgetItem>
#include "treeitem.hpp"
#include "TreeModel.hpp"

#include "misc.h"
#include "qthid.hpp"


class Top_Window : public QMainWindow
{
    Q_OBJECT

public:
    Top_Window();
    ~Top_Window();

    static void build_top_window( HID_Attr_Val* A );

protected:

private:
    /* Functions */
    void addButtonToolbar( void );

    /* Member Objects */
    QComboBox * MyComboBox;
    QToolBar * MyToolBar;
    QMenuBar * MyMenuBar;
    QMenu * Menu[5];
    QStatusBar * myStatusBar;
    TreeModel* prj_outline;

    QtHID*    glObj;

    enum
    {
    FILE_MENU,
    EDIT_MENU,
    HELP_MENU,
    OPTIONS_MENU,

    MENU_CNT
    };

public slots:
    void GridSetting();
    void GridSnap();
    void actionEvent( QAction* );
    void LayerEvent( int, int );
    void OnclickedTree( QModelIndex );
    void OnDoubleclickedTree( QModelIndex );
    void loadFootPrint( QString );

};


#endif /* GUI_TOP_WINDOW_H_ */
