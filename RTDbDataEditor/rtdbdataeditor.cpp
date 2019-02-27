#include <QtGui>
#include "rtdbdataeditor.h"
#include "rtdb_api.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
//#include "shm_interface.h"
#include "dataeditordelegate.h"
using namespace std;
const int pagerownum = 20;

QMap<int,int> g_indexTmp;

RTDbDataEditor::RTDbDataEditor(QString basepath, QWidget *parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags), m_indflag(false), m_basepath(basepath)
{
	ui.setupUi(this);
	m_maxPages = 0;
	ui.btn_pagepre->setEnabled(false);
	ui.btn_pagenext->setEnabled(false);
	ui.btn_saveind->setVisible(false);
	//connect(ui.tbl_data, SIGNAL(onEditValue ( QString)), this, SLOT(datachanged ( QString)));

	//////////////////////////////////////////////////////////////////////////
	char err[256];
	char (*dbnames)[db_name_length];
	int dbnums = 0;
	if (!rtdb_get_dbnames(&dbnames, dbnums, err))
	{
		QMessageBox::warning(this,tr("Error"),QString(tr("Open the DbSets failure,Please verify that the DbSets<%1>Loaded:%2")).arg(getDefaultDbSets()).arg(err));
		exit(-1);
	}
	if(!readRTDbSetsDef(basepath.toLocal8Bit().data(),getDefaultDbSets()))
		QMessageBox::warning(this,tr("Error"),QString(tr("Open the database<%1>definition file failed:%2")).arg(getDefaultDbSets()).arg(err));
	for (int i = 0; i < dbnums;i++)
	{
		QTreeWidgetItem* dbit = new QTreeWidgetItem(QStringList() << QString::fromStdString(dbnames[i]));
		dbit->setIcon(0,QPixmap(":/Resources/db.png"));
		dbit->setData(0,Qt::UserRole,2);
		ui.tree_db->addTopLevelItem(dbit);
		char* tblnms[max_tables_num];
		int tblnum = 0;
		if (rtdb_get_tblnames(tblnms, tblnum, dbnames[i], err))
		{
			for (int i = 0; i < tblnum;i++)
			{
				QTreeWidgetItem* tblit = new QTreeWidgetItem(QStringList() << QString(tblnms[i]));
				tblit->setIcon(0,QPixmap(":/Resources/table.png"));
				tblit->setData(0,Qt::UserRole,1);
				dbit->addChild(tblit);
			}
		}
		else
			QMessageBox::warning(this,tr("Error"),tr("Load database table failure:")+err);
	}

}

RTDbDataEditor::~RTDbDataEditor()
{
}
void RTDbDataEditor::on_tree_db_itemClicked(QTreeWidgetItem * item, int column)
{
	m_maxPages = 0;
	ui.btn_pagepre->setEnabled(false);
	ui.btn_pagenext->setEnabled(false);
	colsizes.clear();
	ui.tbl_data->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::AnyKeyPressed);
	int flag = item->data(0,Qt::UserRole).toInt();
	QString errString;
	bool ret = true;
	switch (flag)
	{
	case 1://打开数据库表
		{
			QByteArray baTbl = item->text(0).toLatin1();
			QByteArray baDb = item->parent()->text(0).toLatin1();
			char* tblnm = baTbl.data();
			char* dbnm = baDb.data();
			const char* dbsets = getDefaultDbSets();

			ui.tbl_data->setColumnCount(0);
			//ui.tbl_data->setRowCount(0);
			DataEditorDelegate* d = new DataEditorDelegate(this);
			ui.tbl_data->setItemDelegate(d);
			//QItemEditorFactory *factory = new QItemEditorFactory;
			//QItemEditorCreatorBase *pixCreator =new QStandardItemEditorCreator<LabelIcon>();
			//factory->registerEditor(QVariant::Pixmap, pixCreator);
			//QItemEditorFactory::setDefaultFactory(factory);

			QStringList labs;
			labs<<"ID";
			//const TYPE_header_RTDb pinfo = (TYPE_header_RTDb)(shm_data(string(dbsets) + "_"+ dbnm+"_header"));
			const TYPE_header_RTDb pinfo = (TYPE_header_RTDb)header_rtdb(dbsets, dbnm);
			if(pinfo == nullptr)
			{
				errString = tr("The DB not load:") + rtdb_lastError();
				ret = false;
				break;
			}
			//m_dbData = (char*)(shm_data(string(dbsets) + "_" + dbnm));
			m_dbData = (char*)data_rtdb(dbsets, dbnm);
			if(m_dbData == nullptr)
			{
				errString = tr("The DB not load:") + rtdb_lastError();
				ret = false;
				break;
			}
			m_dbHeader = (char*)pinfo->tbl_header;
			unsigned short tbl_num = pinfo->tbl_nums;
			unsigned int coladdr = 0;
			ShmDbTblHeaderdef* header;

			RTDbSets* rtdbsets = getRtDbDef(dbsets);
			std::vector<RTDbInfo>::iterator dbit = rtdbsets->rtdbs.begin();
			while(dbit!=rtdbsets->rtdbs.end())
			{
				if(dbit->name == dbnm)
				{
					std::vector<tbldef>::iterator tblit = dbit->tbls.begin();
					while(tblit!=dbit->tbls.end())
					{
						if(tblit->name == tblnm)
						{
							m_curTblDef = tblit;
							break;
						}
						tblit++;
					}
					break;
				}
				dbit++;
			}

			for (int i=0; i<tbl_num; i++)
			{
				header = (ShmDbTblHeaderdef*)m_dbHeader;
				header->tbl_name[tbl_name_length-1] = '\0';
				if(strncmp(header->tbl_name, tblnm, tbl_name_length-1) == 0)
				{
					coladdr = header->tbl_coldef_addr;
					ui.tbl_data->setColumnCount(header->col_num+1);
					IndexDelegate* inddele = new IndexDelegate(this);
					ui.tbl_data->setItemDelegateForColumn(0,inddele);
					ShmDbColDef* col = (ShmDbColDef*)((char*)pinfo+coladdr);
					std::vector<coldef>::iterator colit = m_curTblDef->cols.begin();
					int i=0;
					while(colit!=m_curTblDef->cols.end() && i<header->col_num)
					{
						QString strlab;
						if(colit->dispname.empty())
							strlab = QString::fromLocal8Bit(colit->name.c_str());
						else
							strlab = QString::fromLocal8Bit(colit->dispname.c_str());
						QStringList strs = QString::fromLocal8Bit(colit->data_constraint.c_str()).split(QRegExp("[{:,}]"),QString::SkipEmptyParts);
						if(strs.size()>2 && strs[0] == "Val_range")
							strlab += QString("(%1~%2)").arg(strs[1]).arg(strs[2]);
						labs << strlab;
						colsizes<< col[i].col_size;
						colit++;
						i++;
					}
					ui.tbl_data->setHorizontalHeaderLabels(labs);
					//ui.tbl_data->setRowCount(header->row_num);

					m_maxPages = header->row_num / pagerownum + (header->row_num % pagerownum == 0?0:1);
					ui.lab_allpage->setText(QString::number(m_maxPages));
					ui.le_curpage->setText("1");
					ui.le_curpage->setValidator(new QRegExpValidator(QRegExp("^\\d+(\\d+)?$"), this));
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
		QMessageBox::warning(this,tr("Error"),tr("Error loading data table:")+errString);
}
void RTDbDataEditor::showPage(int npage)
{
	QTableWidgetItem * dit;
	int row = 0;
	ShmDbTblHeaderdef* header = (ShmDbTblHeaderdef*)m_dbHeader;
	int rows = qMin(pagerownum,(const int)(header->row_num-npage*pagerownum));
	if(rows<0)
		rows = 0;
	if(rows > header->max_row_num)
		rows = header->max_row_num;
	ui.tbl_data->setRowCount(rows);
	QByteArray ba = m_basepath.toLocal8Bit();
	map<int, dataTypeDef>& dataTp = getRtDbDataType(ba.data());
	for (int r=0,row = npage*pagerownum; r<rows; r++,row++)
	{
		char* rowdata = m_dbData + header->tbl_address + row* header->row_size;
		if(!ui.tbl_data->item(r,0))
			ui.tbl_data->setItem(r,0,new QTableWidgetItem());
		dit = ui.tbl_data->item(r,0);
		dit->setText(QString::number(row));
		if(!m_indflag)
		{
			dit->setFlags(Qt::ItemIsEnabled);
			dit->setBackgroundColor(Qt::lightGray);
		}
		else
		{
			dit->setFlags(Qt::ItemIsEnabled |Qt::ItemIsSelectable | Qt::ItemIsEditable);
			dit->setBackgroundColor(Qt::white);
			dit->setData(Qt::UserRole,row);
			//g_indexTmp[row] =row;
		}
		std::vector<coldef>::iterator colit = m_curTblDef->cols.begin();
		int c=1;
		while(colit!=m_curTblDef->cols.end() && c<header->col_num+1)
		{
			if(!ui.tbl_data->item(r,c))
				ui.tbl_data->setItem(r,c,new QTableWidgetItem());
			dit = ui.tbl_data->item(r,c);
			if(dataTp.find(colit->datatype)!=dataTp.end())
			{
				dit->setData(Qt::UserRole,(int)rowdata);
				dit->setData(Qt::UserRole+1,colit->datatype);
				dit->setData(Qt::UserRole+2,QString::fromLocal8Bit(colit->data_constraint.c_str()));
				dit->setData(Qt::UserRole+3,colit->tplen);
				int nval;
				switch(colit->datatype)
				{
				case dataTp_int://int
				case dataTp_short://short
				case dataTp_char://char
					{
						if(colit->datatype == dataTp_int)
							nval = *((int*)(rowdata));
						else if(colit->datatype == dataTp_short)
							nval = *((short*)(rowdata));
						else
							nval = (int)(*rowdata);
						QStringList strs = QString::fromLocal8Bit(colit->data_constraint.c_str()).split(QRegExp("[{:,}]"),QString::SkipEmptyParts);
						if(strs.size()>0)
						{
							if(strs[0] == "CheckBox")
							{
								if(strs.size()>4)
								{
									if(nval == strs[2].toInt())
										dit->setText(strs[1]);
									else
										dit->setText(strs[3]);
								}
								break;
							}
							else if(strs[0] == "Enum")
							{
								dit->setText("");
								for(int i=1; i<strs.size()-1; i++)
								{
									if(nval == strs[i+1].toInt())
									{
										dit->setText(strs[i]);
										break;
									}
								}
								break;
							}
							else if(strs[0] == "Flag")
							{
								QStringList lst;
								for(int i=1; i<strs.size()-1; i++)
								{
									if(nval & strs[i+1].toInt())
										lst << strs[i];
								}
								dit->setText(lst.join("|"));
								break;
							}
							else if(strs[0] == "Reference")
							{
								dit->setText("");
								if(strs.size()<2)
									break;
								if(strs[1] == "Db")
								{
									char (*dbnames)[db_name_length];
									int dbnums = 0;
									rtdb_get_dbnames(&dbnames, dbnums);
									if (nval >= dbnums)
										break;///<error
									dit->setText(dbnames[nval]);
									break;
								}
								else 
								{
									if(strs.size()<3)
										break;
									if(strs[2] == "Table")
									{
										//std::list<std::string> tblnames;
										char* tblnms[max_tables_num];
										int tblnum = 0;
										rtdb_get_tblnames(tblnms, tblnum, strs[1].toLocal8Bit().data());
										if (nval >= tblnum)
											break;///<error
										dit->setText(tblnms[nval]);
										break;
									}
									else
									{
										if(strs.size()<4)
											break;
										int bgrow = nval / 1000;
										int valnum = 1000;
										void* vals[1000];
										int datalen;
										rtdb_get_colsdata(strs[1].toLocal8Bit().data(), strs[2].toLocal8Bit().data(),strs[3].toLocal8Bit().data(),bgrow,valnum,vals,datalen);
										dit->setText(QString::fromLocal8Bit((char*)(vals[nval-bgrow])));
										break;
									}
								}
							}
						}
						dit->setText(QString::number(nval));
					}
					break;
				case dataTp_float://float
					dit->setText(QString::number(*((float*)(rowdata))));
					break;
				case dataTp_double://double
					dit->setText(QString::number(*((double*)(rowdata))));
					break;
				case dataTp_string://string
					dit->setText(QString::fromLocal8Bit(rowdata));//,dataTp[colit->datatype].nsize * colit->tplen));
					break;
				case dataTp_varbtye://varByte
					dit->setText("");
					dit->setBackgroundColor(Qt::lightGray);
					dit->setFlags(Qt::ItemIsEnabled);
					break;
				case dataTp_image://image
					{
						QByteArray ba(rowdata,dataTp[colit->datatype].nsize * colit->tplen);
						QPixmap ico;
						QDataStream sout(&ba,QIODevice::ReadOnly);
						sout>>ico;
						dit->setIcon(ico);
						//dit->setData(Qt::EditRole,ico);
					}
					break;
				default:
					dit->setText(QByteArray::fromRawData(rowdata,dataTp[colit->datatype].nsize * colit->tplen).toHex().toUpper());
				}
				rowdata += dataTp[colit->datatype].nsize * colit->tplen;
			}

			colit++;
			c++;
		}
	}
}
void RTDbDataEditor::on_btn_pagepre_clicked()
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
void RTDbDataEditor::on_btn_pagenext_clicked()
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
void RTDbDataEditor::on_act_tblrow_triggered()
{
	if(ui.tree_db->currentItem()->data(0,Qt::UserRole).toInt()!=1)
		return;
	ShmDbTblHeaderdef* header = (ShmDbTblHeaderdef*)m_dbHeader;
	if(!header)
		return;
	QDialog dlg;
	QVBoxLayout* lay = new QVBoxLayout(&dlg);
	QHBoxLayout* hlay =new QHBoxLayout;
	hlay->addWidget(new QLabel(tr("The number of records:")));
	QLineEdit* lerows = new QLineEdit;
	lerows->setValidator(new QRegExpValidator(QRegExp("^\\d+(\\d+)?$"), this));//非负整数
	lerows->setText(QString::number(header->row_num ));
	hlay->addWidget(lerows);
	lay->addLayout(hlay);
	QPushButton* btn =new QPushButton(tr("OK"));
	hlay = new QHBoxLayout;
	hlay->addSpacing(1);
	hlay->addWidget(btn);
	lay->addLayout(hlay);
	connect(btn,SIGNAL(clicked()),&dlg,SLOT(accept()));
	if(dlg.exec() == QDialog::Accepted)
	{
		QByteArray ba = m_basepath.toLocal8Bit();
		map<int, dataTypeDef>& dataTp = getRtDbDataType(ba.data());
		int newrows = qMin((unsigned int)lerows->text().toInt(),header->max_row_num);
		for (int row = header->row_num; row < newrows; row++)
		{
			char* rowdata = m_dbData + header->tbl_address + row* header->row_size;

			std::vector<coldef>::iterator colit = m_curTblDef->cols.begin();
			int c=1;
			while(colit!=m_curTblDef->cols.end() && c<header->col_num)
			{
				if(!colit->default_val.empty() && dataTp.find(colit->datatype)!=dataTp.end())
				{
					switch(colit->datatype)
					{
					case dataTp_int://int
						*(int*)rowdata = QString::fromStdString(colit->default_val).toInt();
						break;
					case dataTp_float://float
						*(float*)rowdata = QString::fromStdString(colit->default_val).toFloat();
						break;
					case dataTp_double://double
						*(double*)rowdata = QString::fromStdString(colit->default_val).toDouble();
						break;
					case dataTp_short://short
						*(short*)rowdata = QString::fromStdString(colit->default_val).toShort();
						break;
					case dataTp_char://char
						*rowdata = colit->default_val.c_str()[0];
						break;
					case dataTp_string://string
						strcpy(rowdata,colit->default_val.c_str());
						break;
					case dataTp_varbtye://varByte
						break;
					case dataTp_image://image
						break;
					default:
						;
					}
					rowdata += dataTp[colit->datatype].nsize * colit->tplen;
				}
				colit++;
				c++;
			}
		}
		header->row_num = newrows;
		showPage(ui.le_curpage->text().toInt()-1);
	}
}
void RTDbDataEditor::on_act_editindex_triggered(bool f)
{
	if(ui.tree_db->currentItem()->data(0,Qt::UserRole).toInt()!=1)
	{
		ui.act_editindex->setChecked(false);
		m_indflag = false;
		ui.btn_saveind->setVisible(false);
		showPage(ui.le_curpage->text().toInt()-1);
		return;
	}
	m_indflag = f;
	ui.btn_saveind->setVisible(f);
	showPage(ui.le_curpage->text().toInt()-1);
}
void RTDbDataEditor::on_btn_saveind_clicked()
{
	if(ui.tree_db->currentItem()->data(0,Qt::UserRole).toInt()!=1)
		return;
	ShmDbTblHeaderdef* header = (ShmDbTblHeaderdef*)m_dbHeader;
	if(!header)
		return;
	foreach(int r,g_indexTmp.keys())
	{
		if(r == g_indexTmp[r])
		{
			g_indexTmp.remove(r);
			continue;
		}
		if (!g_indexTmp.contains(g_indexTmp[r]))
		{
			QMessageBox::warning(this,tr("System Prompt"),QString(tr("The %1 ID setting error,with other ID repeat or exceeds the value range of ID (0~%2) ").arg(r).arg(header->row_num)));
			return;
		}
	}
	char * tmp = new char[header->row_size];
	int ntmp;
	QMap<int,int>::iterator it = g_indexTmp.begin();
	QMap<int,int>::iterator it2;
	char *data1,*data2;
	data1 = m_dbData + header->tbl_address + it.key() * header->row_size;
	ntmp = it.key();
	memcpy(tmp,data1,header->row_size);
	int n1,n2;
	while(it != g_indexTmp.end())
	{
		data1 = m_dbData + header->tbl_address + it.key() * header->row_size;
		if(it.value() == ntmp)
		{
			memcpy(data1,tmp,header->row_size);
			g_indexTmp.erase(it);
			it = g_indexTmp.begin();
			if(it == g_indexTmp.end())
				break;
			data1 = m_dbData + header->tbl_address + it.key() * header->row_size;
			ntmp = it.key();
			memcpy(tmp,data1,header->row_size);
		}
		data2 = m_dbData + header->tbl_address + it.value() * header->row_size;
		memcpy(data1,data2,header->row_size);
		it2 = g_indexTmp.find(it.value());
		g_indexTmp.erase(it);
		it = it2;
	}
	delete[] tmp;

	showPage(ui.le_curpage->text().toInt()-1);
}