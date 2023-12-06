#pragma once
#include <DirectXMath.h>
#include "Graphics/Sprite.h"
#include "HitCollisions.h"

#include "Common.h"
#include "AttackParent.h"
#include "Collision.h"

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
	AttackCharge_Init, //�U���̃`���[�W
	AttackCharge,
	Attack_Init, //�U���X�V����
	Attack,
	Attacking_Init, //�U�����Ă�����
	Attacking,
	AttackingJump_Fly_Init, //�W�����v�����āA�U�����������Ă�����
	AttackingJump_Fly,
	AttackingJump_Stump_Init, //�q�b�v�h���b�v�E�X�^���v�U�����
	AttackingJump_Stump,
	Defence_Init, //�h��A�b�v
	Defence,
	Special_Init, //Special�J�[�h�̎g�p
	Special,
	Debuff_Init, //Debuff�J�[�h�̎g�p
	Debuff,
	Damage_Init, //�_���[�W���󂯂�����
	Damage,
	KnockedBack_Init, //�m�b�N�o�b�N���ꂽ����
	KnockedBack,
	Stunned_Init, //�X�^���������
	Stunned,
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
	Character();
	virtual ~Character();

	//�s��X�V����
	void UpdateTransform();

	//�X�e�[�^�X�̃��Z�b�g
	virtual void ResetStatus();

	//�ړ��ڕW�ʒu���Ó���
	bool IsTargetMovePosValid(const DirectX::XMINT2& targetPos);
	bool IsTargetMoveAttackPosValid(const DirectX::XMINT2& targetPos);

	bool IsInTheSameRow(const Character& target);
	bool IsInTheSameColumn(const Character& target);

	//��
	void Heal(const int hp);

	//�L�����N�^�[�̃T�C�Y�ɂ���Ďw��ʒu�����E���̔���
	int GetWhichHorizontalSide(const DirectX::XMINT2& pos);
	//�L�����N�^�[�̃T�C�Y�ɂ���Ďw��ʒu���㉺���̔���
	int GetWhichVerticalSide(const DirectX::XMINT2& pos);

	void CancelChargeAttack();

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPositionWorld() const { return positionWorld; }
	const DirectX::XMINT2& GetPosition() const { return position; }
	const DirectX::XMINT2& GetTargetMovePosition() const { return targetMovePos; }

	//�ʒu�ݒ�
	void SetPositionWorld(const DirectX::XMINT2& position);

	//�^�[�Q�b�g�ʒu�ݒ�
	void SetTargetMovePosition(const DirectX::XMINT2& targetPos) { targetMovePos = targetPos; }
	void SetTargetMoveTranslation(const DirectX::XMINT2& moveDir);

	//�����Ă���}�X�ʒu���擾
	const std::vector<DirectX::XMINT2> GetSquaresPosition() const;
	const std::vector<DirectX::XMINT2> GetSquaresPositionX(const int row) const;
	const std::vector<DirectX::XMINT2> GetSquaresPositionY(const int column) const;

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

	//���N��Ԃ�ݒ�
	void SetHealth(const int hp) { this->health = hp; }
	//���N��Ԏ擾
	int GetHealth() const { return health; }

	//�ő匒�N��Ԃ�ݒ�
	void SetMaxHealth(const int maxHp) { this->maxHealth = maxHp; }
	//�ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }

	void Render2D(ID3D11DeviceContext* dc, const HitBox2D& box);

	//Getter&Setter*****************************************************
#if 1
	//�T�C�Y�̐ݒ�
	void SetSize(DirectX::XMINT2 size) { this->size = size; }
	//�T�C�Y�̎擾
	const DirectX::XMINT2 GetSize() const { return size; }

	//pivot�ʒu�̐ݒ�
	void SetPivotAdjustPosWorld(DirectX::XMFLOAT3 pivot) { pivotAdjustPosWorld = pivot; }

	const int& Getshield()const { return shield; }

	const bool GetIsActEnd() const { return isActEnd; }

	const Sprite* GetIcon()const noexcept { return icon.get(); }

	void ShowDamageNumber( const int damageNumber );

	//���C�L���X�g�p
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

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
	DirectX::XMFLOAT3 pivotAdjustPosWorld = { 0, 0, 0 };
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
	int maxHealth = 75;
	int attackPower = 0; //�m�[�}���A�^�b�N��
	int shield = 0; //�ꎞ�I�Ȗh��
	int block = 0; //�U�����󂯂����l����������X�e�[�^�X�E�u���b�N
	DirectX::XMINT2 size = { 1, 1 };

	State state = State::Idle_Init;
	float actTimer = 0; //�X�e�[�g�J�ڃ^�C�}�[
	bool isActEnd = false;		//�^�[���ɃA�N�V�������I������̔���

	DirectX::XMINT2 targetMovePos = { -1, -1 }; //�ړ�����^�[�Q�b�g�̈ʒu
	float moveTimer = 0.0f;
	int moveRange = 2; //�ړ��́i�}�X���j
	//int attackAdjacentRange = 0;

	AttackParent* attack = nullptr;
	bool isAttackCharging = false; //1�^�[���ȏォ����U���̏����̏��
	int attackChargeTurn = 0;

	bool isGround = true;

	//HP�o�[�֌W�̒l
	std::unique_ptr<Sprite> icon;

	std::unique_ptr<Model> model;

private:
	std::unique_ptr<Sprite> hpBar[3];
public:
	DirectX::XMFLOAT3 groundNormal = { 0, 0, 0 };
};