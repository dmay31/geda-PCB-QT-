/*
 * gui-top_window.h
 *
 *  Created on: Nov 10, 2012
 *      Author: dale
 */

#ifndef GUI_TOP_WINDOW_H_
#define GUI_TOP_WINDOW_H_

#include <QtGui/QWidget>
#include <QMainWindow>
#include "qthid.h"

class QComboBox;
class QToolBar;

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

    QtHID*	glObj;

    enum
    {
    FILE_MENU,
    EDIT_MENU,
    HELP_MENU,

    MENU_CNT
    };

signals:
	void valueChanged( void );

};


#endif /* GUI_TOP_WINDOW_H_ */
