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

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();
private:
	//�X�e�[�g�X�V����
	void UpdateState(float elapsedTime) override;

	//�ړ��A�N�V�����X�V����
	void UpdateMove(float elapsedTime);
	//�ړ����X�V����
	State MovingEnd();

	//�U���A�N�V�����X�V����
	void UpdateAttack(float elapsedTime);

	//�U���̏�����
	void InitializeAttack(const int damage, const std::vector<DirectX::XMINT2>& posAttack, const float timer);
	void InitializeKnockbackAttack(const int damage, const int knockbackCost, const int knockbackDir, const std::vector<DirectX::XMINT2>& posAttack, const float timer);

	//�Z�b�g�J�[�h�ɂ��A�N�V���������߂�X�V����
	State ChooseAct(float elapsedTime);

	//�_���[�W���󂯂���
	void OnDamaged() override;
private:
	Model* model = nullptr;
	Effect* hitEffect = nullptr;
	CardComboDataBase* cardComboDataBase;
};