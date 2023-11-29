#pragma once

#include "Common.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Camera.h"
#include "CameraController.h"
#include "Effect.h"
#include "CardList.h"

class Player : public Character
{
public:
	Player();
	//Player(CameraController* cameraController);
	~Player() override;

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	void DrawDebugGUI();

	//デバッグプリミティブ描画
	void DrawDebugPrimitive();
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

	//セットカードによるアクションを決める更新処理
	State ChooseAct(float elapsedTime);

	//ダメージを受けた時
	void OnDamaged() override;
private:
	Model* model = nullptr;
	Effect* hitEffect = nullptr;
	CardComboDataBase* cardComboDataBase;
};