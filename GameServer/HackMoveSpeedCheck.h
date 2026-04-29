// HackMoveSpeedCheck.h: interface for the CHackMoveSpeedCheck class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CHackMoveSpeedCheck
{
	CHackMoveSpeedCheck();
	virtual ~CHackMoveSpeedCheck();
	SingletonInstance(CHackMoveSpeedCheck)
public:
	void MainProc(LPOBJ lpObj);
	void Reset(LPOBJ lpObj);
};

#define gHackMoveSpeedCheck SingNull(CHackMoveSpeedCheck)