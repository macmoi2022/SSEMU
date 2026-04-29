#include "stdafx.h"
#include "Util.h"
#include "ItemManager.h"
#include "Message.h"
#include "User.h"
#include "Path.h"
#include "CustomGuard.h"
#include "ReadFile.h"

CCustomGuard::CCustomGuard()
{
	this->m_count = 0;
}

CCustomGuard::~CCustomGuard()
{

}

void CCustomGuard::Load(char* path) 
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

	this->m_count = 0;

	for (int n = 0; n < MAX_GUARD; n++)
	{
		this->m_CustomGuard[n];
	}

	try
	{
		while (true)
		{
			if (lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					this->m_CustomGuard[this->m_count].m_MonsterClass = lpReadFile->GetNumber();

					this->m_count++;
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

CUSTOM_GUARD* CCustomGuard::GetGuard(int MonsterClass)
{
	for (int n = 0; n < MAX_GUARD; n++)
	{
		if (this->m_CustomGuard[n].m_MonsterClass == MonsterClass)
		{
			return &this->m_CustomGuard[n];
		}
	}

	return 0;
}