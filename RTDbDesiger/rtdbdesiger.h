#ifndef RTDBDESIGER_H
#define RTDBDESIGER_H

#include <QtGui>
#include <QtWidgets/QtWidgets>
#include "ui_rtdbdesiger.h"

class RTDbDesiger : public QMainWindow
{
	Q_OBJECT

public:
	RTDbDesiger(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~RTDbDesiger();
	bool m_flag;
private slots:
	void on_act_opendbsets_triggered();
	void contextMenu_treeddb ( const QPoint & pos );
	void selectPath(bool flag = true);
	void selectPath2();
	void close_dbsets();
	void on_tree_db_itemClicked(QTreeWidgetItem * item, int column);
	void on_act_createdbsets_triggered();
	void add_db();
	void add_table();
	void del_db();
	void del_table();
	void del_dbsets();
	void save_dbsets();
	void save_db();
	void rebulid_dbsets();
	void rebulid_db(QTreeWidgetItem * item = nullptr,bool hasWg = true);
	void on_btn_add_clicked();
	void on_btn_delete_clicked();
	void on_btn_ok_clicked();
	void on_tbl_data_doubleClicked(const QModelIndex & ind);
	void on_act_saveall_triggered();
	void on_comb_datatype_currentIndexChanged(int index);
	void treeItemChanged(QTreeWidgetItem *);
	void setOrigName(QTreeWidgetItem *);
	void show_dbsets_property();
private:
	enum tbl_record{rec_name,rec_dispName,rec_dataTp,rec_len,rec_constraint,rec_default};
	enum{TreeTp_Tbl=1,TreeTp_Db,TreeTp_DbSet};
	Ui::RTDbDesigerClass ui;

	QLineEdit* m_leRootPath;
	QString m_rootPath;
	QComboBox* m_dbsetsnm;

	QString m_origName;
	void* m_uirecod;
};

#endif // RTDBDESIGER_H
