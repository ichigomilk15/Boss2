#pragma once

#include "Graphics\Shader.h"
#include "Graphics\Graphics.h"
#include "ProjectileManager.h"
#include "Character.h"

//エネミー
class Enemy : public Character
{
public:
	enum class ENEMY_TYPE {
		NONE = -1,
		BOSS1, //0
	};

	ENEMY_TYPE enemyType = ENEMY_TYPE::NONE;

	Enemy(Character* p)
		: Character(),
		player(p)
	{
		icon = std::make_unique<Sprite>("./Data/Sprite/icon_enemy.png");
	}
	~Enemy()override {}

	//更新処理
	virtual void Update(float elapsedTime);

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//破棄
	void Destroy();

	//死亡時に呼ぶ
	void OnDead()override;

	bool GetIsDestroyed() { return destroyedStatus.isDestroyed; }

	virtual void ResetStatus() override;

	//攻撃範囲を設定
	void SetAttackRange(const int range) { this->attackRange = range; }
	//攻撃範囲を取得
	const int GetAttackRange() const { return this->attackRange; }

	const float& GetHpBarUseScale()const noexcept { return hpBarUseScale; }

protected:
	//移動の更新処理
	virtual bool ChooseTargetMove(float elapsedTime);

	//ステート更新処理
	void UpdateState(float elapsedTime) override {}
	//攻撃アクション更新処理
	virtual void InitializeAttack(float elapsedTime);

	//セットカードによるアクションを決める更新処理
	virtual State ChooseAct(float elapsedTime);

	virtual void MakeHalfTransparent();
	virtual void MakeFullTransparent();

private:
	bool IsConcealPlayer();

protected:
	//std::unique_ptr<Model> model;

	int moveMax = 2;
	int actMax = 3;				//1ターンに行動数
	int actNo = 0;				//行動回数・ターン始まりにリセット

	int attackRange = 2;
	float hpBarUseScale = 0.5f;//HPバーの使用率

	struct DestroyedStatus
	{
		std::unique_ptr<Effect> destroyedEffect;
		float destroyedTime = 0.0f;
		bool isDestroyed = false;
	} destroyedStatus;//死亡時使用データ
protected:
	Character* player;
};