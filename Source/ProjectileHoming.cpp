#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* manager) :
	Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");
	scale.x = scale.y = scale.z = 3.0f;
}

ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

void ProjectileHoming::Update(float elapsedTime)
{
	
	// 寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		// 自分を削除
		Destroy();
	}

	//移動
	{
		float moveSpeed = this->moveSpeed * elapsedTime;
		position.x += direction.x * moveSpeed;
		position.y += direction.y * moveSpeed;
		position.z += direction.z * moveSpeed;
	}

	//旋回
	{
		float turnSpeed = this->turnSpeed * elapsedTime;
		//ターゲットまでベクトルを算出
		DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

		//ゼロベクトルでないなら回転処理
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, LengthSq);
		if (lengthSq > 0.00001f)
		{
			//ターゲットまでのベクトルを単位ベクトル化
			Vec = DirectX::XMVector3Normalize(Vec);
			//向いている方向ベクトルを算出
			DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
			/*DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR subtract = DirectX::XMVectorSubtract(Direction, Pos);*/
			
			//前方向ベクトルとターゲットまでのベクトルの内積（角度）を算出
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

			float dot;
			DirectX::XMStoreFloat(&dot, Dot);

			//２つの単位ベクトルの角度が小さいほど
			//1.0に近づくという性質を利用して回転速度を調整する
			float rot = 1 - dot;
			rot = min(rot, turnSpeed);
			/*if (rot > turnSpeed)
			{
				rot = turnSpeed;
			}*/
			//float angle = DirectX::XMConvertToDegrees(rot);

			//回転角度があるなら回転処理をする
			if (fabsf(rot) > 0.0001f/*rot >= 0.0f && rot <= 2.0f*/)
			{
				//回転軸を算出
				DirectX::XMVECTOR Axis;
				Axis = DirectX::XMVector3Cross(Direction, Vec);
				Axis = DirectX::XMVector3Normalize(Axis); //It is better to normalize

				//回転軸と回転量から回転行列を算出
				DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot /** turnSpeed*/);

				//現在の行列を回転させる
				DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
				Transform = DirectX::XMMatrixMultiply(Transform, Rotation);
				//DirectX::XMStoreFloat4x4(&transform, Transform);

				//回転後の前方向を取り出し、単位ベクトル化する
				//Direction = DirectX::XMVectorSet(transform._31, transform._32, transform._33, 0.0f); 書き方１
				Direction = DirectX::XMVector3Normalize(Transform.r[2]); //書き方２
				DirectX::XMStoreFloat3(&direction, Direction);
			}
		}
	}

	UpdateTransform();
	model->UpdateTransform(transform);
}

void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3 direction, const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 target)
{
	this->direction = direction;
	this->position = position;
	this->target = target;

	UpdateTransform();
	model->UpdateTransform(transform);
}
