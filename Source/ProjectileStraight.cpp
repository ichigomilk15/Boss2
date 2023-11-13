#include "ProjectileStraight.h"

//ProjectileStraight::ProjectileStraight()
//{
//	model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
//
//	// 表示サイズを調整
//	scale.x = scale.y = scale.z = 0.5f;
//}

ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
	: Projectile(manager)
{
	//model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
	model = new Model("Data/Model/Sword/Sword.mdl");

	// 表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;
}

ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

void ProjectileStraight::Update(float elapsedTime)
{
	// 寿命処理
	lifeTimer -= elapsedTime /*1/60.0f*/;
	if (lifeTimer <= 0.0f)
	{
		// 自分を削除
		Destroy();
	}

	// 移動
	float speed = this->speed * elapsedTime;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
