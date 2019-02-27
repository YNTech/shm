#ifndef SETDATASOURCE_H
#define SETDATASOURCE_H

#include <QDialog>
#include "ui_setdatasource.h"

//static char* g_netType[] = {"HLA","DDS","Sock"};
//static int g_netTypeNum = 3;
//static char* g_psSetting[] = {"Publish","Subscribe"};
class setDataSource : public QDialog
{
	Q_OBJECT

public:
	enum {NetTp_HLA =1,NetTp_DDS,NetTp_Sock};
	enum {ps_Publish =1,ps_Subscribe};
	setDataSource(QWidget *parent = 0);
	~setDataSource();
	int m_netType;
	int m_ddsPS;
	int m_hlaPS;
	int m_sockPS;
private slots:
	void on_chk_dds_toggled ( bool);
	void on_chk_hla_toggled ( bool);
	void on_chk_sock_toggled ( bool);
private:
	Ui::setDataSource ui;
};

#endif // SETDATASOURCE_H
