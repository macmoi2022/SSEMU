// Raklion.cpp: implementation of the CRaklion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaklionBattleUser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionBattleUser::CRaklionBattleUser() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	this->ResetData();
	
	#endif
}

CRaklionBattleUser::~CRaklionBattleUser() // OK
{

}

void CRaklionBattleUser::ResetData() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	this->m_Index = -1;
	this->m_IsUse = 0;
	
	#endif
}

void CRaklionBattleUser::SetIndex(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	this->m_Index = aIndex;
	this->m_IsUse = 1;
	
	#endif
}

BOOL CRaklionBattleUser::IsUseData() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	return this->m_IsUse;
	
	#else

	return 0;

	#endif
}

int CRaklionBattleUser::GetIndex() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	return this->m_Index;
	
	#else

	return -1;

	#endif
}
