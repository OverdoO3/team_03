#include "tower.h"

Tower::Tower()
{
	model = std::make_unique<Model>("Data/Model/Stage/tower.mdl");

	scale = { 0.5f,0.5f,0.5f };
	UpdateTransfomEuler();
}

Tower::~Tower()
{
}

void Tower::Update(float elapsedTime)
{

}
