#pragma once

#include "Graphics\Shader.h"
#include "Graphics\Graphics.h"
#include "ProjectileManager.h"
#include "Character.h"

//�G�l�~�[
class Enemy : public Character
{
public:
	enum class ENEMY_TYPE {
		NONE = -1,
		BOSS1, //0
	};

	ENEMY_TYPE enemyType = ENEMY_TYPE::NONE;

	Enemy(Character* p)
		: Character(),
		player(p)
	{}
	~Enemy()override {}

	//�X�V����
	virtual void Update(float elapsedTime);

	//�`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	//�j��
	void Destroy();

	//���S���ɌĂ�
	void OnDead()override;

	bool GetIsDestroyed() { return destroyedStatus.isDestroyed; }

	virtual void ResetStatus() override;

	//�U���͈͂�ݒ�
	void SetAttackRange(const int range) { this->attackRange = range; }
	//�U���͈͂��擾
	const int GetAttackRange() const { return this->attackRange; }

protected:
	//�ړ��̍X�V����
	virtual bool ChooseTargetMove(float elapsedTime);

	//�X�e�[�g�X�V����
	void UpdateState(float elapsedTime) override {}
	//�U���A�N�V�����X�V����
	virtual void InitializeAttack(float elapsedTime);

	//�Z�b�g�J�[�h�ɂ��A�N�V���������߂�X�V����
	virtual State ChooseAct(float elapsedTime);

protected:
	std::unique_ptr<Model> model;

	int moveMax = 2;
	int actMax = 3;				//1�^�[���ɍs����
	int actNo = 0;				//�s���񐔁E�^�[���n�܂�Ƀ��Z�b�g

	int attackRange = 2;

	struct DestroyedStatus
	{
		std::unique_ptr<Effect> destroyedEffect;
		float destroyedTime = 0.0f;
		bool isDestroyed = false;
	} destroyedStatus;
protected:
	Character* player;
};