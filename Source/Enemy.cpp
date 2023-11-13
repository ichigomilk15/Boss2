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

	//�Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}