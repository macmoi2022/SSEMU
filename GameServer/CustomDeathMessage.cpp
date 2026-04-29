// CustomDeathMessage.cpp: implementation of the CGate class.//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CommandManager.h"
#include "CustomDeathMessage.h"
#include "Log.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CustomDeathMessage::CustomDeathMessage() // OK
{
	this->m_CustomDeathMessage.clear();
}


CustomDeathMessage::~CustomDeathMessage() // OK
{


}


void CustomDeathMessage::Load(char* path) // OK
{

	CReadFile* lpReadFile = new CReadFile;

	if (lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR, path);
		return;
	}


	if (lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}


	this->m_CustomDeathMessage.clear();


	try
	{

		while (true)
		{
			if (lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}


			if (strcmp("end", lpReadFile->GetString()) == 0)
			{
				break;
			}


			CUSTOMDEATHMESSAGE_INFO info;


			info.Index = lpReadFile->GetNumber();


			strcpy_s(info.Text, lpReadFile->GetAsString());


			this->m_CustomDeathMessage.insert(std::pair<int, CUSTOMDEATHMESSAGE_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}


	delete lpReadFile;
}


void CustomDeathMessage::GetDeathText(LPOBJ lpTarget, LPOBJ lpObj, int index) // OK
{

	CUSTOMDEATHMESSAGE_INFO CustomDM;

	if (this->GetInfo(index, &CustomDM) == 0)
	{
		return;
	}

	GCChatTargetSend(lpTarget, lpObj->Index, CustomDM.Text);

}


bool CustomDeathMessage::GetInfo(int index, CUSTOMDEATHMESSAGE_INFO* lpInfo) // OK
{

	std::map<int, CUSTOMDEATHMESSAGE_INFO>::iterator it = this->m_CustomDeathMessage.find(index);

	if (it == this->m_CustomDeathMessage.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}