#include <QtWidgets>
#include "netsetting.h"
#include "rtdb_interface.h"
#include "rtdb_define.h"
#include "ui_setdatasource.h"

using namespace std;
netSetting::netSetting(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.tree_db,SIGNAL(expanded( const QModelIndex &)),this,SLOT(resizeTreeItem()));
	connect(ui.tree_db,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(setDataSource(QTreeWidgetItem *)));
}

netSetting::~netSetting()
{

}
void netSetting::on_tbtn_open_clicked()
{

}
void netSetting::on_tbtn_load_clicked()
{
	QDialog dlg(this);
	dlg.move(this->rect().center());
	QVBoxLayout* vlay = new QVBoxLayout(&dlg);
	QHBoxLayout* hlay = new QHBoxLayout;
	m_leRootPath = new QLineEdit;
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

	m_leRootPath->setText("..\\db");
	m_dbsetsnm->clear();
	QDir dir(m_leRootPath->text()+"\\dbdef");
	QStringList flnames = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
	foreach(QString fn,flnames)
		m_dbsetsnm->addItem(fn);
	//selectPath(false);
	if(dlg.exec()== QDialog::Accepted)
	{
		ui.tree_db->clear();
		char err[256];
		QByteArray dbsetsname = m_dbsetsnm->currentText().toLatin1();
		QByteArray rootPath = m_leRootPath->text().toLocal8Bit();
		if(!readRTDbSetsDef(rootPath.data(),dbsetsname , err))
		{
			QMessageBox::warning(this,tr("Error"),tr("Load database set definition of failure:")+err);
			return;
		}
		QTreeWidgetItem* it = new QTreeWidgetItem(QStringList()<<m_dbsetsnm->currentText());
		it->setIcon(0,QPixmap(":/Resources/dbsets.png"));
		it->setData(0,Qt::UserRole,TreeTp_DbSet);
		it->setData(0,Qt::UserRole+6,rootPath);
		ui.tree_db->addTopLevelItem(it);

		RTDbSets *dbsets = getRtDbDef(dbsetsname);
		vector<RTDbInfo>::iterator dbit= dbsets->rtdbs.begin();
		while(dbit != dbsets->rtdbs.end())
		{
			QTreeWidgetItem* dbitem = new QTreeWidgetItem(QStringList()<<QString::fromStdString(dbit->name));
			dbitem->setIcon(0,QPixmap(":/Resources/db.png"));
			dbitem->setData(0,Qt::UserRole,TreeTp_Db);
			it->addChild(dbitem);
			vector<tbldef>::iterator tblit= dbit->tbls.begin();
			while(tblit != dbit->tbls.end())
			{
				QTreeWidgetItem* tblitem = new QTreeWidgetItem(QStringList()<<QString::fromStdString(tblit->name));
				tblitem->setIcon(0,QPixmap(":/Resources/table.png"));
				tblitem->setData(0,Qt::UserRole,TreeTp_Tbl);
				dbitem->addChild(tblitem);
				tblit++;
			}
			dbit++;
		}
	}
}
void netSetting::resizeTreeItem()
{
	ui.tree_db->resizeColumnToContents(0);
	ui.tree_db->resizeColumnToContents(1);
	ui.tree_db->resizeColumnToContents(2);
}
void netSetting::selectPath()
{
	QString rootPath = QFileDialog::getExistingDirectory(this, tr("Open RTDB"),	m_leRootPath->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
	if(rootPath.isEmpty())
		return;
	m_leRootPath->setText(rootPath);
	m_dbsetsnm->clear();
	QDir dir(rootPath+"\\dbdef");
	QStringList flnames = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
	foreach(QString fn,flnames)
		m_dbsetsnm->addItem(fn);
}
void netSetting::setDataSource(QTreeWidgetItem * item)
{

}