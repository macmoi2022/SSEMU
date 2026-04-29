//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomNPCCommand.h"
#include "CommandManager.h"
#include "Map.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "Path.h"
#include "Util.h"
#include "CustomMove.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomNpcCommand::CCustomNpcCommand() // OK
{
	this->m_CustomNpcCommand.clear();
}

CCustomNpcCommand::~CCustomNpcCommand() // OK
{

}

void CCustomNpcCommand::Load(char* path) // OK
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

	this->m_CustomNpcCommand.clear();

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

			NPC_TYPE_INFO info;

			static int Indexagem = 0;

			info.Index = Indexagem++;

			info.MonsterClass = lpReadFile->GetNumber();

			info.Map = lpReadFile->GetAsNumber();

			info.X = lpReadFile->GetAsNumber();

			info.Y = lpReadFile->GetAsNumber();

			strcpy_s(info.Command, lpReadFile->GetAsString());

			this->m_CustomNpcCommand.insert(std::pair<int, NPC_TYPE_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CCustomNpcCommand::GetNpcCommand(LPOBJ lpObj, LPOBJ lpNpc) // OK
{
	for (std::map<int, NPC_TYPE_INFO>::iterator it = this->m_CustomNpcCommand.begin(); it != this->m_CustomNpcCommand.end(); it++)
	{
		if (it->second.MonsterClass == lpNpc->Class && it->second.Map == lpNpc->Map && it->second.X == lpNpc->X && it->second.Y == lpNpc->Y)
		{
			gCommandManager->ManagementCore(lpObj, it->second.Command);
			return 1;
		}
	}
	return 0;
}