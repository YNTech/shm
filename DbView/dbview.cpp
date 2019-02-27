#include <QtWidgets>
#include "dbview.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
//#include "shm_interface.h"
#include "mytablewidget.h"
using namespace std;
const int pagerownum = 20;

DbView::DbView(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags),m_actgroup(this),m_actedit(nullptr)
{
	ui.setupUi(this);
	m_rootPath="../db";
	m_maxPages = 0;
	ui.btn_pagepre->setEnabled(false);
	ui.btn_pagenext->setEnabled(false);

	connect(ui.tbl_data, SIGNAL(customContextMenuRequested ( const QPoint&)), this, SLOT(contextMenu ( const QPoint &)));
	connect(ui.tree_db, SIGNAL(customContextMenuRequested ( const QPoint&)), this, SLOT(contextMenu_treeddb ( const QPoint &)));
	connect(ui.tbl_data, SIGNAL(onEditValue ( QString)), this, SLOT(datachanged ( QString)));
}

DbView::~DbView()
{
	for(int i=0; i<ui.tree_db->topLevelItemCount(); i++)
	{
		QString dbsetsnm  = ui.tree_db->topLevelItem(i)->text(0);
		char err[256];
		if(!closeRTDb(dbsetsnm.toLocal8Bit().data(),err))
			QMessageBox::warning(this,tr("Error"),tr("Unload DbSets failure:")+err);
	}
}
void DbView::on_act_opendbsets_triggered()
{
	//QDir dir(m_rootPath+"\\rtdb");
	//QStringList flnames = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
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
	//foreach(QString fn,flnames)
	//	m_dbsetsnm->addItem(fn);
	if(dlg.exec()== QDialog::Accepted)
	{
		char err[256];
		QByteArray baDbSet = m_dbsetsnm->currentText().toLatin1();
		char* dbsets = baDbSet.data();
		if(!loadRTDb(dbsets, m_rootPath.toLocal8Bit().data(), err))
		{
			QMessageBox::warning(this,tr("Error"),tr("Load DbSets faild:")+err);
			return;
		}
		QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<m_dbsetsnm->currentText());
		it->setIcon(0,QPixmap(":/Resources/dbsets.png"));
		it->setData(0,Qt::UserRole,2);
		ui.tree_db->addTopLevelItem(it);
		char (*dbnms)[db_name_length];
		int dbnums = 0;
		if (!rtdb_get_dbnames(&dbnms, dbnums, err, dbsets))
		{
			QMessageBox::warning(this,tr("Error"),tr("Open DbSets faild:")+err);
			return;
		}
		for (int i = 0; i < dbnums; i++)
		{
			QTreeWidgetItem* dbit = new QTreeWidgetItem(QStringList() << QString::fromStdString(dbnms[i]));
			dbit->setIcon(0,QPixmap(":/Resources/db.png"));
			it->addChild(dbit);
			char* tblnms[max_tables_num];
			int tblnum = 0;
			if (rtdb_get_tblnames(tblnms, tblnum, dbnms[i], err, dbsets))
			{
				for (int j = 0; j < tblnum; j++)
				{
					QTreeWidgetItem* tblit = new QTreeWidgetItem(QStringList() << QString::fromStdString(tblnms[j]));
					tblit->setIcon(0,QPixmap(":/Resources/table.png"));
					tblit->setData(0,Qt::UserRole,1);
					dbit->addChild(tblit);
				}
			}
			else
				QMessageBox::warning(this,tr("Error"),tr("Load DB table faild:")+err);
		}
	}
}
void DbView::on_tree_db_itemClicked(QTreeWidgetItem * item, int column)
{
	m_maxPages = 0;
	ui.btn_pagepre->setEnabled(false);
	ui.btn_pagenext->setEnabled(false);
	colsizes.clear();
	setTblEditable(false);
	int flag = item->data(0,Qt::UserRole).toInt();
	QString errString;
	bool ret = true;
	switch (flag)
	{
	case 1://打开数据库表
		{
			QByteArray baTbl = item->text(0).toLatin1();
			QByteArray baDb = item->parent()->text(0).toLatin1();
			QByteArray baDbSet = item->parent()->parent()->text(0).toLatin1();
			char* tblnm = baTbl.data();
			char* dbnm = baDb.data();
			char* dbsets = baDbSet.data();

			ui.tbl_data->setColumnCount(0);
			//ui.tbl_data->setRowCount(0);
			QStringList labs;
			labs<<"ID";
			//const char* pinfo = (char*)(shm_data(dbsets + "_"+ dbnm+"_header"));
			const TYPE_header_RTDb pinfo = (TYPE_header_RTDb)header_rtdb(dbsets, dbnm);
			if(pinfo == nullptr)
			{
				errString = tr("The DB head_partition not load:") + rtdb_lastError();
				ret = false;
				break;
			}
			//m_dbData = (char*)(shm_data(dbsets + "_"+ dbnm));
			m_dbData = (char*)data_rtdb(dbsets, dbnm);
			if(m_dbData == nullptr)
			{
				errString = tr("The DB not load:") + rtdb_lastError();
				ret = false;
				break;
			}
			m_dbHeader = (const char*)pinfo;
			unsigned short tbl_num = *((unsigned short*)m_dbHeader);
			m_dbHeader+= 2;
			unsigned int coladdr = 0;
			ShmDbTblHeaderdef* header;
			for (int i=0; i<tbl_num; i++)
			{
				header = (ShmDbTblHeaderdef*)m_dbHeader;
				header->tbl_name[tbl_name_length-1] = '\0';
				if(strncmp(header->tbl_name, tblnm, tbl_name_length-1) == 0)
				{
					coladdr = header->tbl_coldef_addr;
					ui.tbl_data->setColumnCount(header->col_num+1);
					ShmDbColDef* col = (ShmDbColDef*)((const char*)pinfo + coladdr);
					for (int i=0; i<header->col_num; i++)
					{
						col[i].col_name[col_name_length-1] = '\0';
						labs << col[i].col_name;
						colsizes<< col[i].col_size;
					}
					ui.tbl_data->setHorizontalHeaderLabels(labs);
					//ui.tbl_data->setRowCount(header->row_num);

					m_maxPages = header->max_row_num / pagerownum + (header->max_row_num % pagerownum == 0?0:1);
					ui.lab_allpage->setText(QString::number(m_maxPages));
					ui.le_curpage->setText("1");
					ui.btn_pagepre->setEnabled(false);
					if(m_maxPages>1)
						ui.btn_pagenext->setEnabled(true);
					else
						ui.btn_pagenext->setEnabled(false);
					//npages = 0;
					showPage(0);
//
					ret = true;
					break;
				}
				m_dbHeader+= sizeof(ShmDbTblHeaderdef);
			}
			if(coladdr == 0 )
			{
				errString = tr("Not found the table:")+tblnm;
				ret = false;
				break;
			}
		}
		break;
	default:
		;
	}
	if(!ret)
		QMessageBox::warning(this,tr("Error"),tr("Load DB table faild:")+errString);
}
void DbView::showPage(int npage)
{
	QTableWidgetItem * dit;
	int row = 0;
	ShmDbTblHeaderdef* header = (ShmDbTblHeaderdef*)m_dbHeader;
	int rows = qMin(pagerownum,(const int)(header->max_row_num-npage*pagerownum));
	ui.tbl_data->setRowCount(rows);
	for (int r=0,row = npage*pagerownum; r<rows; r++,row++)
	{
		if(!ui.tbl_data->verticalHeaderItem(r))
			ui.tbl_data->setVerticalHeaderItem(r, new QTableWidgetItem());
		if(row<header->row_num)
			ui.tbl_data->verticalHeaderItem(r)->setText(QString(">%1").arg(r+1));
		else
			ui.tbl_data->verticalHeaderItem(r)->setText(QString("%1").arg(r+1));
		char* rowdata = m_dbData + header->tbl_address + row* header->row_size;
		if(!ui.tbl_data->item(r,0))
			ui.tbl_data->setItem(r,0,new QTableWidgetItem());
		dit = ui.tbl_data->item(r,0);
		dit->setText(QString::number(row));
		dit->setFlags(Qt::ItemIsEnabled);
		dit->setBackgroundColor(Qt::lightGray);
		for (int c=0; c<colsizes.size(); c++)
		{
			if(!ui.tbl_data->item(r,c+1))
				ui.tbl_data->setItem(r,c+1,new QTableWidgetItem());
			dit = ui.tbl_data->item(r,c+1);
			QByteArray rawdata = QByteArray::fromRawData(rowdata,colsizes[c]);
			//QString str(rawdata);
			//if (str.isEmpty())
				dit->setText(rawdata.toHex().toUpper());
			//else
			//	dit->setText(str);
			dit->setData(Qt::UserRole,rawdata);
			dit->setData(Qt::UserRole+1,VIEW_HEX);
			dit->setData(Qt::UserRole+2,(int)rowdata);
			//dit->setData(Qt::UserRole+3,colsizes[c]);
			//ui.tbl_data->setItem(r,c+1,dit);
			rowdata += colsizes[c];
		}
	}
}
void DbView::on_btn_pagepre_clicked()
{
	int page = ui.le_curpage->text().toUInt()-1;
	if(page <1)
	{
		page = 1;
		ui.btn_pagepre->setEnabled(false);
		//return;
	}
	ui.le_curpage->setText(QString::number(page));
	if(!ui.btn_pagenext->isEnabled())
		ui.btn_pagenext->setEnabled(true);
	showPage(page-1);
}
void DbView::on_btn_pagenext_clicked()
{
	int page = ui.le_curpage->text().toUInt()+1;
	if(page >m_maxPages)
	{
		page = m_maxPages;
		//return;
	}
	ui.le_curpage->setText(QString::number(page));
	if(page == m_maxPages)
		ui.btn_pagenext->setEnabled(false);
	if(page == 1)
		ui.btn_pagepre->setEnabled(false);
	else
		ui.btn_pagepre->setEnabled(true);
	showPage(page-1);
}
void DbView::contextMenu_treeddb ( const QPoint & pos )
{
	if(ui.tree_db->currentItem() && ui.tree_db->currentItem()->data(0,Qt::UserRole).toInt() == 2)
	{
		QMenu menu;
		menu.addAction(tr("Close"),this,SLOT(close_dbsets()));
		menu.exec(QCursor::pos());
	}
}
void DbView::close_dbsets()
{
	QTreeWidgetItem * it = ui.tree_db->currentItem();
	if(it)
	{
		QString dbsetsnm = it->text(0);
		char err[256];
		if(!closeRTDb(dbsetsnm.toLocal8Bit().data(),err))
			QMessageBox::warning(this,tr("Error"),tr("Unload DbSets failure:")+err);
		int ind = ui.tree_db->currentIndex().row();
		ui.tree_db->clearSelection ();
		ui.tree_db->takeTopLevelItem(ind);
	}
}
void DbView::contextMenu ( const QPoint & pos )
{
	
	QList<QTableWidgetItem*> items = ui.tbl_data->selectedItems();
	if(items.size()>0 && m_tblmenu.isEmpty())
	{
		QStringList strs = QStringList()<<"Hex"<<"Text"<<"Int"<<"Float"<<"Double";
		QList<int> ntps = QList<int>()<<VIEW_HEX<<VIEW_TEXT<<VIEW_INT<<VIEW_FLOAT<<VIEW_DOUBLE;
		QAction * act;
		for (int i=0; i< strs.size(); i++)
		{
			act =new QAction(strs[i],&m_tblmenu);
			act->setCheckable(true);
			act->setData(ntps[i]);
			m_tblmenu.addAction(act);
			m_actgroup.addAction(act);
			connect(act,SIGNAL(triggered(bool )),this, SLOT(showItemData(bool)));
		}
		m_tblmenu.addSeparator();
		m_actedit =new QAction(tr("Edit"),&m_tblmenu);
		m_actedit->setCheckable(true);
		m_tblmenu.addAction(m_actedit);
		connect(m_actedit,SIGNAL(triggered(bool)),this, SLOT(setTblEditable(bool)));
	}
	m_tblmenu.exec(QCursor::pos());
}
void DbView::showItemData(bool flag)
{
	QAction * act = qobject_cast<QAction *>(sender ());
	if(act)
	{
		act->setChecked(flag);

		QList<QTableWidgetItem*> items = ui.tbl_data->selectedItems();
		switch(act->data().toInt())
		{
			case VIEW_HEX:
				foreach(QTableWidgetItem* it,items)
				{
					if(it->data(Qt::UserRole+1).toInt() == VIEW_HEX)
						continue;
					it->setText(it->data(Qt::UserRole).toByteArray().toHex());
					it->setData(Qt::UserRole+1,VIEW_HEX);
				}
				break;
			case VIEW_TEXT:
				foreach(QTableWidgetItem* it,items)
				{
					if(it->data(Qt::UserRole+1).toInt() == VIEW_TEXT)
						continue;
					QByteArray data = it->data(Qt::UserRole).toByteArray();
					for (int i=0; i<data.size(); i++)
					{
						if((unsigned int)data.at(i) == 0)
							break;
						if((unsigned int)data.at(i) < 32)
							data[i] = 0x2e;
					}
					it->setText(QString::fromLocal8Bit(data));
					it->setData(Qt::UserRole+1,VIEW_TEXT);
				}
				break;
			case VIEW_INT:
				foreach(QTableWidgetItem* it,items)
				{
					if(it->data(Qt::UserRole+1).toInt() == VIEW_INT)
						continue;
					int n = *(int*)(it->data(Qt::UserRole).toByteArray().constData());
					it->setText(QString::number(n));
					it->setData(Qt::UserRole+1,VIEW_INT);
				}
				break;
			case VIEW_FLOAT:
				foreach(QTableWidgetItem* it,items)
				{
					if(it->data(Qt::UserRole+1).toInt() == VIEW_FLOAT)
						continue;
					float data = *(float*)(it->data(Qt::UserRole).toByteArray().constData());
					it->setText(QString::number(data));
					it->setData(Qt::UserRole+1,VIEW_FLOAT);
				}
				break;
			case VIEW_DOUBLE:
				foreach(QTableWidgetItem* it,items)
				{
					if(it->data(Qt::UserRole+1).toInt() == VIEW_DOUBLE)
						continue;
					double data = *(double*)(it->data(Qt::UserRole).toByteArray().constData());
					it->setText(QString::number(data));
					it->setData(Qt::UserRole+1,VIEW_DOUBLE);
				}
				break;
			default:
				;
		}
	}
}
void DbView::selectPath(bool flag)
{
	if(flag)
		m_rootPath = QFileDialog::getExistingDirectory(this, tr("Open RTDB"),	m_leRootPath->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
	if(m_rootPath.isEmpty())
		return;
	m_leRootPath->setText(m_rootPath);
	m_dbsetsnm->clear();
	QDir dir(m_rootPath+"\\rtdb");
	QStringList flnames = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
	foreach(QString fn,flnames)
		m_dbsetsnm->addItem(fn);
}
void DbView::datachanged(QString strdata)
{
	if (m_isEditable) 
	{
		QTableWidgetItem * it = ui.tbl_data->currentItem();
		if(it)
		{
			int tp = it->data(Qt::UserRole+1).toInt();
			char* pshmdata = (char *)(it->data(Qt::UserRole+2).toInt());
			switch(tp)
			{
			case VIEW_HEX:
				{
					QByteArray ndata = QByteArray::fromHex(strdata.toLatin1());
					int nszie= qMin(it->data(Qt::UserRole).toByteArray().size(),ndata.size());
					memcpy(pshmdata,ndata.constData(),nszie);
					it->setData(Qt::UserRole,QByteArray::fromRawData(pshmdata,nszie));
				}
				break;
			case VIEW_TEXT:
				{
					QByteArray ba = strdata.toLocal8Bit();
					int nszie= qMin(it->data(Qt::UserRole).toByteArray().size(),ba.size());
					memcpy(pshmdata,ba.data(),nszie);
					it->setData(Qt::UserRole,QByteArray::fromRawData(pshmdata,nszie));
				}
				break;
			case VIEW_INT:
				{
					int nszie= qMin(it->data(Qt::UserRole).toByteArray().size(),4);
					int data = strdata.toInt();
					memcpy(pshmdata,&data,nszie);
					it->setData(Qt::UserRole,QByteArray::fromRawData(pshmdata,nszie));
				}
				break;
			case VIEW_FLOAT:
				{
					int nszie= qMin(it->data(Qt::UserRole).toByteArray().size(),4);
					float data = strdata.toFloat();
					memcpy(pshmdata,&data,nszie);
					it->setData(Qt::UserRole,QByteArray::fromRawData(pshmdata,nszie));
				}
				break;
			case VIEW_DOUBLE:
				{
					int nszie= qMin(it->data(Qt::UserRole).toByteArray().size(),8);
					double data = strdata.toDouble();
					memcpy(pshmdata,&data,nszie);
					it->setData(Qt::UserRole,QByteArray::fromRawData(pshmdata,nszie));
				}
				break;
			default:
				;
			}
		}
	}
}
void DbView::setTblEditable(bool f)
{
	if(m_actedit)
		m_actedit->setChecked(f);
	m_isEditable =f;
	if(f)
		ui.tbl_data->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::AnyKeyPressed);
	else
		ui.tbl_data->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void myTableWidget::commitData ( QWidget * editor )
{
	QTableWidget::commitData(editor);
	QLineEdit* leVal = qobject_cast<QLineEdit*>(editor);
	if(leVal)
		emit onEditValue(leVal->text());
}