#pragma once

#include "User.h"

class CAUTOHP
{
public:
	CAUTOHP();
	virtual ~CAUTOHP();

	void AutoHp(LPOBJ lpObj);
	void AutoMana(LPOBJ lpObj);
	void AutoAntidote(LPOBJ lpObj);
	void AutoComplex(LPOBJ lpObj);

	void MainProc();
private:

};

extern CAUTOHP gAutoHP;