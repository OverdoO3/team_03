#include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
	Clear();
}

void ProjectileManager::Update(float elapsedTime)
{
	//XVˆ—
	for (Projectile* pro : projectiles)
	{
		pro->Update(elapsedTime);
	}
	for (Projectile* projectile : removes)
	{
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}
		delete projectile;
	}
	removes.clear();
}

void ProjectileManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//•`‰æ
	for (Projectile* pro : projectiles)
	{
		pro->Render(rc,renderer);
	}
}

void ProjectileManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Projectile* pro : projectiles)
	{
		pro->RenderDebugPrimitivbe(rc, renderer);
	}
}

void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

void ProjectileManager::Clear()
{
	for (Projectile* pro : projectiles)
	{
		delete pro;
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	removes.insert(projectile);
}
