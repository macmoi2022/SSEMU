// RaklionObjInfo.cpp: implementation of the CRaklionObjInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaklionObjInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionObjInfo::CRaklionObjInfo() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	this->Reset();
	
	#endif
}

CRaklionObjInfo::~CRaklionObjInfo() // OK
{

}

void CRaklionObjInfo::Reset() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	this->m_ObjCount = 0;

	for(int n=0;n < MAX_RAKLION_OBJ_INDEX;n++)
	{
		this->m_ObjIndex[n] = -1;
	}
	
	#endif
}

bool CRaklionObjInfo::AddObj(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	if(this->m_ObjCount < 0 || this->m_ObjCount >= MAX_RAKLION_OBJ_INDEX)
	{
		return 0;
	}

	this->m_ObjIndex[this->m_ObjCount++] = aIndex;
	return 1;
	
	#else

	return 0;

	#endif
}

int CRaklionObjInfo::GetCount() // OK
{
	#if(GAMESERVER_UPDATE>=401)

	return this->m_ObjCount;
	
	#else

	return 0;

	#endif
}
