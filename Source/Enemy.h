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
	{}
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

protected:
	//移動の更新処理
	virtual bool UpdateMove(float elapsedTime);

	//ステート更新処理
	void UpdateState(float elapsedTime) override {}


protected:
	std::unique_ptr<Model> model;

	int moveMax = 2;

	struct DestroyedStatus
	{
		std::unique_ptr<Effect> destroyedEffect;
		float destroyedTime = 0.0f;
		bool isDestroyed = false;
	} destroyedStatus;
protected:
	Character* player;
};