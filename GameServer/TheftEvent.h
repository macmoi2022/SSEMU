#pragma once

#include "User.h"

struct sTheft
{
	int                 _Day;
	int                 _Hours;
	int                 _Minutes;
	int                 _TimeOpen;
	int                 _Duration;
};

struct sTheftPlayer
{
	bool                _InEvent;
	bool                _AttackBlock;
	int                 _Kills;
};


class cTheftEvent
{
public:
	cTheftEvent();

public:
	bool Load(char* path);
	bool Check(LPOBJ lpObj);
	void Run();
	bool Attack(LPOBJ lpObj, LPOBJ Target);
	void Killer(LPOBJ lpObj, LPOBJ Target);
	void Winner();
	void Finish();
	void Quit(LPOBJ lpObj);
	DWORD GetRemainingTime();
	DWORD GetNextEventTime();
public:
	bool                _Active;
	char	            _Syntax[25];
	bool                _Patente;
	bool                _Portal;
	bool                _Progress;
	int                 _Level;
	int                 _Zen;
	BYTE                _MapNumber;
	BYTE                _X;
	BYTE                _Y;
	int                 _Count;
	int                 _Value;
	int                 _Players;
	char                _Notice[255];
	char                _Buffer[255];

public:
	sTheft			    TheftStruct[255];

	sTheftPlayer	    PlayerStruct[MAX_OBJECT];
};

extern cTheftEvent TheftEvent;