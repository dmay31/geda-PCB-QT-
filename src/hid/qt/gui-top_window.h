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

class QComboBox;
class QToolBar;

class Top_Window : public QMainWindow
{
	Q_OBJECT

public:
	Top_Window();
	~Top_Window();

protected:

private:
	/* Functions */
	void addButtonToolbar( void );

	/* Member Objects */
    QComboBox * MyComboBox;
    QToolBar * MyToolBar;
    QMenuBar * MyMenuBar;
    QMenu * Menu[5];

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
