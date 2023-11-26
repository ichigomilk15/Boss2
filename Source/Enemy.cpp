#include "Enemy.h"
#include <EnemyManager.h>

void Enemy::Update(float elapsedTime)
{
	// 速力処理更新
	UpdateVelocity(elapsedTime);

	//ステート更新処理
	UpdateState(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);
	//モデル行列更新
	model->UpdateTransform(transform);
}

void Enemy::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

void Enemy::DrawDebugGUI()
{
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::OnDead()
{
}

void Enemy::ResetStatus()
{
	Character::ResetStatus();
	actNo = 0;
	isActEnd = false;
}

bool Enemy::UpdateMove(float elapsedTime)
{
	return false;
}
