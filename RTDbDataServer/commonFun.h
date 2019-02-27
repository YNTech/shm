#pragma once
#ifndef __COMMONFUN_H__
#define __COMMONFUN_H__
#pragma execution_character_set("utf-8")
#include <QByteArray>
int  getCurrentNodeId();//当前计算机所对应的节点id
int  getSvrNodeId();//查询svr id;
unsigned int getSvrAdress();//返回IPv4
unsigned short getSvrPort();
void setSvrNode(int nid, unsigned int addr = 0x7f000001, unsigned short port = 3477);//更改Svr id;
int encodeRtdb_Db(QByteArray& d, const char* dbname, const int bg_addr, int max_size);
bool decodeRtdb_Db(QByteArray& d);
void runRTDBwithGui();
bool g_curIsSvr();
#endif//__COMMONFUN_H__

