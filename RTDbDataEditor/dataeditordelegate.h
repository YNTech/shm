#ifndef DATAEDITORDELEGATE_H
#define DATAEDITORDELEGATE_H

#include <QItemDelegate>
#include <QDialog>
class QLabel;
class QDialog;

class DataEditorDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	DataEditorDelegate(QObject *parent=0);
	~DataEditorDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
	bool eventFilter(QObject *object, QEvent *event);

};
class IndexDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	IndexDelegate(QObject *parent=0);
	~IndexDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;


};
class myDialog : public QDialog
{
	Q_OBJECT
public:
	myDialog(QWidget * parent = 0, Qt::WindowFlags f = 0):QDialog(parent,f){};
public slots:
	void accept();
};
class LabelIcon : public QDialog
{
	Q_OBJECT
	Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap USER true)
public:
	LabelIcon(QWidget * parent = 0, Qt::WindowFlags f = 0);
	void setPixmap ( const QPixmap & );
	const QPixmap * pixmap () const;
private slots:
	void selectPath();
private:
	QLabel * m_labicon;
};

#endif // DATAEDITORDELEGATE_H
