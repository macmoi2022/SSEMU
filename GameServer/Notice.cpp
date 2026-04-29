// Notice.cpp: implementation of the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Notice.h"
#include "Message.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotice::CNotice() // OK
{
	for(int n = 0; n < MAX_LANGUAGE; n++)
	{
		this->m_NoticeInfo[n].index = 0;

		this->m_NoticeInfo[n].Time = GetTickCount();

		this->m_NoticeInfo[n].NoticeInfo.clear();
	}
}

CNotice::~CNotice() // OK
{

}

void CNotice::Load(char* path,int lang) // OK
{
	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_NoticeInfo[lang].index = 0;

	this->m_NoticeInfo[lang].Time = GetTickCount();

	this->m_NoticeInfo[lang].NoticeInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			NOTICE_DATA_INFO info;

			memset(&info,0,sizeof(info));

			strcpy_s(info.Message,lpReadFile->GetString());

			info.Type = lpReadFile->GetAsNumber();

			info.Count = lpReadFile->GetAsNumber();

			info.Opacity = lpReadFile->GetAsNumber();

			info.Delay = lpReadFile->GetAsNumber();

			info.Color = 0;
			
			info.Color |= lpReadFile->GetAsNumber();
			
			info.Color |= (lpReadFile->GetAsNumber() << 8);
			
			info.Color |= (lpReadFile->GetAsNumber() << 16);
			
			info.Color |= (info.Opacity << 24);

			info.Speed = lpReadFile->GetAsNumber();

			info.RepeatTime = lpReadFile->GetAsNumber()*1000;

			this->m_NoticeInfo[lang].NoticeInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CNotice::MainProc() // OK
{
	for(int n = 0; n < MAX_LANGUAGE; n++)
	{
		NOTICE_INFO* lpNotice = &this->m_NoticeInfo[n];

		if(lpNotice->NoticeInfo.empty() == 0)
		{
			NOTICE_DATA_INFO* lpInfo = &lpNotice->NoticeInfo[lpNotice->index];

			if((GetTickCount()-lpNotice->Time) >= ((DWORD)lpInfo->RepeatTime))
			{
				lpNotice->index = (((lpNotice->index+1)>=(int)lpNotice->NoticeInfo.size())?0:(lpNotice->index+1));
				lpNotice->Time = GetTickCount();
		
				for(int i=OBJECT_START_USER;i < MAX_OBJECT;i++)
				{
					if(gObjIsConnectedGP(i) != 0 && gObj[i].Lang == n)
					{
						this->GCNoticeSend(i,lpInfo->Type,lpInfo->Count,lpInfo->Opacity,lpInfo->Delay,lpInfo->Color,lpInfo->Speed,"%s",lpInfo->Message);
					}
				}
			}
		}
	}
}

void CNotice::GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CNotice::GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,int message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,gMessage->GetText(message,gObj[aIndex].Lang),arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CNotice::GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void CNotice::GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,int message,...) // OK
{
	PMSG_NOTICE_SEND pMsg;

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		char buff[256] = {0};

		va_list arg;
		va_start(arg,message);
		vsprintf_s(buff,gMessage->GetText(message,gObj[n].Lang),arg);
		va_end(arg);

		int size = strlen(buff);

		size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

		pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

		memcpy(pMsg.message,buff,size);

		pMsg.message[size] = 0;

		DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
	}
}