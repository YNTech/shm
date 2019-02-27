#ifndef __NETCMDDEF_H__
#define __NETCMDDEF_H__
//端口定义
#define CMDSVRPORT 3477 //RTDB服务端指令接收端口 UDP
#define CMDRECVPORT 5174 //各节点指令接收端口    UDP
#define DATARECVPORT 4174 //TCP数据接收端口   cilent TCP

//tcp每包大小
#define MAX_PACKET_TCP 0xFFFF
#define HEARTBEAT_MAXTIME 50000 //tcp连接心跳超时时间
#define HEARTBEAT_TIME 10000 //tcp连接心跳周期

enum CtrCommand{ //udp控制指令
	RTDB_CALLSVR = 1,			//查找服务器广播
	RTDB_BCSVRID,				//服务器广播svrid
	//RTDB_SENDDATA, 
	RTDB_LNKSVR,				//连接请求
};
enum CtrDataTrans{ //通过tcp交互的数据传输指令
	DATATRANS_CONNECT,			//加入连接指令
	DATATRANS_HEARTBEAT,		//心跳
	DATATRANS_RTDBSYNC,			//同步RTDB请求		3
	DATATRANS_CHECKMD5,		    //校验数据集MD5     4
	DATATRANS_DBDEFSYNCREQ,		//同步DB def请求    5
	DATATRANS_DBDEFSYNCFINISH,	//同步DB def完成指令    6
	DATATRANS_DBDATASYNCREQ,	//RTDB Data同步请求   7
	DATATRANS_DBDATASYNCFINISH,	//RTDB Data同步完成   8

	DATATRANS_DBDATA,			//同步数据库数据   9
	//文件管理
	DATATRANS_FILEBEGIN,		//文件头
	DATATRANS_FILEDATA,			//文件中间数据
	DATATRANS_FILEEND,			//文件结尾
	//
	//DATATRANS_DBSETS = 0x1001, //数据集和path 4
	
	//DATATRANS_DBDEFFILE,		//数据集db    
	//DATATRANS_DBSETSDATA, 
	//DATATRANS_DBSYNREQUEST,     //数据库同步请求
	//DATATRANS_DBDATA 
};

#endif
