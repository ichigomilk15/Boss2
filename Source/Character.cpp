#include "Character.h"
#include <CustomMathf.h>

void Character::UpdateTransform()
{
	//スケール行列
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	//位置行列を作る
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//三つの行物を組み合わせ, ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行れつを取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}

void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);
	//水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);
	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
}

bool Character::ApplyDamage(int damage, float invicibleTime)
{
	//ダメージを０の場合は健康状態を変更する必要がない
	if (damage <= 0) return false;
	//死亡している場合は健康状態を変更しない
	if (health <= 0) return false;

	if (invisibleTimer > 0) return false;

	//ダメージ処理
	health -= damage;

	//死亡通知
	if (health <= 0)
	{
		OnDead();
	}
	//ダメージ通知
	else
	{
		invisibleTimer = invicibleTime;
		//UpdateInvisibleTimer(elapsedTime);
		OnDamaged();
	}

	//健康状態が変更した場合はtrueを返す
	return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//速力に力を与える
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// 垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// 重力処理
	velocity.y += gravity * elapsedFrame;
}

//垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	//乗直方向の移動量
	float my = velocity.y * elapsedTime;
	slopeRate = 0.0f;

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	//落下中
	if (my < 0.0f)
	{
		//レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffSet, position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

	}
	//上昇中
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}

	//地面の向きに沿うようにXZ軸回転
	{
		// Y軸が法線ベクトル方向に向くオイラー角回転を算出する
		float ax = atan2f(normal.z, normal.y);
		float az = -atan2f(normal.x, normal.y);

		//線形補完で滑らかなに回転する
		angle.x = CustomMathf::Lerp(angle.x, ax, 0.2f);
		angle.z = CustomMathf::Lerp(angle.z, az, 0.2f);
		/*angle.x = ax;
		angle.z = az;*/

		groundNormal = normal;
	}
}

//水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ平面の速力を減速する
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length != 0.0f)
		int i = 0;
	if (length > 0.0f)
	{
		float friction = this->friction * elapsedFrame;

		//空中にいるときは摩擦力を減らす
		//friction -= (isGround) ? 0 : airControl;
		if (isGround) friction *= airControl;

		//摩擦による横方向の減速処理
		if (length > friction)
		{
#if 0
			float length2 = length - friction;
			velocity.x *= length2 / length;
			velocity.z *= length2 / length;
			length = length2;
#else
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
#endif
		}
		//横方向の速力が摩擦力以下になったので速力を無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ平面の速力を加速する
	if (length <= maxMoveSpeed)
	{
		//移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			//加速力
			float acceleration = this->acceleration * elapsedFrame;

			//空中にいるときは加速力を減らす
			acceleration -= (isGround) ? 0 : airControl;

			//移動ベクトルによる加速処理
			//moveVecLength += acceleration;
			velocity.x += acceleration * moveVecX;
			velocity.z += acceleration * moveVecZ;

			//最大速度制限
			length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				//length = maxMoveSpeed;
				velocity.x /= length;
				velocity.z /= length;
				velocity.x *= maxMoveSpeed;
				velocity.z *= maxMoveSpeed;
			}

			//下り坂でガタガタしないようにする
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame;
			}
		}
	}
	//移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 移動処理
	/*position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;*/

	//水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.z * velocity.z + velocity.x * velocity.x);
	if (velocityLengthXZ > 0.0f)
	{
		//水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffSet, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffSet, position.z + mz };

		//レイキャストによる壁判定
	}

}

void Character::Move(float vx, float vz, float speed)
{
	/*speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;*/

	//移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;
	//最大速度設定
	maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//進行ベクトルがゼロベクトルの場合は処理する必要なし
	//if (vz <= 0.000000001f || vz >= -0.000000001f) return;
	//float temp = sqrtf(vx * vx + vz * vz);
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	//進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	//自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = vx * frontX + vz * frontZ;
	//float rot = cosf(dot);
	float rot = 1.0f - dot;

	//内積値は-1.0~1.0で表現させており、２つの単位ベクトルの角度が
	//小さいほど1.0に近づくという性質を利用して回転速度を調整する
	//speed *= rot;
	if (rot > speed) rot = speed;

	//左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = ((vx * frontZ) - (vz * frontX));

	//2Dの外積値が正の場合か負の場合によって左右判定が行える
	//左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y -= (rot);
	}
	else
	{
		angle.y += (rot);
	}

	/*
	{
		//DirectX::XMFLOAT3 frontF = { frontX, 0, frontZ };
		//DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&frontF);
		//DirectX::XMVECTOR Vec = DirectX::XMVectorSet(vx, 0, vz, 0);
		//DirectX::XMVECTOR Axis;
		//Axis = DirectX::XMVector3Cross(Front, Vec);

		////回転軸と回転量から回転行列を算出
		//DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

		////現在の行列を回転させる
		//DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
		//Transform = DirectX::XMMatrixMultiply(Transform, Rotation);
		//DirectX::XMStoreFloat4x4(&transform, Transform);

		////回転後の前方向を取り出し、単位ベクトル化する
		//Front = DirectX::XMVectorSet(transform._31, transform._32, transform._33, 0.0f);
		//DirectX::XMStoreFloat3(&direction, Direction);
	}
	*/
}

void Character::UpdateInvisibleTimer(float elapsedTime)
{
	if (invisibleTimer > 0.0f)
		invisibleTimer -= elapsedTime;
}
