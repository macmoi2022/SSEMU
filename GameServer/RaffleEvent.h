#pragma once

struct sRaffle
{
	int                     _Day;
	int                     _Hours;
	int                     _Minutes;
};

class cRaffleEvent
{
public:
	cRaffleEvent();

public:
	bool Load(char* path);
	void Run();

public:
	bool                    _Active;
	int                     _Count[2];
	int                     _Received[1000];

private:
	sRaffle			        RaffleStruct[255];
};

extern cRaffleEvent RaffleEvent;