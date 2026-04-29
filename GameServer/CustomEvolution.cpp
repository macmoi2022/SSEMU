//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomEvolution.h"
#include "ReadFile.h"
#include "ItemManager.h"
#include "Message.h"
#include "Notice.h"
#include "Path.h"
#include "ScheduleManager.h"
#include "Util.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomEvo::CCustomEvo() // OK
{
	this->m_CustomEvo.clear();
}

CCustomEvo::~CCustomEvo() // OK
{

}

void CCustomEvo::Load(char* path) // OK
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

	this->m_CustomEvo.clear();

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

			CUSTOMEVO_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(), lpReadFile->GetAsNumber()));

			info.ResultIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(), lpReadFile->GetAsNumber()));

			this->m_CustomEvo.insert(std::pair<int, CUSTOMEVO_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}