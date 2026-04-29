// Notice.h: interface for the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Message.h"
#include "Protocol.h"

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_NOTICE_SEND
{
	PBMSG_HEAD header; // C1:0D
	BYTE type;
	BYTE count;
	BYTE opacity;
	WORD delay;
	DWORD color;
	BYTE speed;
	char message[256];
};

//**********************************************//
//**********************************************//
//**********************************************//

struct NOTICE_DATA_INFO
{
	char Message[128];
	int Type;
	int Count;
	int Opacity;
	int Delay;
	int Color;
	int Speed;
	int RepeatTime;
};

struct NOTICE_INFO
{
	int index;
	DWORD Time;
	std::vector<NOTICE_DATA_INFO> NoticeInfo;
};

class CNotice
{
	CNotice();
	virtual ~CNotice();
	SingletonInstance(CNotice)
public:
	void Load(char* path,int lang);
	void MainProc();
	void GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...);
	void GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,int message,...);
	void GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...);
	void GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,int message,...);
private:
	NOTICE_INFO m_NoticeInfo[MAX_LANGUAGE];
};

#define gNotice SingNull(CNotice)