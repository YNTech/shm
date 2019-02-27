#include "dataeditordelegate.h"
#include <QtGui>
#include "rtdb_define.h"
#include "rtdb_interface.h"
#include "rtdbdataeditor.h"
#include "rtdb_api.h"
using namespace std;
class RTDbDataEditor;

extern QMap<int,int> g_indexTmp;
DataEditorDelegate::DataEditorDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

DataEditorDelegate::~DataEditorDelegate()
{

}
QWidget *DataEditorDelegate::createEditor(QWidget *parent,
										const QStyleOptionViewItem & option ,
										const QModelIndex & index ) const
{
	QWidget * editor =nullptr;
	int ntp = index.model()->data(index, Qt::UserRole+1).toInt();
	switch(ntp)
	{
	case dataTp_int://int
	case dataTp_short://short
	case dataTp_char://char
		{
			QStringList strs = index.model()->data(index, Qt::UserRole+2).toString().split(QRegExp("[{:,}]"),QString::SkipEmptyParts);
			if(strs.size()>0)
			{
				if(strs[0] == "CheckBox")
					editor = new QCheckBox(parent);
				else if(strs[0] == "Enum")
				{
					QComboBox* comb = new QComboBox(parent);
					editor = comb;
					for(int i=1; i<strs.size()-1; i+=2)
						comb->addItem(strs[i],strs[i+1]);
				}
				else if(strs[0] == "Flag")
				{
					myDialog *dlg =new myDialog(parent,Qt::ToolTip);
					editor = dlg;
					QVBoxLayout* vlay =new QVBoxLayout(dlg);
					vlay->setContentsMargins(0,0,0,0);
					QListWidget* lst = new QListWidget;
					lst->setObjectName("lst_val");
					lst->setSelectionMode(QAbstractItemView::MultiSelection);
					vlay->addWidget(lst);
					QHBoxLayout* hlay = new QHBoxLayout;
					QToolButton* btnok = new QToolButton;
					btnok->setIcon(QIcon(QPixmap(QString::fromUtf8(":/Resources/ok.png"))));
					hlay->addSpacing(1);
					hlay->addWidget(btnok);
					QToolButton* btcancel = new QToolButton;
					btcancel->setIcon(QIcon(QPixmap(QString::fromUtf8(":/Resources/cancel.png"))));
					hlay->addWidget(btcancel);
					vlay->addLayout(hlay);
					connect(btnok,SIGNAL(clicked()),dlg,SLOT(accept()));
					connect(btcancel,SIGNAL(clicked()),dlg,SLOT(reject()));

					for(int i=1; i<strs.size()-1; i+=2)
					{
						QListWidgetItem* it =new QListWidgetItem;
						it->setText(strs[i]);
						it->setData(Qt::UserRole,strs[i+1]);
						lst->addItem(it);
					}
				}
				else if(strs[0] == "Reference")
				{
					QComboBox* comb = new QComboBox(parent);
					editor = comb;
					int nNum = strs.size();
					if (nNum<2)
						break;
					if (nNum ==2 && strs[1].toUpper() == "DB")//Reference{Db}：数据库列表 
					{
						char (*dbnames)[db_name_length];
						int dbnums = 0;
						rtdb_get_dbnames(&dbnames, dbnums);
						for (int i = 0; i < dbnums;i++)
							comb->addItem(dbnames[i], i);
					}
					else 
					{
						if (nNum<3)
							break;
						if (nNum == 3 && strs[2].toUpper() == "TABLE")
						{
							QByteArray dbnm;
							if (strs[1].toUpper() == "DB")
							{
								if (index.column()>=1)
									dbnm = index.model()->data(index.model()->index(index.row(), index.column()-1)).toString().toLocal8Bit();
							}
							else
								dbnm = strs[1].toLocal8Bit();
							char* tblnames[max_tables_num];
							int tblnum = 0;
							rtdb_get_tblnames(tblnames, tblnum, dbnm.data());
							for (int i = 0; i < tblnum; i++)
								comb->addItem(tblnames[i], i);
						}
						else
						{
							if (nNum<4)
								break;
							void* vals[1000];//
							//std::list<void*> vals;
							int datalen, rownum = 1000, bgrow = 0;
							int ind = 0;
							while (rownum==1000)
							{
								rtdb_get_colsdata(strs[1].toLocal8Bit().data(), strs[2].toLocal8Bit().data(), strs[3].toLocal8Bit().data(), bgrow, rownum, vals, datalen);
								for (int i = 0; i < rownum;i++)
									comb->addItem(QString::fromLocal8Bit((char*)(vals[i])), ind++);
								bgrow += rownum;
							}
						}
					}
				}
			}
			if(!editor)
			{
				QLineEdit* le = new QLineEdit(parent);
				editor =le;
				if(ntp != dataTp_char)
					le->setValidator(new QRegExpValidator(QRegExp("^-?[1-9]\\d*|0$"), le));
				else
					le->setValidator(new QRegExpValidator(QRegExp("^(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])$"), le));//0~255
			}
		}
		break;
	case dataTp_float://float
	case dataTp_double://double
		editor = new QLineEdit(parent);
		((QLineEdit*)editor)->setValidator(new QRegExpValidator(QRegExp("^-?([1-9]\\d*\\.\\d*|0\\.\\d*[1-9]\\d*|0?\\.0+|0)$"), editor));//浮点数
		break;
	case dataTp_string://string
		editor = new QLineEdit(parent);
		break;
	case dataTp_varbtye://varByte
		break;
	case dataTp_image://image
		editor = new LabelIcon(parent,Qt::ToolTip);//
		break;
	default:
		return QItemDelegate::createEditor(parent,option,index);
	}
	return editor;
}
void myDialog::accept()
{
	setResult(QDialog::Accepted);
	QDialog::accept();
}
void DataEditorDelegate::setEditorData(QWidget *editor,
									 const QModelIndex &index) const
{
	unsigned char* value = (unsigned char*)(index.model()->data(index, Qt::UserRole).toInt());
	int ntp = index.model()->data(index, Qt::UserRole+1).toInt();
	int nval;
	switch(ntp)
	{
	case dataTp_int://int
	case dataTp_short://short
	case dataTp_char://char
		{
			if(ntp == dataTp_int)
				nval = *((int*)value);
			else if(ntp == dataTp_short)
				nval = *((short*)value);
			else
				nval = *value;

			QStringList strs = index.model()->data(index, Qt::UserRole+2).toString().split(QRegExp("[{:,}]"),QString::SkipEmptyParts);
			if(strs.size()>0)
			{
				if(strs[0] == "CheckBox")
				{
					QCheckBox* chk = qobject_cast<QCheckBox*>(editor);
					if(chk)
						chk->setChecked((bool)(*value));
					break;
				}
				else if(strs[0] == "Enum")
				{
					QComboBox* comb =  qobject_cast<QComboBox*>(editor);
					if(comb)
						comb->setCurrentIndex(comb->findData(nval));
					break;
				}
				else if(strs[0] == "Flag")
				{
					QDialog* dlg =  qobject_cast<QDialog*>(editor);
					if(dlg)
					{
						QListWidget* lst = dlg->findChild<QListWidget *>("lst_val");
						for(int i=0; i<lst->count(); i++)
						{
							QListWidgetItem* it = lst->item(i);
							if(nval&(it->data(Qt::UserRole).toInt()))
								it->setSelected(true);
							else
								it->setSelected(false);
						}
					}
					break;
				}
				else if(strs[0] == "Reference")
				{
					QComboBox* comb =  qobject_cast<QComboBox*>(editor);
					if(comb)
						comb->setCurrentIndex(comb->findData(nval));
					break;
				}
			}
			QLineEdit* wg = qobject_cast<QLineEdit*>(editor);
			if(wg)
				wg->setText(QString::number(nval));
		}
		break;
	case dataTp_float://float
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			le->setText(QString::number(*((float*)value)));
		}
		break;
	case dataTp_double://double
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			le->setText(QString::number(*((double*)value)));
		}
		break;
	case dataTp_string://string
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			le->setText(QString::fromLocal8Bit((char*)value));
		}
		break;
	case dataTp_varbtye://varByte
		break;
	case dataTp_image://image
		{
			LabelIcon* lab = qobject_cast<LabelIcon*>(editor);
			char* value = (char*)(index.model()->data(index, Qt::UserRole).toInt());
			int nlen = index.model()->data(index, Qt::UserRole+3).toInt();
			QByteArray ba = QByteArray::fromRawData(value,nlen);
			QPixmap ico;
			QDataStream sout(&ba,QIODevice::ReadOnly);
			sout>>ico;
			lab->setPixmap(ico);
		}
		break;
	default:
		return QItemDelegate::setEditorData(editor,index);
	}

}
void DataEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
									const QModelIndex &index) const
{
	char* rawval = (char*)(index.model()->data(index, Qt::UserRole).toInt());
	int ntp = index.model()->data(index, Qt::UserRole+1).toInt();
	bool flag =false;
	int nval = 0;
	QStringList strs = index.model()->data(index, Qt::UserRole+2).toString().split(QRegExp("[{:,}]"),QString::SkipEmptyParts);
	switch(ntp)
	{
	case dataTp_int://int
	case dataTp_short://short
	case dataTp_char://char
		{
			if(strs.size()>0)
			{
				if(strs[0] == "CheckBox")
				{
					QCheckBox* chk = qobject_cast<QCheckBox*>(editor);
					if(chk)
						nval = chk->isChecked();
					if(strs.size() == 5)
						model->setData(index, nval==strs[2].toInt()?strs[1]:strs[3], Qt::EditRole);
					flag =true;
				}
				else if(strs[0] == "Enum")
				{
					QComboBox* comb =  qobject_cast<QComboBox*>(editor);
					if(comb)
						nval = comb->itemData(comb->currentIndex()).toInt();
					model->setData(index, comb->currentText(), Qt::EditRole);
					flag =true;
				}
				else if(strs[0] == "Flag")
				{
					QDialog* dlg =  qobject_cast<QDialog*>(editor);
					if(dlg)
					{
						if(dlg->result() == QDialog::Rejected)
							break;
						QListWidget* lst = dlg->findChild<QListWidget *>("lst_val");
						QStringList lstval;
						for(int i=0; i<lst->count(); i++)
						{
							QListWidgetItem* it = lst->item(i);
							if(it->isSelected())
							{
								nval |= it->data(Qt::UserRole).toInt();
								lstval << it->text();
							}
						}
						model->setData(index, lstval.join("|"), Qt::EditRole);
					}
					flag =true;
				}
				else if(strs[0] == "Val_range" && strs.size() == 3)
				{
					QLineEdit* wg = qobject_cast<QLineEdit*>(editor);
					nval = qBound(strs[1].toInt(),wg->text().toInt(),strs[2].toInt());
					model->setData(index, QString::number(nval), Qt::EditRole);
					flag = true;
				}
				else if(strs[0] == "Reference")
				{
					QComboBox* comb =  qobject_cast<QComboBox*>(editor);
					if(comb)
						nval = comb->itemData(comb->currentIndex()).toInt();
					model->setData(index, comb->currentText(), Qt::EditRole);
					flag =true;
				}
			}
			if(!flag)
			{
				QLineEdit* wg = qobject_cast<QLineEdit*>(editor);
				nval = wg->text().toInt();
				model->setData(index, QString::number(nval), Qt::EditRole);
			}
			if(ntp == dataTp_int)
				*((int*)rawval) = nval;
			else if(ntp == dataTp_short)
				*((short*)rawval) = nval;
			else if(ntp == dataTp_char)
				*((char*)rawval) = nval;
		}
		break;
	case dataTp_float://float
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			float fval;
			if(strs.size() == 3 && strs[0] == "Val_range")
				fval = qBound(strs[1].toFloat(),le->text().toFloat(),strs[2].toFloat());
			else
				fval = le->text().toFloat();
			*((float*)rawval) = fval;
			model->setData(index, QString::number(fval), Qt::EditRole);
		}
		break;
	case dataTp_double://double
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			double fval;
			if(strs.size() == 3 && strs[0] == "Val_range")
				fval = qBound(strs[1].toDouble(),le->text().toDouble(),strs[2].toDouble());
			else
				fval = le->text().toDouble();
			*((double*)rawval) = fval;
			model->setData(index, QString::number(fval), Qt::EditRole);
		}
		break;
	case dataTp_string://string
		{
			QLineEdit* le = qobject_cast<QLineEdit*>(editor);
			int nlen = index.model()->data(index, Qt::UserRole+3).toInt();
			sprintf_s(rawval,nlen,le->text().toLocal8Bit().data());
			model->setData(index, le->text(), Qt::EditRole);
		}
		break;
	case dataTp_varbtye://varByte
		break;
	case dataTp_image://image
		{
			LabelIcon* lab = qobject_cast<LabelIcon*>(editor);
			int nlen = index.model()->data(index, Qt::UserRole+3).toInt();
			QByteArray ba;//(rawval,nlen);
			const QPixmap *ico = lab->pixmap();
			QDataStream sin(&ba,QIODevice::WriteOnly);
			sin<<*ico;
			model->setData(index, *ico, Qt::DecorationRole);
			int nsize = ba.size();
			if(nsize > nlen)
				QMessageBox::warning(0,tr("System Prompts"),tr("Because the selected picture size is greater than the memory size cannot be saved!"));
			memcpy(rawval,ba.data(),qMin(nlen,nsize));
		}
		break;
	default:
		return QItemDelegate::setModelData(editor,model,index);
	}
}
void DataEditorDelegate::updateEditorGeometry(QWidget *editor,
											const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	QDialog* dlg =qobject_cast<QDialog*>(editor);
	if(dlg)
	{
		QPoint p = dlg->parentWidget()->mapToGlobal(option.rect.topLeft());
		dlg->setGeometry(p.x(),p.y(),option.rect.width(),option.rect.height());
	}
	else
		editor->setGeometry(option.rect);
}
bool DataEditorDelegate::eventFilter(QObject *object, QEvent *event)
{
	LabelIcon *editor = qobject_cast<LabelIcon*>(object);
	if (editor && event->type() == QEvent::FocusOut )
	{
		//the Hide event will take care of he editors that are in fact complete dialogs
		if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
			QWidget *w = QApplication::focusWidget();
			while (w) { // don't worry about focus changes internally in the editor
				if (w == editor)
					return false;
				w = w->parentWidget();
			}
			// Opening a modal dialog will start a new eventloop
			// that will process the deleteLater event.
			if (QApplication::activeModalWidget()
				&& !QApplication::activeModalWidget()->isAncestorOf(editor)
				&& qobject_cast<QDialog*>(QApplication::activeModalWidget()))
				return false;
			emit commitData(editor);
			emit closeEditor(editor, NoHint);
			return false;
		}
	}
	return QItemDelegate::eventFilter(object,event);
}
LabelIcon::LabelIcon(QWidget * parent , Qt::WindowFlags f):QDialog(parent,f)
{
	QHBoxLayout * lay =new QHBoxLayout(this);
	lay->setContentsMargins(0,0,0,0);
	m_labicon = new QLabel;
	lay->addWidget(m_labicon);
	QToolButton * btn =new QToolButton;
	btn->setText("...");
	lay->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(selectPath()));
}
void LabelIcon::selectPath()
{
	QString f = QFileDialog::getOpenFileName(parentWidget()->parentWidget(), tr("Open Pixmap"),	".",tr("Images (*.png *.xpm *.jpg *.bmp *.gif *.jpeg *.pbm *.pgm *.ppm *.xbm)"));
	if(!f.isEmpty())
		m_labicon->setPixmap(QPixmap(f));
}
void LabelIcon::setPixmap ( const QPixmap & pix)
{
	m_labicon->setPixmap(pix);
}
const QPixmap * LabelIcon::pixmap () const
{
	return m_labicon->pixmap();
}

IndexDelegate::IndexDelegate(QObject *parent)
: QItemDelegate(parent)
{

}

IndexDelegate::~IndexDelegate()
{

}
QWidget *IndexDelegate::createEditor(QWidget *parent,
										  const QStyleOptionViewItem &/* option */,
										  const QModelIndex & index ) const
{
	QLineEdit* editor = new QLineEdit(parent);
	((QLineEdit*)editor)->setValidator(new QRegExpValidator(QRegExp("^\\d+$"), editor));//非负整数
	return editor;
}
void IndexDelegate::setEditorData(QWidget *editor,
									   const QModelIndex &index) const
{
	QString ind = index.model()->data(index, Qt::EditRole).toString();
	QLineEdit* le = qobject_cast<QLineEdit*>(editor);
	if(le)
		le->setText(ind);
}
void IndexDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
									  const QModelIndex &index) const
{
	QLineEdit* le = qobject_cast<QLineEdit*>(editor);
	if(le)
	{
		model->setData(index,le->text(), Qt::EditRole);
		int row = model->data(index,Qt::UserRole).toInt();
		//if(g_indexTmp.contains(row))
			g_indexTmp[row] = le->text().toInt();
	}
}
void IndexDelegate::updateEditorGeometry(QWidget *editor,
											  const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}