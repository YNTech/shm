#include "comboboxdelegate.h"
#include <QtWidgets>
#include "rtdb_interface.h"
#include "rtdb_define.h"
using namespace std;

ComboBoxDelegate::ComboBoxDelegate(QString rootpath, QObject *parent)
	: QItemDelegate(parent)
{
	m_rootpath = rootpath;
}

ComboBoxDelegate::~ComboBoxDelegate()
{

}
QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
									   const QStyleOptionViewItem &/* option */,
									   const QModelIndex & index ) const
{
	QComboBox *editor = new QComboBox(parent);
	QByteArray ba = m_rootpath.toLocal8Bit();
	map<int,dataTypeDef>& dataTp = getRtDbDataType(ba.data());
	map<int,dataTypeDef>::iterator it = dataTp.begin();
	while(it!=dataTp.end())
	{
		editor->addItem(QString::fromStdString(it->second.name),it->first);
		it++;
	}
	return editor;
}
void ComboBoxDelegate::setEditorData(QWidget *editor,
									const QModelIndex &index) const
{
	int value = index.model()->data(index, Qt::UserRole).toInt();

	QComboBox * combeditor = qobject_cast<QComboBox*>(editor);
	combeditor->setCurrentIndex(combeditor->findData(value));
}
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
								   const QModelIndex &index) const
{
	QComboBox *combeditor = qobject_cast<QComboBox*>(editor);

	model->setData(index, combeditor->itemData(combeditor->currentIndex()), Qt::UserRole);
}
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
										   const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}