#pragma once
#include "tower.h"

class Nexus : public Tower
{
public:
	Nexus()
	{
		HP = 0;
	}

	void Update(float elapsedTime) override
	{
		Tower::Update(elapsedTime);
	}
};
