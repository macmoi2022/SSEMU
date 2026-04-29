#pragma once
#include "User.h"

struct sRoletaPlayer
{
	bool                _InEvent;
	int                 _Index;
};

class cRoletaEvent
{
public:
	cRoletaEvent();
	~cRoletaEvent();

public:
	bool Load(char* path);
	bool GameMaster(LPOBJ lpObj, char* arg);
	bool Check(LPOBJ lpObj);
	void Start(int Time);
	void Run();
	bool Attack(LPOBJ lpObj, LPOBJ lpTargetObj);
	bool Trade(int Target, int aIndex);
	static void Fire(int aIndex);
	void Die(LPOBJ Target);
	void Quit(LPOBJ lpObj);

	char				_Syntax[25];

private:
	bool				_Active;

	int					_Level;
	BYTE				_MapNumber;
	BYTE				_X;
	BYTE				_Y;
	BYTE				_State;
	BYTE				_Type;
	int			        _Count;
	int                 _Time;
	std::vector<LPOBJ>	_Players;

public:
	sRoletaPlayer	    PlayerStruct[MAX_OBJECT];

private:
	enum State
	{
		Empty, Register, Progress, Waiting, Final
	};
};
extern cRoletaEvent RoletaEvent;