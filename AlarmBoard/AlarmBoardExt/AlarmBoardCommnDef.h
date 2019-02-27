#ifndef __ALARMBOARDCOMMNDEF_H__
#define __ALARMBOARDCOMMNDEF_H__


#define ALARMBOARDNAME "AlarmBoard"
#define TYYSENDTIMES  3
#define HEARTBEAT_MAXTIME 50000 //tcp连接心跳超时时间
#define HEARTBEAT_TIME 10000 //tcp连接心跳周期
enum Log_Level
{
	Log_Debug = 0,
	Log_Notify = 1, 
	Log_Warning = 2,
	Log_Error = 3,
	Lev_5 = 4, 
	Lev_6 = 5,
	Lev_7 = 6,
};
enum commdKey
{
	CMD_Register		= 1,
	CMD_registerReturn	= 2,
	CMD_unRegister		= 3,
	CMD_Log				= 5,
	CMD_LogSys			= 6,
	CMD_bgSave			= 7,
	CMD_endSave			= 9,
	CMD_isShow			= 12,
	CMD_HEARTBEAT		= 13,
};

#endif//__ALARMBOARDCOMMNDEF_H__