#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics\Graphics.h"

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}