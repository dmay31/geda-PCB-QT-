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
public:
	Top_Window();
	~Top_Window();

protected:

private:
	/* Functions */

	/* Member Objects */
    QComboBox * MyComboBox;
    QToolBar * MyToolBar;

};


#endif /* GUI_TOP_WINDOW_H_ */
