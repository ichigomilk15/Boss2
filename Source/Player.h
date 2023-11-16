#pragma once

#include "Common.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Camera.h"
#include "CameraController.h"
#include "Effect.h"

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
	//スティック入力値から移動ベクトルを取得
	DirectX::XMINT2 GetMoveVec() const;

	//移動入力しょり
	void InputMove(float elapsedTime);

	//ステート更新処理
	void UpdateState(float elapsedTime) override;

	//移動アクション更新処理
	void UpdateMove(float elapsedTime);

private:
	Model* model = nullptr;


	Effect* hitEffect = nullptr;
};