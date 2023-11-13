#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Camera.h"
#include "CameraController.h"
#include "ProjectileManager.h"
#include "Effect.h"
//#include "SceneGame.h"

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

	const float getMoveSpeed() const { return moveSpeed; }
	const DirectX::XMFLOAT3 getVelocity() const { return velocity; }
	void setMoveSpeed(float speed) { moveSpeed = speed; }
	const float getTurnSpeed() const { return turnSpeed; }
	void setTurnSpeed(float speed) { turnSpeed = speed; }

	ProjectileManager getProjectileManagaer() { return projectileManager; }

	void CollisionProjectilesVsEnemies();

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	//移動処理
	//void Move(float elapsedTime, float vx, float vz, float speed);
	// ジャンプ処理
	//void Jump(float speed);
	// 速力処理更新
	//void UpdateVelocity(float elapsedTime);
	//旋回処理
	//void Turn(float elapsedTime, float vx, float vz, float speed);
	
	//移動入力しょり
	void InputMove(float elapsedTime);
	// ジャンプ入力処理
	void InputJump();

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// 弾丸入力処理
	void InputProjectile();

private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	//CameraController* cameraController = nullptr;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;
	//float gravity = -1.0f;
	//DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	Effect* hitEffect = nullptr;
};