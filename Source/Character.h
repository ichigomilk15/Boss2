#pragma once
#include "Common.h"
#include "AttackParent.h"
#include <DirectXMath.h>

enum class State
{
	Idle_Init, //順番待ち
	Idle,
	Act_Init,  //ターンにアクションの選択
	Act,
	Move_Init, //移動更新処理
	Move,
	Moving_Init, //移動している状態
	Moving,
	Attack_Init, //攻撃更新処理
	Attack,
	Attacking_Init, //攻撃している状態
	Attacking,
	Max,
};

class Character
{
public:
	Character() {};
	virtual ~Character() {};

	//行列更新処理
	void UpdateTransform();

	//位置取得
	const DirectX::XMFLOAT3& GetPositionWorld() const { return positionWorld; }
	const DirectX::XMINT2& GetPosition() const { return position; }

	//位置設定
	void SetPositionWorld(const DirectX::XMINT2 & position);

	//回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//方向設定
	void SetDirection(int dir);
	void SetDirection(const DirectX::XMINT2 targetpos);
	const int GetDirection() const { return this->direction; }

	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	const float GetHeight() const { return height; }

	//ダメージを与える
	bool ApplyDamage(int damage);

	//移動中かの判定
	bool IsMoving() const;

	//攻撃中かの判定
	bool IsAttacking() const;

	//衝突を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	// ステート設定
	void SetState(State state) { this->state = state; }

	//健康状態取得
	int GetHealth() const { return health; }

	//最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }

	//Getter&Setter*****************************************************
#if 1
#endif // 1
	//Getter&Setter*****************************************************

protected:
	//移動処理
	void Move(int vx, int vy);
	// 速力処理更新
	void UpdateVelocity(float elapsedTime);
	//ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {};
	//死亡した時に呼ばれる
	virtual void OnDead() {};
	//ステート更新処理
	virtual void UpdateState(float elapsedTime) {};

protected:
	DirectX::XMFLOAT3 positionWorld = { 0, 0, 0 };
	DirectX::XMINT2 position = { 0, 0 }; //マスの位置　X・Y
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	int direction = CommonClass::DirectionFace::Back;
	int movePosX = 0;
	int movePosY = 0;

	float height = 2.0f;
	int health = 1000;
	int maxHealth = 1000;
	int shield = 10;

	State state;

	DirectX::XMINT2 targetMovePos = { -1, -1 }; //移動するターゲットの位置
	float moveTimer = 0.0f;
	int moveRange = 2; //移動力（マス数）

	AttackParent* attack = nullptr;

public:

	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};