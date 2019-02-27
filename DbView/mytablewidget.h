#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>

class myTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit myTableWidget(QWidget *parent = 0):QTableWidget(parent){}
	myTableWidget(int rows, int columns, QWidget *parent = 0):QTableWidget(rows,columns,parent){};
protected:
	void commitData ( QWidget * editor );
signals:
	void onEditValue(QString);
};
#endif // MYTABLEWIDGET_H
