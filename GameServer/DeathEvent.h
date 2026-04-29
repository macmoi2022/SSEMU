#pragma once

#include "User.h"

struct sDeath
{
	int                 _Day;
	int                 _Hours;
	int                 _Minutes;
	int                 _TimeOpen;
	int                 _Duration;
};

struct sDeathPlayer
{
	bool                _InEvent;
	bool                _AttackBlock;
	int                 _Kills;
};

class cDeathEvent
{
public:
	cDeathEvent();
public:
	bool Load(char* path);
	bool Check(LPOBJ lpObj);
	void Run();
	bool Attack(LPOBJ lpObj, LPOBJ Target);
	void Killer(LPOBJ lpObj, LPOBJ Target);
	void Winner();
	void Finish();
	void Quit(LPOBJ lpObj);
	int GetRemainingTime();
	int GetNextEventTime();
public:
	bool                _Active;
	bool                _Patente;
	char	            _Syntax[25];
	bool                _Portal;
	bool                _Progress;
	int                 _Level;
	BYTE                _MapNumber;
	BYTE                _X;
	BYTE                _Y;
	int                 _List;
	int                 _Value;
	int                 _Players;
	char                _Buffer[255];

public:
	sDeath			    DeathStruct[255];
	sDeathPlayer	    PlayerStruct[MAX_OBJECT];

};

extern cDeathEvent DeathEvent;