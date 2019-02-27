#ifndef __NETCMDDEF_H__
#define __NETCMDDEF_H__
//�˿ڶ���
#define CMDSVRPORT 3477 //RTDB�����ָ����ն˿� UDP
#define CMDRECVPORT 5174 //���ڵ�ָ����ն˿�    UDP
#define DATARECVPORT 4174 //TCP���ݽ��ն˿�   cilent TCP

//tcpÿ����С
#define MAX_PACKET_TCP 0xFFFF
#define HEARTBEAT_MAXTIME 50000 //tcp����������ʱʱ��
#define HEARTBEAT_TIME 10000 //tcp������������

enum CtrCommand{ //udp����ָ��
	RTDB_CALLSVR = 1,			//���ҷ������㲥
	RTDB_BCSVRID,				//�������㲥svrid
	//RTDB_SENDDATA, 
	RTDB_LNKSVR,				//��������
};
enum CtrDataTrans{ //ͨ��tcp���������ݴ���ָ��
	DATATRANS_CONNECT,			//��������ָ��
	DATATRANS_HEARTBEAT,		//����
	DATATRANS_RTDBSYNC,			//ͬ��RTDB����		3
	DATATRANS_CHECKMD5,		    //У�����ݼ�MD5     4
	DATATRANS_DBDEFSYNCREQ,		//ͬ��DB def����    5
	DATATRANS_DBDEFSYNCFINISH,	//ͬ��DB def���ָ��    6
	DATATRANS_DBDATASYNCREQ,	//RTDB Dataͬ������   7
	DATATRANS_DBDATASYNCFINISH,	//RTDB Dataͬ�����   8

	DATATRANS_DBDATA,			//ͬ�����ݿ�����   9
	//�ļ�����
	DATATRANS_FILEBEGIN,		//�ļ�ͷ
	DATATRANS_FILEDATA,			//�ļ��м�����
	DATATRANS_FILEEND,			//�ļ���β
	//
	//DATATRANS_DBSETS = 0x1001, //���ݼ���path 4
	
	//DATATRANS_DBDEFFILE,		//���ݼ�db    
	//DATATRANS_DBSETSDATA, 
	//DATATRANS_DBSYNREQUEST,     //���ݿ�ͬ������
	//DATATRANS_DBDATA 
};

#endif
