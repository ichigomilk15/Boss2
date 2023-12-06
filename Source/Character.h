#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"
#include "HitCollisions.h"

#include "Common.h"
#include "AttackParent.h"
#include "Collision.h"

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
	AttackCharge_Init, //攻撃のチャージ
	AttackCharge,
	Attack_Init, //攻撃更新処理
	Attack,
	Attacking_Init, //攻撃している状態
	Attacking,
	AttackingJump_Fly_Init, //ジャンプをして、攻撃準備をしている状態
	AttackingJump_Fly,
	AttackingJump_Stump_Init, //ヒップドロップ・スタンプ攻撃状態
	AttackingJump_Stump,
	Defence_Init, //防御アップ
	Defence,
	Special_Init, //Specialカードの使用
	Special,
	Debuff_Init, //Debuffカードの使用
	Debuff,
	Damage_Init, //ダメージを受けた処理
	Damage,
	KnockedBack_Init, //ノックバックされた処理
	KnockedBack,
	Stunned_Init, //スタンされる状態
	Stunned,
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
	Character();
	virtual ~Character();

	//行列更新処理
	void UpdateTransform();

	//ステータスのリセット
	virtual void ResetStatus();

	//移動目標位置が妥当か
	bool IsTargetMovePosValid(const DirectX::XMINT2& targetPos);
	bool IsTargetMoveAttackPosValid(const DirectX::XMINT2& targetPos);

	bool IsInTheSameRow(const Character& target);
	bool IsInTheSameColumn(const Character& target);

	//回復
	void Heal(const int hp);

	//キャラクターのサイズによって指定位置が左右かの判定
	int GetWhichHorizontalSide(const DirectX::XMINT2& pos);
	//キャラクターのサイズによって指定位置が上下かの判定
	int GetWhichVerticalSide(const DirectX::XMINT2& pos);

	void CancelChargeAttack();

	//位置取得
	const DirectX::XMFLOAT3& GetPositionWorld() const { return positionWorld; }
	const DirectX::XMINT2& GetPosition() const { return position; }
	const DirectX::XMINT2& GetTargetMovePosition() const { return targetMovePos; }

	//位置設定
	void SetPositionWorld(const DirectX::XMINT2& position);

	//ターゲット位置設定
	void SetTargetMovePosition(const DirectX::XMINT2& targetPos) { targetMovePos = targetPos; }
	void SetTargetMoveTranslation(const DirectX::XMINT2& moveDir);

	//立っているマス位置を取得
	const std::vector<DirectX::XMINT2> GetSquaresPosition() const;
	const std::vector<DirectX::XMINT2> GetSquaresPositionX(const int row) const;
	const std::vector<DirectX::XMINT2> GetSquaresPositionY(const int column) const;

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

	//健康状態を設定
	void SetHealth(const int hp) { this->health = hp; }
	//健康状態取得
	int GetHealth() const { return health; }

	//最大健康状態を設定
	void SetMaxHealth(const int maxHp) { this->maxHealth = maxHp; }
	//最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }

	void Render2D(ID3D11DeviceContext* dc, const HitBox2D& box);

	//Getter&Setter*****************************************************
#if 1
	//サイズの設定
	void SetSize(DirectX::XMINT2 size) { this->size = size; }
	//サイズの取得
	const DirectX::XMINT2 GetSize() const { return size; }

	//pivot位置の設定
	void SetPivotAdjustPosWorld(DirectX::XMFLOAT3 pivot) { pivotAdjustPosWorld = pivot; }

	const int& Getshield()const { return shield; }

	const bool GetIsActEnd() const { return isActEnd; }

	const Sprite* GetIcon()const noexcept { return icon.get(); }

	void ShowDamageNumber( const int damageNumber );

	//レイキャスト用
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

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
	DirectX::XMFLOAT3 pivotAdjustPosWorld = { 0, 0, 0 };
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
	int maxHealth = 75;
	int attackPower = 0; //ノーマルアタック力
	int shield = 0; //一時的な防御
	int block = 0; //攻撃を受けた数値を減少するステータス・ブロック
	DirectX::XMINT2 size = { 1, 1 };

	State state = State::Idle_Init;
	float actTimer = 0; //ステート遷移タイマー
	bool isActEnd = false;		//ターンにアクションが終わったの判定

	DirectX::XMINT2 targetMovePos = { -1, -1 }; //移動するターゲットの位置
	float moveTimer = 0.0f;
	int moveRange = 2; //移動力（マス数）
	//int attackAdjacentRange = 0;

	AttackParent* attack = nullptr;
	bool isAttackCharging = false; //1ターン以上かかる攻撃の準備の状態
	int attackChargeTurn = 0;

	bool isGround = true;

	//HPバー関係の値
	std::unique_ptr<Sprite> icon;

	std::unique_ptr<Model> model;

private:
	std::unique_ptr<Sprite> hpBar[3];
public:
	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};