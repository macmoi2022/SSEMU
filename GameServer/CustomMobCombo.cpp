#include "stdafx.h"
#include "Util.h"
#include "ItemManager.h"
#include "Message.h"
#include "User.h"
#include "Path.h"
#include "CustomMobCombo.h"
#include "ReadFile.h"

CCustomMobCombo::CCustomMobCombo()
{
	this->m_count = 0;
}

CCustomMobCombo::~CCustomMobCombo()
{

}

void CCustomMobCombo::Load(char* path)
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

	for (int n = 0; n < MAX_MOB_COMBO; n++)
	{
		this->m_CustomMobCombo[n];
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

					this->m_CustomMobCombo[this->m_count].m_MonsterClass = lpReadFile->GetNumber();

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

CUSTOM_MOB_COMBO* CCustomMobCombo::GetMob(int MonsterClass)
{
	for (int n = 0; n < MAX_MOB_COMBO; n++)
	{
		if (this->m_CustomMobCombo[n].m_MonsterClass == MonsterClass)
		{
			return &this->m_CustomMobCombo[n];
		}
	}

	return 0;
}