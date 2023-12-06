#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"
#include "Audio\AudioSource.h"

// 直線移動の敵(仮)
class EnemyMinion1 : public Enemy
{
public:
	EnemyMinion1(Character* p);

private:
	void UpdateState(float elapsedTime) override;

	//ダメージを受けた時
	void OnDamaged() override;

	//死亡した時に呼ばれる
	void OnDead() override;

	//オーディオの初期化
	void InitializeAudio();

private:
	struct Minion1Ses
	{
		std::unique_ptr<AudioSource> attackSe;
		std::unique_ptr<AudioSource> damageSe;
		std::unique_ptr<AudioSource> deathSe;
		std::unique_ptr<AudioSource> walkSe;
	}minion1Ses;
};


