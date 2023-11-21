#pragma once
#include <DirectXMath.h>

enum class State
{
	Idle_Init, //順番待ち
	Idle,
	Act_Init,  //ターンにアクションを選ぶ
	Act,
	Move_Init, //移動
	Move,
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
	const DirectX::XMFLOAT3& GetPosition() const { return positionWorld; }

	//位置設定
	void SetPositionWorld(const DirectX::XMINT2 & position);

	//回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	const float GetHeight() const { return height; }

	//ダメージを与える
	bool ApplyDamage(int damage);

	//衝突を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

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
	int movePosX = 0;
	int movePosY = 0;

	float height = 2.0f;
	int health = 1000;
	int maxHealth = 1000;

	State state;

public:

	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};