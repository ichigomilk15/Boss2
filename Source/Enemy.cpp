#include "Enemy.h"
#include <EnemyManager.h>

void Enemy::Update(float elapsedTime)
{
	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	//�X�e�[�g�X�V����
	UpdateState(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);
	//���f���s��X�V
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
