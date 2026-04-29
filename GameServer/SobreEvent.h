#pragma once

#include "User.h"

struct sSobreEvent
{
	int Day;
	int Hours;
	int Minutes;
	int Time;
};

class cSobreEvent
{
public:
	cSobreEvent();
	~cSobreEvent();

public:
	bool Load(char* path);
	bool GameMaster(LPOBJ lpObj, char* arg);
	bool Check(LPOBJ lpObj);
	void Start(int Time, int Class);
	void Run();
	bool Attack(LPOBJ lpObj, LPOBJ lpTargetObj);
	void Die(LPOBJ lpObj, LPOBJ lpTargetObj);
	void Quit(LPOBJ lpObj);
	int GetRemainingTime();
	int GetNextEventTime();

	bool				_Active;
	bool				_Patente;
	BYTE				_State;
	char	            _Syntax[25];

private:
	int					_Level;
	BYTE				_MapNumber;
	BYTE				_X;
	BYTE				_Y;
	BYTE				_Player;
	BYTE				_Type;
	int                 _List;
	int			        _Count;
	std::vector<LPOBJ>	_Players;

private:
	sSobreEvent			EventStruct[255];

public:
	enum State
	{
		Empty, Register, Progress, Fight, Final
	};
};

extern cSobreEvent SobreEvent;