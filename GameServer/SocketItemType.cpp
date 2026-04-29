// SocketItemType.cpp: implementation of the CSocketItemType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketItemType.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketItemType::CSocketItemType() // OK
{
	this->m_SocketItemTypeInfo.clear();
}

CSocketItemType::~CSocketItemType() // OK
{

}

void CSocketItemType::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=401)

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

	this->m_SocketItemTypeInfo.clear();

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

			SOCKET_ITEM_TYPE_INFO info;

			memset(&info,0,sizeof(info));

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.MaxSocket = lpReadFile->GetAsNumber();

			this->m_SocketItemTypeInfo.insert(std::pair<int,SOCKET_ITEM_TYPE_INFO>(info.ItemIndex,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

bool CSocketItemType::CheckSocketItemType(int index) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	std::map<int,SOCKET_ITEM_TYPE_INFO>::iterator it = this->m_SocketItemTypeInfo.find(index);

	if(it == this->m_SocketItemTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}

	#else

	return 0;

	#endif
}

long CSocketItemType::GetSocketItemMaxSocket(int index) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	std::map<int,SOCKET_ITEM_TYPE_INFO>::iterator it = this->m_SocketItemTypeInfo.find(index);

	if(it == this->m_SocketItemTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.MaxSocket;
	}

	#else

	return 0;

	#endif
}
