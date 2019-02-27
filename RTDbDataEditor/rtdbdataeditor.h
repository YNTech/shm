#ifndef RTDBDATAEDITOR_H
#define RTDBDATAEDITOR_H

#include <QtWidgets>
#include "ui_rtdbdataeditor.h"
#include "rtdb_define.h"
class RTDbDataEditor : public QMainWindow
{
	Q_OBJECT

public:
	RTDbDataEditor(QString basepath, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~RTDbDataEditor();
private slots:
	void on_tree_db_itemClicked(QTreeWidgetItem * item, int column);
	void on_act_tblrow_triggered();//修改记录数
	void on_act_editindex_triggered(bool);//修改index
	void on_btn_saveind_clicked();

	void on_btn_pagepre_clicked();
	void on_btn_pagenext_clicked();
private:
	void showPage(int npage);
	bool m_indflag;

	const char* m_dbHeader;
	char* m_dbData;
	QList<int> colsizes;
	int m_maxPages;
	std::vector<tbldef>::iterator m_curTblDef;
private:
	Ui::RTDbDataEditorClass ui;
	QString m_basepath;
};

#endif // RTDBDATAEDITOR_H
