#ifndef __MYDLG_H__
#define __MYDLG_H__
#include <QtWidgets>
class MyDlg :public QDialog
{
	Q_OBJECT
public:
	MyDlg()
	{
		//QDialog dlg(this);
		//dlg.move(this->rect().center());
		QVBoxLayout* vlay = new QVBoxLayout(this);
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
		connect(btnok, SIGNAL(clicked()), this, SLOT(accept()));
		connect(btn, SIGNAL(clicked()), this, SLOT(selectPath()));
		hlay->addWidget(btnok);
		vlay->addLayout(hlay);

		m_leRootPath->setText("..\\db");
		m_dbsetsnm->clear();
		QDir dir(m_leRootPath->text() + "\\rtdb");
		QStringList flnames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
		foreach(QString fn, flnames)
			m_dbsetsnm->addItem(fn);
	}
	private slots:
	void selectPath()
	{
		QString rootPath = QFileDialog::getExistingDirectory(this, tr("Open RTDB"), m_leRootPath->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (rootPath.isEmpty())
			return;
		m_leRootPath->setText(rootPath);
		m_dbsetsnm->clear();
		QDir dir(rootPath + "\\rtdb");
		QStringList flnames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
		foreach(QString fn, flnames)
			m_dbsetsnm->addItem(fn);
	}
public:
	QComboBox* m_dbsetsnm;
	QLineEdit* m_leRootPath;
};

#endif
