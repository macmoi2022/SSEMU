#include "stdafx.h"
#include "ObjectManager.h"
#include "CustomRespawn.h"
#include "ReadFile.h"
#include "Gate.h"
#include "Util.h"

CCustomRespawn::CCustomRespawn()
{
	this->m_Info.clear();
}

CCustomRespawn::~CCustomRespawn()
{

}

void CCustomRespawn::Load(char* Path)
{
	CReadFile* lpReadFile = new CReadFile;

	if (lpReadFile == NULL)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR, Path);
		return;
	}

	if (lpReadFile->SetBuffer(Path) == false)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->m_Info.clear();

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

			RESPAWN_MANAGER_INFO Info;

			memset(&Info, NULL, sizeof(Info));

			Info.m_GateMap = lpReadFile->GetNumber();

			Info.m_GateAccLevel[0] = lpReadFile->GetAsNumber();

			Info.m_GateAccLevel[1] = lpReadFile->GetAsNumber();

			Info.m_GateAccLevel[2] = lpReadFile->GetAsNumber();

			Info.m_GateAccLevel[3] = lpReadFile->GetAsNumber();

			this->m_Info.push_back(Info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CCustomRespawn::GetGate(LPOBJ lpObj, int MapIndex)
{
	for (std::vector<RESPAWN_MANAGER_INFO>::iterator it = this->m_Info.begin(); it != this->m_Info.end(); it++)
	{
		if (MapIndex == it->m_GateMap)
		{
			return it->m_GateAccLevel[lpObj->AccountLevel];
		}
	}

	return -2;
}


int CCustomRespawn::GetRespawnLocation(LPOBJ lpObj) //OK 
{
	bool result = 0;

	int gate, map, x, y, dir, level;

	int pMap = lpObj->Map;
	int pX = lpObj->X;
	int pY = lpObj->Y;
	int pdir = lpObj->Dir;

	int CustomGate = this->GetGate(lpObj, lpObj->Map);

	if (CustomGate == 0)
	{
		lpObj->Map = pMap;
		lpObj->X = pX;
		lpObj->Y = pY += (GetLargeRand() % 3);
		lpObj->Dir = pdir;
		return TRUE;
	}

	if (CustomGate == -1)
	{
		lpObj->Map = pMap;
		lpObj->X = pX;
		lpObj->Y = pY;
		lpObj->Dir = pdir;
		return TRUE;
	}

	if (CustomGate)
	{
		result = gGate->GetGate(CustomGate, &gate, &map, &x, &y, &dir, &level);
	}

	if (result != 0)
	{
		lpObj->Map = map;
		lpObj->X = x;
		lpObj->Y = y;
		lpObj->Dir = dir;
	}

	return result;
}