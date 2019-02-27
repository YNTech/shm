#include "rtdbdesiger.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
#include "ui_RecordDef.h"
#include "ui_DbSetsPropDlg.h"
#include "comboboxdelegate.h"
#include "variantdelegate.h"
#include "rtdb_api.h"
#include "time.h"

using namespace std;

RTDbDesiger::RTDbDesiger(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags),m_flag(false)
{
	ui.setupUi(this);
	ui.tree_db->setItemDelegate(new VariantDelegate(this));
	//ui.tree_db->setEditTriggers(QAbstractItemView::AllEditTriggers);
	m_rootPath="../db";

	connect(ui.tree_db, SIGNAL(customContextMenuRequested ( const QPoint&)), this, SLOT(contextMenu_treeddb ( const QPoint &)));
	connect(ui.tree_db, SIGNAL(itemChanged ( QTreeWidgetItem *, int)), this, SLOT(treeItemChanged ( QTreeWidgetItem *)));
	connect(ui.tree_db, SIGNAL(itemClicked ( QTreeWidgetItem *, int)), this, SLOT(setOrigName ( QTreeWidgetItem *)));
}

RTDbDesiger::~RTDbDesiger()
{
}
void RTDbDesiger::on_act_opendbsets_triggered()
{
	QDialog dlg(this);
	//dlg.move(this->rect().center());
	QVBoxLayout* vlay = new QVBoxLayout(&dlg);
	QHBoxLayout* hlay = new QHBoxLayout;
	m_leRootPath = new QLineEdit(m_rootPath);
	hlay->addWidget(m_leRootPath);
	QToolButton* btn = new QToolButton();
	btn->setText("...");
	hlay->addWidget(btn);
	vlay->addLayout(hlay);
	hlay = new QHBoxLayout;
	hlay->addWidget(new QLabel(tr("DbSets:")));
	m_dbsetsnm = new QComboBox;
	m_dbsetsnm->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_dbsetsnm->setEditable(true);
	hlay->addWidget(m_dbsetsnm);
	vlay->addLayout(hlay);
	hlay = new QHBoxLayout;
	hlay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton* btnok = new QPushButton(tr("OK"));
	connect(btnok,SIGNAL(clicked()),&dlg,SLOT(accept()));
	connect(btn,SIGNAL(clicked()),this,SLOT(selectPath()));
	hlay->addWidget(btnok);
	vlay->addLayout(hlay);
	selectPath(false);
	if(dlg.exec()== QDialog::Accepted)
	{
		ui.tree_db->clear();
		char err[256];
		std::string strDbset = m_dbsetsnm->currentText().toStdString();
		const char* dbsetsname = strDbset.c_str();
		if (!readRTDbSetsDef(m_rootPath.toLocal8Bit().data(), dbsetsname, err))
		{
			QMessageBox::warning(this,tr("Error"),tr("Load database set definition of failure:")+err);
			return;
		}
		QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<m_dbsetsnm->currentText());
		it->setIcon(0,QPixmap(":/Resources/dbsets.png"));
		it->setData(0,Qt::UserRole,TreeTp_DbSet);
		it->setData(0,Qt::UserRole+6,m_rootPath);
		//it->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui.tree_db->addTopLevelItem(it);

		RTDbSets *dbsets = getRtDbDef(dbsetsname);
		vector<RTDbInfo>::iterator dbit= dbsets->rtdbs.begin();
		while(dbit != dbsets->rtdbs.end())
		{
			QTreeWidgetItem* dbitem = new QTreeWidgetItem(QStringList()<<QString::fromStdString(dbit->name));
			dbitem->setIcon(0,QPixmap(":/Resources/db.png"));
			dbitem->setData(0,Qt::UserRole,TreeTp_Db);
			dbitem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			it->addChild(dbitem);
			vector<tbldef>::iterator tblit= dbit->tbls.begin();
			while(tblit != dbit->tbls.end())
			{
				QTreeWidgetItem* tblitem = new QTreeWidgetItem(QStringList()<<QString::fromStdString(tblit->name));
				tblitem->setIcon(0,QPixmap(":/Resources/table.png"));
				tblitem->setData(0,Qt::UserRole,TreeTp_Tbl);
				tblitem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				dbitem->addChild(tblitem);
				tblit++;
			}
			dbit++;
		}
		
		//ui.tree_db->setItemDelegateForColumn(0,new VariantDelegate(this));
		//ui.tree_db->setEditTriggers(QAbstractItemView::AllEditTriggers);
	}
}
void RTDbDesiger::selectPath(bool flag)
{
	if(flag)
		m_rootPath = QFileDialog::getExistingDirectory(this, tr("Open RTDB"),	m_leRootPath->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
	if(m_rootPath.isEmpty())
		return;
	m_leRootPath->setText(m_rootPath);
	m_dbsetsnm->clear();
	QDir dir(m_rootPath+"\\dbdef");
	QStringList flnames = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
	foreach(QString fn,flnames)
		m_dbsetsnm->addItem(fn);
}
void RTDbDesiger::selectPath2()
{
	m_rootPath = QFileDialog::getExistingDirectory(this, tr("Open RTDB"),	m_leRootPath->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
	m_leRootPath->setText(m_rootPath);
}
void RTDbDesiger::contextMenu_treeddb ( const QPoint & pos )
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if(!item)
		return;
	int flag = item->data(0,Qt::UserRole).toInt();
	QMenu menu;
	if(flag == TreeTp_DbSet)
	{
		menu.addAction(tr("new database"),this,SLOT(add_db()));
		if(item->data(0,Qt::UserRole+1).toInt() == 1)
			menu.addAction(tr("save"),this,SLOT(save_dbsets()));
		menu.addAction(tr("rebuild dbsets"),this,SLOT(rebulid_dbsets()));
		menu.addSeparator();
		menu.addAction(tr("property"), this, SLOT(show_dbsets_property()));
		menu.addSeparator();
		menu.addAction(tr("close"),this,SLOT(close_dbsets()));
		menu.addAction(tr("delete dbsets"), this, SLOT(del_dbsets()));

	}
	else if(flag == TreeTp_Db)
	{
		menu.addAction(tr("new table"),this,SLOT(add_table()));
		if(item->data(0,Qt::UserRole+1).toInt() == 1)
			menu.addAction(tr("save and rebuild database"),this,SLOT(save_db()));
		menu.addAction(tr("rebuild database"),this,SLOT(rebulid_db()));
		menu.addSeparator();
		menu.addAction(tr("delete database"), this, SLOT(del_db()));
	}
	else if (flag == TreeTp_Tbl)
	{
		menu.addAction(tr("delete table"), this, SLOT(del_table()));
	}
	menu.exec(QCursor::pos());

}
void RTDbDesiger::close_dbsets()
{
	QTreeWidgetItem * it = ui.tree_db->currentItem();
	if(it)
	{
		int ind = ui.tree_db->currentIndex().row();
		ui.tree_db->clearSelection ();
		ui.tree_db->takeTopLevelItem(ind);
	}
}
void RTDbDesiger::on_tree_db_itemClicked(QTreeWidgetItem * item, int column)
{
	if (item->data(0,Qt::UserRole).toInt() == TreeTp_Tbl)
	{//打开数据库定义表
		QByteArray baTbl = item->text(0).toLatin1();
		QByteArray baDb = item->parent()->text(0).toLatin1();
		QByteArray baDbSet = item->parent()->parent()->text(0).toLatin1();
		char* tblnm = baTbl.data();
		char* dbnm = baDb.data();
		char* dbsetsnm = baDbSet.data();
		ui.tbl_data->setColumnCount(6);
		ui.tbl_data->setRowCount(0);
		ComboBoxDelegate* d = new ComboBoxDelegate(m_rootPath,this);
		ui.tbl_data->setItemDelegateForColumn(rec_dataTp,d);
		QStringList labs;
		RTDbSets *dbsets = getRtDbDef(dbsetsnm);
		if(dbsets)
		{
			vector<RTDbInfo>::iterator dbit= dbsets->rtdbs.begin();
			while(dbit != dbsets->rtdbs.end())
			{
				if(dbit->name == dbnm)
				{
					ui.tbl_data->setHorizontalHeaderLabels(QStringList()<<"name"<<"displa yname"<<"data type" << "length" << "data constraint" << "default value");
					QByteArray ba = m_rootPath.toLocal8Bit();
					map<int,dataTypeDef>& dataTp = getRtDbDataType(ba.data());
					vector<tbldef>::iterator tblit = dbit->tbls.begin();
					while(tblit != dbit->tbls.end())
					{
						if(tblit->name == tblnm)
						{
							ui.le_maxrownum->setText(QString::number(tblit->row_num));
							vector<coldef>::iterator colit = tblit->cols.begin();
							ui.tbl_data->setRowCount(tblit->cols.size());
							int r=0;
							while(colit != tblit->cols.end())
							{
								QTableWidgetItem* it = new QTableWidgetItem;
								it->setText(QString::fromStdString(colit->name));
								ui.tbl_data->setItem(r,rec_name,it);
								it = new QTableWidgetItem;
								it->setText(QString::fromLocal8Bit(colit->dispname.c_str()));
								ui.tbl_data->setItem(r,rec_dispName,it);
								it = new QTableWidgetItem;
								//it->setFlags(Qt::ItemIsSelectable);
								if(dataTp.find(colit->datatype) == dataTp.end())
									colit->datatype =0;
								it->setText(QString::fromStdString(dataTp[colit->datatype].name));
								it->setData(Qt::UserRole,colit->datatype);
								ui.tbl_data->setItem(r,rec_dataTp,it);
								it = new QTableWidgetItem;
								it->setText(QString::number(colit->tplen));
								ui.tbl_data->setItem(r,rec_len,it);
								it = new QTableWidgetItem;
								//it->setFlags(Qt::ItemIsSelectable);
								it->setText(QString::fromLocal8Bit(colit->data_constraint.c_str()));
								ui.tbl_data->setItem(r,rec_constraint,it);
								it = new QTableWidgetItem;
								it->setText(QString::fromLocal8Bit(colit->default_val.c_str()));
								ui.tbl_data->setItem(r,rec_default,it);
								//it = new QTableWidgetItem;
								//it->setText(QString::number(colit->dataProp));
								//ui.tbl_data->setItem(r,6,it);
								r++;
								colit++;
							}
							ui.tbl_data->resizeColumnsToContents();

							ui.btn_add->setEnabled(true);
							ui.btn_delete->setEnabled(true);
							ui.btn_ok->setEnabled(true);
							break;
						}
						tblit++;
					}
					break;
				}
				dbit++;
			}
		}
	}
	else
	{
		ui.tbl_data->setRowCount(0);
		ui.btn_add->setEnabled(false);
		ui.btn_delete->setEnabled(false);
		ui.btn_ok->setEnabled(false);
	}
}
void RTDbDesiger::on_act_createdbsets_triggered()
{
	QDialog dlg(this);
	dlg.move(this->rect().center());
	QVBoxLayout* vlay = new QVBoxLayout(&dlg);
	QHBoxLayout* hlay = new QHBoxLayout;
	m_leRootPath = new QLineEdit(m_rootPath);
	hlay->addWidget(m_leRootPath);
	QToolButton* btn = new QToolButton();
	btn->setText("...");
	hlay->addWidget(btn);
	vlay->addLayout(hlay);
	hlay = new QHBoxLayout;
	hlay->addWidget(new QLabel(tr("dbsets name:")));
	QLineEdit* dbsetsnm =new QLineEdit;
	hlay->addWidget(dbsetsnm);
	vlay->addLayout(hlay);
	hlay = new QHBoxLayout;
	hlay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton* btnok = new QPushButton(tr("OK"));
	connect(btnok,SIGNAL(clicked()),&dlg,SLOT(accept()));
	connect(btn,SIGNAL(clicked()),this,SLOT(selectPath2()));
	hlay->addWidget(btnok);
	vlay->addLayout(hlay);
	m_leRootPath->setText(m_rootPath);
	if(dlg.exec()== QDialog::Accepted)
	{
		QByteArray baDbSet = dbsetsnm->text().toLatin1();
		char* dbsetsname = baDbSet.data();
		if(strlen(dbsetsname)>0)
		{
			QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<dbsetsnm->text());
			it->setForeground(0,QBrush(QColor(255,0,0,128)));
			it->setIcon(0,QPixmap(":/Resources/dbsets.png"));
			it->setData(0,Qt::UserRole,TreeTp_DbSet);
			it->setData(0,Qt::UserRole+1,1);
			it->setData(0,Qt::UserRole+6,m_rootPath);
			ui.tree_db->addTopLevelItem(it);
			createRtDbDef(dbsetsname);
		}
	}
}
void RTDbDesiger::add_db()
{
	QTreeWidgetItem* dbsetsit = ui.tree_db->currentItem();
	if(!dbsetsit)
		return;
	QByteArray baDbSet = dbsetsit->text(0).toLatin1();
	char* dbsetsnm = baDbSet.data();
	RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
	if(rtdbsets)
	{
		RTDbInfo db;
		char str[32];
		sprintf(str,"db_%d",rtdbsets->rtdbs.size()+1);
		db.name = str;
		db.md5 = "";
		db.ver = 0;
		db.modifyTm = time(0);
		rtdbsets->rtdbs.push_back(db);
		rtdbsets->dbnums++;
		QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<str);
		it->setForeground(0,QBrush(QColor(255,0,0,128)));
		it->setIcon(0,QPixmap(":/Resources/db.png"));
		it->setData(0,Qt::UserRole,TreeTp_Db);
		it->setData(0,Qt::UserRole+1,1);
		it->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		dbsetsit->addChild(it);

		if(dbsetsit->data(0,Qt::UserRole+1).toInt()!=1)
		{
			dbsetsit->setForeground(0,QBrush(QColor(255,0,0,128)));
			dbsetsit->setData(0,Qt::UserRole+1,1);
		}
	}
}
void RTDbDesiger::del_db()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if (item->data(0, Qt::UserRole).toInt() == TreeTp_Db)
	{
		QByteArray baDbSet = item->parent()->text(0).toLatin1();
		QByteArray baDb = item->text(0).toLatin1();
		char* dbsetsnm = baDbSet.data();
		char* dbnm = baDb.data();
		QString basepath = item->parent()->data(0, Qt::UserRole + 6).toString();
		RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
		if (rtdbsets)
		{
			vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
			for (; dbit != rtdbsets->rtdbs.end(); dbit++)
			{
				if (dbit->name == dbnm)
				{
					rtdbsets->rtdbs.erase(dbit);
					item->parent()->setForeground(0, QBrush(QColor(255, 0, 0, 128)));
					item->parent()->setData(0, Qt::UserRole + 1, 1);
					item->parent()->removeChild(item);
					//删除定义和数据文件
					QFile fileTemp(basepath+"/dbdef/"+dbsetsnm+"/"+dbnm+".dbdef");
					fileTemp.remove();
					fileTemp.setFileName(basepath + "/include/" + dbsetsnm + "/header_" + dbnm + ".h");
					fileTemp.remove();
					fileTemp.setFileName(basepath + "/rtdb/" + dbsetsnm + "/" + dbnm + ".db");
					fileTemp.remove();
					fileTemp.setFileName(basepath + "/rtdb/" + dbsetsnm + "/" + dbnm + "_header.db");
					fileTemp.remove();
					return;
				}
			}
		}
	}
}
void RTDbDesiger::del_table()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if (item->data(0, Qt::UserRole).toInt() == TreeTp_Tbl)
	{
		QByteArray baDbSet = item->parent()->parent()->text(0).toLatin1();
		QByteArray baDb = item->parent()->text(0).toLatin1();
		QByteArray baTbl = item->text(0).toLatin1();
		char* dbsetsnm = baDbSet.data();
		char* dbnm = baDb.data();
		char* tblnm = baTbl.data();
		RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
		if (rtdbsets)
		{
			vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
			for (; dbit != rtdbsets->rtdbs.end(); dbit++)
			{
				if (dbit->name == dbnm)
				{
					std::vector<tbldef>::iterator tblit = dbit->tbls.begin();
					for (; tblit != dbit->tbls.end(); tblit++)
					{
						if (tblit->name == tblnm)
						{
							dbit->tbls.erase(tblit);
							item->parent()->setForeground(0, QBrush(QColor(255, 0, 0, 128)));
							item->parent()->setData(0, Qt::UserRole + 1, 1);
							item->parent()->removeChild(item);
							return;
						}
					}
				}
			}
		}
	}
}
void RTDbDesiger::del_dbsets()
{
	QTreeWidgetItem* dbsetsit = ui.tree_db->currentItem();
	QString dbsetsnm = dbsetsit->text(0);
	QString basepath = dbsetsit->data(0, Qt::UserRole + 6).toString();
	//删除定义文件
	QDir d;
	d.setPath(basepath + "/dbdef/" + dbsetsnm);
	d.removeRecursively();
	d.setPath(basepath + "/include/" + dbsetsnm);
	d.removeRecursively();
	d.setPath(basepath + "/rtdb/" + dbsetsnm);
	d.removeRecursively();
	close_dbsets();
}
void RTDbDesiger::treeItemChanged(QTreeWidgetItem *item)
{
	if((!item) || !m_flag)
		return;
	m_flag = false;
	int flag = item->data(0,Qt::UserRole).toInt();
	char* dbsetsnm,*dbnm;

	if(flag == TreeTp_Db)
	{
		QByteArray baDbSet = item->parent()->text(0).toLatin1();
		QByteArray baDb = item->text(0).toLatin1();
		dbsetsnm = baDbSet.data();
		dbnm = baDb.data();
		RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
		if(rtdbsets)
		{
			vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
			while(dbit!= rtdbsets->rtdbs.end())
			{
				if(dbit->name == dbnm)
				{
					item->setText(0,m_origName);
					QMessageBox::warning(this,tr("The system prompts"),tr("The name is a duplicate, please use a different name!"));
					return;
				}
				dbit++;
			}
			dbit = rtdbsets->rtdbs.begin();
			while(dbit!= rtdbsets->rtdbs.end())
			{
				if(dbit->name == m_origName.toStdString())
				{
					dbit->name = dbnm;
					break;
				}
				dbit++;
			}
			if(dbit == rtdbsets->rtdbs.end())
				return;
		}
		if(item->data(0,Qt::UserRole+1).toInt()!=1)
		{
			item->setForeground(0,QBrush(QColor(255,0,0,128)));
			item->setData(0,Qt::UserRole+1,1);
			item->parent()->setForeground(0,QBrush(QColor(255,0,0,128)));
			item->parent()->setData(0,Qt::UserRole+1,1);
		}
	}
	else if(flag == TreeTp_Tbl)
	{
		QByteArray baDbSet = item->parent()->parent()->text(0).toLatin1();
		QByteArray baDb = item->parent()->text(0).toLatin1();
		dbsetsnm = baDbSet.data();
		dbnm = baDb.data();
		RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
		if(rtdbsets)
		{
			vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
			while(dbit!= rtdbsets->rtdbs.end())
			{
				if(dbit->name == dbnm)
				{
					vector<tbldef>::iterator tblit = dbit->tbls.begin();
					while(tblit!= dbit->tbls.end())
					{
						if(tblit->name == dbnm)
						{
							item->setText(0,m_origName);
							QMessageBox::warning(this,tr("The system prompts"),tr("The name is a duplicate, please use a different name!"));
							return;
						}
						tblit++;
					}
					tblit = dbit->tbls.begin();
					while(tblit!= dbit->tbls.end())
					{
						if(tblit->name == m_origName.toStdString())
						{
							tblit->name = item->text(0).toStdString();
							break;
						}
						tblit++;
					}
					if(tblit== dbit->tbls.end())
						return;
					break;
				}
				dbit++;
			}
		}
		if(item->parent()->data(0,Qt::UserRole+1).toInt()!=1)
		{
			item->parent()->setForeground(0,QBrush(QColor(255,0,0,128)));
			item->parent()->setData(0,Qt::UserRole+1,1);
		}
	}
}
void RTDbDesiger::setOrigName ( QTreeWidgetItem *item)
{
	m_origName = item->text(0);
}
void RTDbDesiger::add_table()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	QByteArray baDbSet = item->parent()->text(0).toLatin1();
	QByteArray baDb = item->text(0).toLatin1();
	char* dbsetsnm = baDbSet.data();
	char* dbnm = baDb.data();
	RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
	if(rtdbsets)
	{
		vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
		while(dbit!= rtdbsets->rtdbs.end())
		{
			if(dbit->name == dbnm)
			{
				tbldef tbl;
				char str[32];
				sprintf(str,"Table_%d",dbit->tbls.size());
				tbl.name = str;
				tbl.row_num = 0;
				dbit->tbls.push_back(tbl);
				QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<str);
				it->setIcon(0,QPixmap(":/Resources/table.png"));
				it->setData(0,Qt::UserRole,TreeTp_Tbl);
				it->setData(0,Qt::UserRole+1,1);
				it->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				item->addChild(it);

				if(item->data(0,Qt::UserRole+1).toInt()!=1)
				{
					item->setForeground(0,QBrush(QColor(255,0,0,128)));
					item->setData(0,Qt::UserRole+1,1);
				}
				break;
			}
			dbit++;
		}
	}
}
void RTDbDesiger::save_dbsets()
{
	if (rtdb_has_load())
	{
		QMessageBox::warning(this, tr("Error"), tr("The RTDb has been opened and the RTDb closed."));
		return;
	}
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if(item->data(0,Qt::UserRole).toInt()!=TreeTp_DbSet)
		return;
	QByteArray baDb= item->text(0).toLatin1();
	char* dbsetsnm = baDb.data();
	char* basepath = item->data(0,Qt::UserRole+6).toString().toLocal8Bit().data();
	char err[256];
	verDbSets(getRtDbDef(dbsetsnm));
	if(!genDbSetsDefFile(basepath,getRtDbDef(dbsetsnm),err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Save dbsets definition file failed:")+err);
		return;
	}
	//重建数据库集
	if(!genDbSetsFile(basepath,dbsetsnm,err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Rebuilding the database failed:")+err);
		return;
	}
	if(item->data(0,Qt::UserRole+1).toInt()== 1)
	{
		item->setForeground(0,QBrush(Qt::black));
		item->setData(0,Qt::UserRole+1,0);
	}
}
void RTDbDesiger::save_db()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if(item->data(0,Qt::UserRole).toInt()!= TreeTp_Db)
		return;
	QByteArray baDb = item->text(0).toLatin1();
	QByteArray baDbSet = item->parent()->text(0).toLatin1();
	QByteArray bapath = item->parent()->data(0, Qt::UserRole + 6).toString().toLocal8Bit();
	char* dbnm = baDb.data();
	char* dbsetsnm = baDbSet.data();
	char* basepath = bapath.data();
	char err[256];
	RTDbSets* sets = getRtDbDef(dbsetsnm);
	if (!genDbDefFile(basepath, sets, dbnm, err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Save database definition file failed:")+err);
		return;
	}
	verDbSets(sets);
	//重建数据库
	if(!genDbFile(basepath,dbsetsnm,dbnm,err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Rebuilding the database failed:")+err);
		return;
	}
	if(item->data(0,Qt::UserRole+1).toInt()== 1)
	{
		item->setForeground(0,QBrush(Qt::black));
		item->setData(0,Qt::UserRole+1,0);
	}
}
void RTDbDesiger::on_act_saveall_triggered()
{
	if (rtdb_has_load())
	{
		QMessageBox::warning(this, tr("Error"), tr("The RTDb has been opened and the RTDb closed."));
		return;
	}
	for (int i=0; i<ui.tree_db->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* dbsetsit = ui.tree_db->topLevelItem(i);
		QByteArray baDbSet = dbsetsit->text(0).toLatin1();
		QByteArray bapath = dbsetsit->data(0, Qt::UserRole + 6).toString().toLocal8Bit();
		char* dbsetsnm = baDbSet.data();
		char* basepath = bapath.data();
		RTDbSets* sets = getRtDbDef(dbsetsnm);

		for (int j=0; j<dbsetsit->childCount(); j++)
		{
			QTreeWidgetItem* dbit = dbsetsit->child(j);
			if(dbit->data(0,Qt::UserRole+1).toInt()== 1)
			{
				QByteArray baDb = dbit->text(0).toLatin1();
				char* dbnm = baDb.data();
				char err[256];
				if(!genDbDefFile(basepath,getRtDbDef(dbsetsnm),dbnm,err))
				{
					QMessageBox::warning(this,tr("Error"),tr("Save database definition file failed:")+err);
					return;
				}
				//重建数据库
				if(!genDbFile(basepath,dbsetsnm,dbnm,err))
				{
					QMessageBox::warning(this,tr("Error"),tr("Rebuilding the database failed:")+err);
					return;
				}

				dbit->setForeground(0,QBrush(Qt::black));
				dbit->setData(0,Qt::UserRole+1,0);
			}
		}
		verDbSets(sets);
		//if (dbsetsit->data(0, Qt::UserRole + 1).toInt() == 1)
		{
			char err[256];
			if (!genDbSetsDefFile(basepath, sets, err))
			{
				QMessageBox::warning(this, tr("Error"), tr("Save DbSets definition file failed:") + err);
				return;
			}
			//重建数据库集
			if (!genDbSetsFile(basepath, dbsetsnm, err))
			{
				QMessageBox::warning(this, tr("Error"), tr("Rebuilding the database failed:") + err);
				return;
			}
			dbsetsit->setForeground(0, QBrush(Qt::black));
			dbsetsit->setData(0, Qt::UserRole + 1, 0);
		}
	}
}
void RTDbDesiger::rebulid_dbsets()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	QByteArray baDbSet = item->text(0).toLatin1();
	QByteArray baPath = item->data(0, Qt::UserRole + 6).toString().toLocal8Bit();
	char* dbsetsnm = baDbSet.data();
	char* basepath = baPath.data();
	char err[256];
	//重建实时库数据集
	if(!genDbSetsFile(basepath,dbsetsnm,err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Rebuilding the database failed:")+err);
		return;
	}
	for (int i=0; i<item->childCount(); i++)
		rebulid_db(item->child(i),false);
	QMessageBox::information(this,tr("Prompt"),tr("Rebuild the DbSets completed!"));
}
void RTDbDesiger::rebulid_db(QTreeWidgetItem * item,bool hasWg)
{
	if(!item)
		item = ui.tree_db->currentItem();	
	if(item->data(0,Qt::UserRole).toInt()!= TreeTp_Db)
		return;
	char err[256];
	QByteArray baDb = item->text(0).toLatin1();
	QByteArray baDbSet = item->parent()->text(0).toLatin1();
	QByteArray baPath = item->parent()->data(0, Qt::UserRole + 6).toString().toLocal8Bit();
	char* dbnm = baDb.data();
	char* dbsetsnm = baDbSet.data();
	char* basepath = baPath.data();
	if(!genDbFile(basepath,dbsetsnm,dbnm,err))
	{
		QMessageBox::warning(this,tr("Error"),tr("Rebuilding the database failed:")+err);
		return;
	}
	if(hasWg)
		QMessageBox::information(this,tr("Prompt"),tr("Rebuild the Db completed!"));
}
void RTDbDesiger::on_btn_add_clicked()
{
	int row = ui.tbl_data->rowCount();
	ui.tbl_data->insertRow(row);
	QTableWidgetItem* it = new QTableWidgetItem;
	it->setText("");
	ui.tbl_data->setItem(row,rec_name,it);
	it = new QTableWidgetItem;
	it->setText("");
	ui.tbl_data->setItem(row,rec_dispName,it);
	it = new QTableWidgetItem;
	//it->setFlags(Qt::ItemIsSelectable);
	it->setText("undefined");
	it->setData(Qt::UserRole,0);
	ui.tbl_data->setItem(row,rec_dataTp,it);
	it = new QTableWidgetItem;
	it->setText("1");
	ui.tbl_data->setItem(row,rec_len,it);
	it = new QTableWidgetItem;
	//it->setFlags(Qt::ItemIsSelectable);
	it->setText("");
	ui.tbl_data->setItem(row,rec_constraint,it);
	it = new QTableWidgetItem;
	it->setText("");
	ui.tbl_data->setItem(row,rec_default,it);
}
void RTDbDesiger::on_btn_delete_clicked()
{
	int row = ui.tbl_data->currentRow();
	ui.tbl_data->removeRow(row);
}
void RTDbDesiger::on_btn_ok_clicked()
{
	QTreeWidgetItem* item = ui.tree_db->currentItem();
	if(item->data(0,Qt::UserRole).toInt()!= TreeTp_Tbl)
		return;
	QByteArray baTbl = item->text(0).toLatin1();
	QByteArray baDb = item->parent()->text(0).toLatin1();
	QByteArray baDbSet = item->parent()->parent()->text(0).toLatin1();
	QByteArray baPath = item->parent()->parent()->data(0, Qt::UserRole + 6).toString().toLocal8Bit();
	char* tblnm = baTbl.data();
	char* dbnm = baDb.data();
	char* dbsetsnm = baDbSet.data();
	char* basepath = baPath.data();

	RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
	if(rtdbsets)
	{
		vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
		while(dbit!= rtdbsets->rtdbs.end())
		{
			if(dbit->name == dbnm)
			{
				vector<tbldef>::iterator tblit = dbit->tbls.begin();
				while(tblit!= dbit->tbls.end())
				{
					if(tblit->name == tblnm)
					{
						tblit->cols.clear();
						for(int r =0; r<ui.tbl_data->rowCount(); r++)
						{
							coldef col;
							QTableWidgetItem * item = ui.tbl_data->item(r,rec_name);
							col.name = item->text().toStdString();
							item = ui.tbl_data->item(r,rec_dispName);
							col.dispname = item->text().toLocal8Bit().data();
							item = ui.tbl_data->item(r,rec_dataTp);
							col.datatype = item->data(Qt::UserRole).toInt();
							item = ui.tbl_data->item(r,rec_len);
							col.tplen = item->text().toInt();
							if(col.tplen < 1)
								col.tplen = 1;
							item = ui.tbl_data->item(r,rec_constraint);
							col.data_constraint = item->text().toLocal8Bit().data();
							item = ui.tbl_data->item(r,rec_default);
							col.default_val = item->text().toLocal8Bit().data();
							tblit->cols.push_back(col);
						}
						tblit->row_num = ui.le_maxrownum->text().toInt();

						if(item->parent()->data(0,Qt::UserRole+1).toInt()!=1)
						{
							item->parent()->setForeground(0,QBrush(QColor(255,0,0,128)));
							item->parent()->setData(0,Qt::UserRole+1,1);
						}

						break;
					}
					tblit++;
				}
				break;
			}
			dbit++;
		}
	}
}
void RTDbDesiger::show_dbsets_property()
{
	QDialog dlg;
	Ui::DbSetsPropDlg uiProp;
	uiProp.setupUi(&dlg);
	QStandardItemModel mod;
	mod.setHorizontalHeaderLabels(QStringList() << tr("db_name") << tr("version") << tr("modify time") << tr("md5"));
	uiProp.tbl_prop->setModel(&mod);
	mod.setRowCount(0);
	mod.setColumnCount(4);
	QTreeWidgetItem* dbsetsit = ui.tree_db->currentItem();
	if (!dbsetsit)
		return;
	QByteArray baDbSet = dbsetsit->text(0).toLatin1();
	char* dbsetsnm = baDbSet.data();
	RTDbSets* rtdbsets = getRtDbDef(dbsetsnm);
	mod.insertRow(0);
	QStandardItem *item0 = new QStandardItem(QString::fromStdString(rtdbsets->dbsets));
	QStandardItem *item1 = new QStandardItem(QString::number(rtdbsets->ver));
	QStandardItem *item2 = new QStandardItem(QDateTime::fromTime_t(rtdbsets->modifyTm).toString("yyyy-MM-dd HH:mm:ss"));
	QStandardItem *item3 = new QStandardItem(QString::fromStdString(rtdbsets->md5));
	mod.setItem(0, 0, item0);
	mod.setItem(0, 1, item1);
	mod.setItem(0, 2, item2);
	mod.setItem(0, 3, item3);
	for (auto db:rtdbsets->rtdbs)
	{
		int r = mod.rowCount();
		mod.insertRow(r);
		item0 = new QStandardItem(QString::fromStdString(db.name));
		item1 = new QStandardItem(QString::number(db.ver));
		item2 = new QStandardItem(QDateTime::fromTime_t(db.modifyTm).toString("yyyy-MM-dd HH:mm:ss"));
		item3 = new QStandardItem(QString::fromStdString(db.md5));
		mod.setItem(r, 0, item0);
		mod.setItem(r, 1, item1);
		mod.setItem(r, 2, item2);
		mod.setItem(r, 3, item3);
	}
	dlg.exec();
}
void RTDbDesiger::on_tbl_data_doubleClicked(const QModelIndex & index)
{
	int row = index.row();
	QDialog dlg;
	Ui::Dlg_RecordDef uirecod;
	m_uirecod = &uirecod;
	uirecod.setupUi(&dlg);
	uirecod.le_name->setText(ui.tbl_data->item(row,rec_name)->text());
	uirecod.le_dispname->setText(ui.tbl_data->item(row,rec_dispName)->text());
	uirecod.le_len->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d*$"), this));//大于0整数
	uirecod.le_len->setText(ui.tbl_data->item(row,rec_len)->text());
	uirecod.le_default->setText(ui.tbl_data->item(row,rec_default)->text());
	QByteArray ba = m_rootPath.toLocal8Bit();
	map<int,dataTypeDef>& dataTp = getRtDbDataType(ba.data());
	map<int,dataTypeDef>::iterator it = dataTp.begin();
	while(it!=dataTp.end())
	{
		uirecod.comb_datatype->addItem(QString::fromStdString(it->second.name),it->first);
		it++;
	}
	connect(uirecod.comb_constraint, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comb_datatype_currentIndexChanged(int)));
	uirecod.comb_datatype->setCurrentIndex(uirecod.comb_datatype->findData(ui.tbl_data->item(row, rec_dataTp)->data(Qt::UserRole).toInt()));
	QStringList consStr = ui.tbl_data->item(row,rec_constraint)->text().split(QRegExp("[{}]"),QString::SkipEmptyParts);
	if(consStr.size()>0)
	{
		uirecod.comb_constraint->setCurrentIndex(uirecod.comb_constraint->findText(consStr[0]));
		if(consStr.size()>1)
			uirecod.txt_constraint->setText(consStr[1]);
	}
	else
		uirecod.comb_constraint->setCurrentIndex(0);
	if(dlg.exec() == QDialog::Accepted)
	{
		ui.tbl_data->item(row,rec_name)->setText(uirecod.le_name->text());
		ui.tbl_data->item(row,rec_dispName)->setText(uirecod.le_dispname->text());
		ui.tbl_data->item(row,rec_len)->setText(uirecod.le_len->text());
		ui.tbl_data->item(row,rec_default)->setText(uirecod.le_default->text());
		ui.tbl_data->item(row,rec_dataTp)->setText(uirecod.comb_datatype->currentText());
		ui.tbl_data->item(row,rec_dataTp)->setData(Qt::UserRole,uirecod.comb_datatype->itemData(uirecod.comb_datatype->currentIndex()));
		if(uirecod.comb_constraint->currentIndex()==0)
			ui.tbl_data->item(row,rec_constraint)->setText("");
		else
			ui.tbl_data->item(row,rec_constraint)->setText(QString("%1{%2}").arg(uirecod.comb_constraint->currentText()).arg(uirecod.txt_constraint->toPlainText()));
	}
}
void RTDbDesiger::on_comb_datatype_currentIndexChanged(int index)
{
	Ui::Dlg_RecordDef* ui = (Ui::Dlg_RecordDef*)m_uirecod;
	//if (!ui->txt_constraint->toPlainText().isEmpty())
	{
		switch (index)
		{
			case 0://none
				break;
			case 1://CheckBox
				ui->txt_constraint->setText(QStringLiteral("是:1,否:0"));
				break;
			case 2://Val_range
				ui->txt_constraint->setText(QStringLiteral("min_val,max_val"));
				break;
			case 3://Enum
				ui->txt_constraint->setText(QStringLiteral("Val1:1,Val2:2,Val3:3"));
				break;
			case 4://Flag
				ui->txt_constraint->setText(QStringLiteral("Val1:1,Val2:2,Val3:3"));
				break;
			case 5://Reference
				ui->txt_constraint->setText(QStringLiteral("数据库名,表名,显示列"));
				break;
			default:
				break;
		}
	}
}