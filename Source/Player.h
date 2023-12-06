#pragma once

#include "Common.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Camera.h"
#include "CameraController.h"
#include "Effect.h"
#include "CardList.h"
#include "Audio\AudioSource.h"

class Player : public Character
{
public:
	Player();
	//Player(CameraController* cameraController);
	~Player() override;

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	//void Render2D(RenderContext& rc,ID3D11DeviceContext* dc);//2D関係の描画
	void DrawDebugGUI();

	//デバッグプリミティブ描画
	void DrawDebugPrimitive();

	void SetTurnPosInit(const DirectX::XMINT2& pos) { this->turnPosInit = pos; }
private:
	//ステート更新処理
	void UpdateState(float elapsedTime) override;

	//移動アクション更新処理
	void UpdateMove(float elapsedTime);
	//移動中更新処理
	State MovingEnd();

	//攻撃アクション更新処理
	void UpdateAttack(float elapsedTime);

	//攻撃の初期化
	void InitializeAttack(const int damage, const std::vector<DirectX::XMINT2>& posAttack, const float timer);
	void InitializeKnockbackAttack(const int damage, const int knockbackCost, const int knockbackDir, const std::vector<DirectX::XMINT2>& posAttack, const float timer);
	//シールドカードのアクション
	void SetShieldAction();
	//デバフカードのアクション
	void SetDebuffAction();

	//セットカードによるアクションを決める更新処理
	State ChooseAct(float elapsedTime);

	//ダメージを受けた時
	void OnDamaged() override;
	
	//死亡した時に呼ばれる
	void OnDead() override;

	//ステージからのカードを取得処理
	void GetCard(Card* getCard);

	//敵の詳細を見る更新処理
	void UpdateViewEnemyDetail();

	//オーディオの初期化
	void InitializeAudio();

private:
	//Model* model = nullptr;
	Effect* hitEffect = nullptr;
	CardComboDataBase* cardComboDataBase;
	DirectX::XMINT2 turnPosInit;			//ターン開始の位置

	struct LookAtEnemyDetail
	{
		bool isLookAtEnemyDetail = false;
		Character* target = nullptr;
		std::vector<DirectX::XMINT2> targetAttackRange{};
	} lookAtEnemyDetail;
	struct PlayerSes
	{
		std::unique_ptr<AudioSource> attackSe;
		std::unique_ptr<AudioSource> damageSe;
		std::unique_ptr<AudioSource> deadSe;
		std::unique_ptr<AudioSource> shieldSe;
		std::unique_ptr<AudioSource> moveSe;
	}playerSes;
};