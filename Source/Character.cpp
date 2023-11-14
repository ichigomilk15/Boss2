#include "Character.h"
#include "StageManager.h"
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
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(positionWorld.x, positionWorld.y, positionWorld.z);

	//三つの行物を組み合わせ, ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行れつを取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	if (movePosX != 0)
	{
		this->position.x += movePosX;
		this->positionWorld.x += movePosX * 1.0f;
	}
	if (movePosY != 0)
	{
		this->position.y += movePosY;
		this->positionWorld.z += movePosY * 1.0f;
	}

#if 0
	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);
	//水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);
	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
#endif
}

bool Character::ApplyDamage(int damage)
{
	//ダメージを０の場合は健康状態を変更する必要がない
	if (damage <= 0) return false;
	//死亡している場合は健康状態を変更しない
	if (health <= 0) return false;

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
	//velocity.y += gravity * elapsedFrame;
}

//垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	//乗直方向の移動量
	float my = velocity.y * elapsedTime;

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	//落下中
	if (my < 0.0f)
	{
		//レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { positionWorld.x, positionWorld.y, positionWorld.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { positionWorld.x, positionWorld.y + my, positionWorld.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			normal = hit.normal;

			//地面に接地している
			//positionWorld.y = hit.positionWorld.y;
			positionWorld = hit.positionWorld;

			//回転
			//this->angle.x += hit.rotation.x;
			this->angle.y += hit.rotation.y;
			//this->angle.z += hit.rotation.z;
			
			// 傾斜率の計算
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);

			//着地した
			/*if (!isGround)
			{
				OnLanding();
			}*/
			//isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			//空中に浮いている
			positionWorld.y += my;
			//isGround = false;
		}
	}
	//上昇中
	else if (my > 0.0f)
	{
		positionWorld.y += my;
		//isGround = false;
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
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 移動処理
	/*positionWorld.x += velocity.x * elapsedTime;
	positionWorld.z += velocity.z * elapsedTime;*/

	//水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.z * velocity.z + velocity.x * velocity.x);
	if (velocityLengthXZ > 0.0f)
	{
		//水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { positionWorld.x, positionWorld.y, positionWorld.z };
		DirectX::XMFLOAT3 end = { positionWorld.x + mx, positionWorld.y, positionWorld.z + mz };

		//レイキャストによる壁判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(/*&start*/&hit.positionWorld);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			//壁の法線w
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
			Dot = DirectX::XMVectorScale(Dot, 1.1f);
			float dot = DirectX::XMVectorGetX(Dot);

			//補正位置の計算
			DirectX::XMVECTOR Pos = DirectX::XMVectorAdd(End, DirectX::XMVectorScale(Normal, dot));
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, Pos);

			HitResult hitSecondWall;
			if (StageManager::Instance().RayCast(start, pos, hitSecondWall))
			{
				DirectX::XMVECTOR Line = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitSecondWall.positionWorld), Start);

				auto CorrectPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&hit.positionWorld), DirectX::XMVectorScale(Line, 0.2f));
				DirectX::XMFLOAT3 correctPos;
				DirectX::XMStoreFloat3(&correctPos, CorrectPos);
				positionWorld.x = correctPos.x;
				positionWorld.z = correctPos.z;
			}
			else
			{
				positionWorld.x = DirectX::XMVectorGetX(Pos);
				positionWorld.z = DirectX::XMVectorGetZ(Pos);
			}
		}
		else
		{
			//移動
			positionWorld.x += mx;
			positionWorld.z += mz;
		}
	}

}

void Character::Move(int vx, int vy)
{
	//移動方向ベクトルを設定
	movePosX = vx;
	movePosY = vy;
}
