#pragma once
#include <DirectXMath.h>

enum class State
{
	Idle_Init, //���ԑ҂�
	Idle,
	Act_Init,  //�^�[���ɃA�N�V������I��
	Act,
	Move_Init, //�ړ�
	Move,
	Max,
};

class Character
{
public:
	Character() {};
	virtual ~Character() {};

	//�s��X�V����
	void UpdateTransform();

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return positionWorld; }

	//�ʒu�ݒ�
	void SetPositionWorld(const DirectX::XMINT2 & position);

	//��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//�X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	const float GetHeight() const { return height; }

	//�_���[�W��^����
	bool ApplyDamage(int damage);

	//�Փ˂�^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	//���N��Ԏ擾
	int GetHealth() const { return health; }

	//�ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }

	//Getter&Setter*****************************************************
#if 1
#endif // 1
	//Getter&Setter*****************************************************

protected:
	//�ړ�����
	void Move(int vx, int vy);
	// ���͏����X�V
	void UpdateVelocity(float elapsedTime);
	//�_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {};
	//���S�������ɌĂ΂��
	virtual void OnDead() {};
	//�X�e�[�g�X�V����
	virtual void UpdateState(float elapsedTime) {};

protected:
	DirectX::XMFLOAT3 positionWorld = { 0, 0, 0 };
	DirectX::XMINT2 position = { 0, 0 }; //�}�X�̈ʒu�@X�EY
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	int movePosX = 0;
	int movePosY = 0;

	float height = 2.0f;
	int health = 1000;
	int maxHealth = 1000;

	State state;

public:

	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};