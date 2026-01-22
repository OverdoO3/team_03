#pragma once
#include "Obj.h"
#include "Character.h"

class Tower : public Obj
{
public:
	Tower();
	~Tower();

	void TowerApplyDamage(int num){ HP -= num; }
	int GetHP() { return HP; }

	void Update(float elapsedTime);

	void PlayAnimation(int index, bool loop);
	void PlayAnimation(const char* name, bool loop);
	void UpdateAnimation(float elapsedTime);

protected:
	int									HP = 10;
	std::unique_ptr<Effect> explosion;
	Effekseer::Handle expHandle = -1;
private:
	int									animationIndex = -1;
	float								animationSeconds = 0.0f;
	bool								animationLoop = false;
	bool								animationPlaying = false;
	float								animationBlendSecondsLength = 0.2f;

};