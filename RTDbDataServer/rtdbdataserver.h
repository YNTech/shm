#ifndef RTDBDATASERVER_H
#define RTDBDATASERVER_H

#include <QtWidgets/QMainWindow>
#include <QMap>
#include "NetSettingClass.h"
#include "ui_rtdbdataserver.h"
#include "rtdbnetplugn.h"

class RTDbDataServer : public QMainWindow
{
	Q_OBJECT

public:
	RTDbDataServer(QString nd,QWidget *parent = 0);
	~RTDbDataServer();
	
private slots:
	void on_actionNet_setting_triggered();
	void on_actionStart_triggered();
	void on_actionStop_triggered();
private:
	Ui::RTDbDataServerClass ui;
	RTDbNetPlugn m_netplugn;
	QString m_curNd;
	NetSettingClass m_cmdSvr;//÷∏¡Óπ‹¿Ì
};

#endif // RTDBDATASERVER_H
