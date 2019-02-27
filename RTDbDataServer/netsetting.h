#ifndef NETSETTING_H
#define NETSETTING_H

#include <QDialog>
#include "ui_netsetting.h"

class QLineEdit;
class QComboBox;

class netSetting : public QDialog
{
	Q_OBJECT

public:
	netSetting(QWidget *parent = 0);
	~netSetting();
private slots:
	void on_tbtn_open_clicked();
	void on_tbtn_load_clicked();
	void selectPath();
	void resizeTreeItem();
	void setDataSource(QTreeWidgetItem * item);
private:
	enum{TreeTp_Tbl=1,TreeTp_Db,TreeTp_DbSet};
	Ui::netSetting ui;

	QLineEdit* m_leRootPath;
	QComboBox* m_dbsetsnm;
};

#endif // NETSETTING_H
