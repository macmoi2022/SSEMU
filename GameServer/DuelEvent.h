#pragma once

#include "User.h"

struct sDuelEvent
{
	int Day;
	int Hours;
	int Minutes;
	int Time;
	int Class;
};

struct DuelPlayer
{
	bool Die;
	bool Quit;
	bool Select;
	BYTE Score;
	LPOBJ lpObj;
};

class cDuelEvent
{
public:
	cDuelEvent();
	~cDuelEvent();

public:
	bool Load(char* path);
	bool GameMaster(LPOBJ lpObj, char* arg);
	bool Check(LPOBJ lpObj);
	void Start(int Time, int Class);
	void Run();
	bool Attack(LPOBJ lpObj, LPOBJ lpTargetObj);
	void Die(LPOBJ lpObj);
	void Quit(LPOBJ lpObj);
	int GetRemainingTime();
	int GetNextEventTime();
	bool				_Active;
	bool				_Patente;
	char	            _Syntax[25];
	BYTE					_State;
	int						_Count;
private:
	bool                    _Sended;
	int						_Level;
	BYTE					_MapNumber[2];
	BYTE					_X[2];
	BYTE					_Y[2];
	BYTE					_Type;
	int                     _List;

	std::vector<DuelPlayer> _Players[2];
	DuelPlayer				_Selected[2];

private:
	sDuelEvent			    EventStruct[255];

public:
	enum State
	{
		Empty, Register, Select, Progress, Died, NextStage, WO, Final
	};
};

extern cDuelEvent DuelEvent;