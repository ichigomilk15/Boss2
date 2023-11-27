#pragma once
#include "Common.h"
#include "AttackParent.h"
#include <DirectXMath.h>

enum Animation
{
	Idle,
	Run,
	Attack,
	Damage,
};

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
	Defence_Init, //防御アップ
	Defence,
	Damage_Init, //ダメージを受けた処理
	Damage,
	Act_Finish_Init, //全てのアクションが終わった処理
	Act_Finish,
	Max,
};

//enum class Status
//{
//	Shield,
//	Max_Status
//};

class Character
{
public:
	Character() {};
	virtual ~Character() {};

	//行列更新処理
	void UpdateTransform();

	//ステータスのリセット
	virtual void ResetStatus();

	//位置取得
	const DirectX::XMFLOAT3& GetPositionWorld() const { return positionWorld; }
	const DirectX::XMINT2& GetPosition() const { return position; }

	//位置設定
	void SetPositionWorld(const DirectX::XMINT2& position);

	//ターゲット位置設定
	void SetTargetMovePosition(const DirectX::XMINT2& targetPos) { targetMovePos = targetPos; }

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
	//bool IsAttacking() const;

	//衝突を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	// ステート設定
	void SetState(State state) { this->state = state; }

	//ステート取得
	const State GetState() const { return this->state; }

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
	int attackPower = 0; //アタック力
	int shield = 0; //一時的な防御

	State state;
	float actTimer = 0; //ステート遷移タイマー

	DirectX::XMINT2 targetMovePos = { -1, -1 }; //移動するターゲットの位置
	float moveTimer = 0.0f;
	int moveRange = 2; //移動力（マス数）
	int attackAdjacentRange = 0;

	AttackParent* attack = nullptr;

public:

	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};