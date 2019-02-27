#include "setdatasource.h"

setDataSource::setDataSource(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

setDataSource::~setDataSource()
{

}
void setDataSource::on_chk_dds_toggled ( bool f)
{
	ui.chk_dds_pub->setEnabled(f);
	ui.chk_dds_sub->setEnabled(f);
}
void setDataSource::on_chk_hla_toggled ( bool f)
{
	ui.chk_hla_pub->setEnabled(f);
	ui.chk_hla_sub->setEnabled(f);
}
void setDataSource::on_chk_sock_toggled ( bool f )
{
	ui.chk_sock_pub->setEnabled(f);
	ui.chk_sock_sub->setEnabled(f);
}