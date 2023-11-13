#include "EnemySlime.h"

EnemySlime::EnemySlime()
{
	model = new Model("Data/Model/Slime/Slime.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;
	position.x = 1;
	radius = 0.5f;
	height = 1.0f;
}

EnemySlime::~EnemySlime()
{
	delete model;
}

void EnemySlime::Update(float elapsedTime)
{
	UpdateVelocity(elapsedTime);

	//無効時間更新
	UpdateInvisibleTimer(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform(transform);
}

void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void EnemySlime::OnDead()
{
	Destroy();
}
