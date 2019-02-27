#ifndef DBVIEW_H
#define DBVIEW_H

#include <QMainWindow>
#include "ui_dbview.h"


class DbView : public QMainWindow
{
	Q_OBJECT

public:
	DbView(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~DbView();
private slots:
	void on_act_opendbsets_triggered();
	void setTblEditable(bool f);
	void on_tree_db_itemClicked(QTreeWidgetItem * item, int column);
	void contextMenu ( const QPoint & pos );
	void contextMenu_treeddb ( const QPoint & pos );
	void showItemData(bool);
	void selectPath(bool flag = true);
	void datachanged(QString strdata);

	void on_btn_pagepre_clicked();
	void on_btn_pagenext_clicked();
	void close_dbsets();
//protected:
//	bool eventFilter(QObject *obj, QEvent *ev);
private:
	void showPage(int npage);

	enum dataItem_{VIEW_HEX,VIEW_TEXT,VIEW_INT,VIEW_FLOAT,VIEW_DOUBLE};
	Ui::DbViewClass ui;
	QLineEdit* m_leRootPath;
	QString m_rootPath;
	QComboBox* m_dbsetsnm;
	QMenu m_tblmenu;
	bool m_isEditable;
	QActionGroup m_actgroup;
	QAction * m_actedit;
	//unsigned int npages;

	const char* m_dbHeader;
	char* m_dbData;
	QList<int> colsizes;
	int m_maxPages;
};

#endif // DBVIEW_H
