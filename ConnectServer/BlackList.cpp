// BlackList.cpp: implementation of the CBlackList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlackList.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlackList::CBlackList() // OK
{
	this->m_BlackListIpInfo.clear();
}

void CBlackList::Load(char* path) // OK
{
	CReadFile* lpReadFile = new CReadFile;

	if (lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if (lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_BlackListIpInfo.clear();

	try
	{
		while (true)
		{
			if (lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end",lpReadFile->GetAsString()) == 0)
			{
				break;
			}

			BLACKLIST_IP_INFO info;
			
			strcpy_s(info.IpAddress,lpReadFile->GetString());

			this->m_BlackListIpInfo.insert(std::pair<std::string,BLACKLIST_IP_INFO>(std::string(info.IpAddress),info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CBlackList::CheckIpAddress(char* IpAddress) // OK
{
	std::map<std::string,BLACKLIST_IP_INFO>::iterator it = this->m_BlackListIpInfo.find(std::string(IpAddress));

	if (it == this->m_BlackListIpInfo.end())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
