#pragma once
#include "Common.h"
#include "AttackParent.h"
#include <DirectXMath.h>

enum Animation
{
	Idle,
	Run,
	Attack,
	Damage,
};

enum class State
{
	Idle_Init, //���ԑ҂�
	Idle,
	Act_Init,  //�^�[���ɃA�N�V�����̑I��
	Act,
	Move_Init, //�ړ��X�V����
	Move,
	Moving_Init, //�ړ����Ă�����
	Moving,
	Attack_Init, //�U���X�V����
	Attack,
	Attacking_Init, //�U�����Ă�����
	Attacking,
	Defence_Init, //�h��A�b�v
	Defence,
	Damage_Init, //�_���[�W���󂯂�����
	Damage,
	Act_Finish_Init, //�S�ẴA�N�V�������I���������
	Act_Finish,
	Max,
};

//enum class Status
//{
//	Shield,
//	Max_Status
//};

class Character
{
public:
	Character() {};
	virtual ~Character() {};

	//�s��X�V����
	void UpdateTransform();

	//�X�e�[�^�X�̃��Z�b�g
	virtual void ResetStatus();

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPositionWorld() const { return positionWorld; }
	const DirectX::XMINT2& GetPosition() const { return position; }

	//�ʒu�ݒ�
	void SetPositionWorld(const DirectX::XMINT2& position);

	//�^�[�Q�b�g�ʒu�ݒ�
	void SetTargetMovePosition(const DirectX::XMINT2& targetPos) { targetMovePos = targetPos; }

	//��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//�����ݒ�
	void SetDirection(int dir);
	void SetDirection(const DirectX::XMINT2 targetpos);
	const int GetDirection() const { return this->direction; }

	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//�X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	const float GetHeight() const { return height; }

	//�_���[�W��^����
	bool ApplyDamage(int damage);

	//�ړ������̔���
	bool IsMoving() const;

	//�U�������̔���
	//bool IsAttacking() const;

	//�Փ˂�^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	// �X�e�[�g�ݒ�
	void SetState(State state) { this->state = state; }

	//�X�e�[�g�擾
	const State GetState() const { return this->state; }

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
	int direction = CommonClass::DirectionFace::Back;
	int movePosX = 0;
	int movePosY = 0;

	float height = 2.0f;
	int health = 1000;
	int maxHealth = 1000;
	int attackPower = 0; //�A�^�b�N��
	int shield = 0; //�ꎞ�I�Ȗh��

	State state;
	float actTimer = 0; //�X�e�[�g�J�ڃ^�C�}�[

	DirectX::XMINT2 targetMovePos = { -1, -1 }; //�ړ�����^�[�Q�b�g�̈ʒu
	float moveTimer = 0.0f;
	int moveRange = 2; //�ړ��́i�}�X���j
	int attackAdjacentRange = 0;

	AttackParent* attack = nullptr;

public:

	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};