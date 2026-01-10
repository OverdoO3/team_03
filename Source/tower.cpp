#include "tower.h"

Tower::Tower()
{
	model = std::make_unique<Model>("Data/Model/Stage/tower.mdl");

	scale = { 0.8f,0.8f,0.8f };
	UpdateTransfomEuler();
}

Tower::~Tower()
{
}
