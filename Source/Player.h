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

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	const float getMoveSpeed() const { return moveSpeed; }
	const DirectX::XMFLOAT3 getVelocity() const { return velocity; }
	void setMoveSpeed(float speed) { moveSpeed = speed; }
	const float getTurnSpeed() const { return turnSpeed; }
	void setTurnSpeed(float speed) { turnSpeed = speed; }

	ProjectileManager getProjectileManagaer() { return projectileManager; }

	void CollisionProjectilesVsEnemies();

protected:
	// ���n�������ɌĂ΂��
	void OnLanding() override;

private:
	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;
	//�ړ�����
	//void Move(float elapsedTime, float vx, float vz, float speed);
	// �W�����v����
	//void Jump(float speed);
	// ���͏����X�V
	//void UpdateVelocity(float elapsedTime);
	//���񏈗�
	//void Turn(float elapsedTime, float vx, float vz, float speed);
	
	//�ړ����͂����
	void InputMove(float elapsedTime);
	// �W�����v���͏���
	void InputJump();

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayerVsEnemies();

	// �e�ۓ��͏���
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