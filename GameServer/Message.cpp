// Message.cpp: implementation of the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Message.h"
#include "ServerInfo.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessage::CMessage() // OK
{
	memset(this->m_DefaultMessage,0,sizeof(this->m_DefaultMessage));

	for(int n = 0; n < MAX_LANGUAGE; n++)
	{
		this->m_MessageInfo[n].clear();
	}
}

CMessage::~CMessage() // OK
{

}

void CMessage::Load(char* path,int lang) // OK
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

	this->m_MessageInfo[lang].clear();

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

			MESSAGE_INFO info;

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Message,lpReadFile->GetAsString());

			this->m_MessageInfo[lang].insert(std::pair<int,MESSAGE_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

char* CMessage::GetText(int index,int lang) // OK
{
	if(CHECK_RANGE(lang,MAX_LANGUAGE) != 0)
	{
		std::map<int,MESSAGE_INFO>::iterator it = this->m_MessageInfo[lang].find(index);

		if(it != this->m_MessageInfo[lang].end())
		{
			return it->second.Message;
		}
	}

	wsprintf(this->m_DefaultMessage,"Could not find message: %d, lang: %d!",index,lang);
	return this->m_DefaultMessage;
}


char* CMessage::GetMessage(int index) // OK
{
	if (CHECK_RANGE(gServerInfo->m_ServerLang, MAX_LANGUAGE) != 0)
	{
		std::map<int, MESSAGE_INFO>::iterator it = this->m_MessageInfo[gServerInfo->m_ServerLang].find(index);

		if (it != this->m_MessageInfo[gServerInfo->m_ServerLang].end())
		{
			return it->second.Message;
		}
	}

	wsprintf(this->m_DefaultMessage, "Could not find message: %d, lang: %d!", index, gServerInfo->m_ServerLang);
	return this->m_DefaultMessage;
}