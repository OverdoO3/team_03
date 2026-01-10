#pragma once
#include "Obj.h"

class Tower : public Obj
{
public:
	Tower();
	~Tower();

	void TowerApplyDamage(int num){ HP -= num; }
	int GetHP() { return HP; }
private:
	int HP = 1000;
};