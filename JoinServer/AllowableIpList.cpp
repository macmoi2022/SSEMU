// AllowableIpList.cpp: implementation of the CAllowableIpList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AllowableIpList.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAllowableIpList::CAllowableIpList() // OK
{
	this->m_AllowableIpInfo.clear();
}

CAllowableIpList::~CAllowableIpList() // OK
{

}

void CAllowableIpList::Load(char* path) // OK
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

	this->m_AllowableIpInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					ALLOWABLE_IP_INFO info;

					strcpy_s(info.IpAddr,lpReadFile->GetString());

					this->m_AllowableIpInfo.insert(std::pair<std::string,ALLOWABLE_IP_INFO>(std::string(info.IpAddr),info));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CAllowableIpList::CheckAllowableIp(char* ip) // OK
{
	std::map<std::string,ALLOWABLE_IP_INFO>::iterator it = this->m_AllowableIpInfo.find(std::string(ip));

	if(it == this->m_AllowableIpInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
