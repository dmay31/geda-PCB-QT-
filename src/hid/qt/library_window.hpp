/*
 * library_window.hpp
 *
 *  Created on: Dec 31, 2018
 *      Author: dale
 */

#ifndef LIBRARY_WINDOW_H_
#define LIBRARY_WINDOW_H_

#include <QObject>
#include <QtCore>
#include <QDialog>
#include <QTreeWidget>
#include <QVBoxLayout>

class library_window : public QDialog
{
Q_OBJECT

public:

library_window( void );
~library_window( void );

signals:
void footPrintSelection( QString );

private:
QTabWidget * tabWidget;

private slots:
void footPrintSelected(QTreeWidgetItem * item,int col );
};

#endif

