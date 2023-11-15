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

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();
private:
	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMINT2 GetMoveVec() const;

	//�ړ����͂����
	void InputMove(float elapsedTime);

	//�X�e�[�g�X�V����
	void UpdateState(float elapsedTime) override;

	//�ړ��A�N�V�����X�V����
	void UpdateMove(float elapsedTime);

private:
	Model* model = nullptr;


	Effect* hitEffect = nullptr;
};