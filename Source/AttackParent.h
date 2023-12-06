#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "Effect.h"
//#include "EnemyManager.h"
//#include "PlayerManager.h"

class Character;

enum class TargetAttackEnum
{
	Target_Player,
	Target_Enemy,
};

class AttackParent
{
public: //function
	AttackParent(Character* parent, const int damage, TargetAttackEnum target, const std::vector<DirectX::XMINT2> targetAttackPos = {}, const float damageTimer = 0.0f);
	virtual ~AttackParent();

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �j��
	void Destroy();

	//parent
	Character* GetParent()const { return parent; }

	const bool GetIsDestroy()const { return isDestroy; }

	//Getter&Setter*************************************************************************
#if 1
	const std::vector<DirectX::XMINT2> GetTargetAttackPos() const { return targetAttackPos; }
#endif
protected: //function
	virtual void Initialize();

	virtual void AttackCard();

protected: //members

	std::vector<DirectX::XMINT2>        targetAttackPos = {  };
	DirectX::XMFLOAT3	    scale = { 1,1,1 };
	int                     damage = 1;
	float					damageTimer = 0.0f; //�_���[�W��^����^�C�}�[
	float					lifeTimer = 0.0f; //�p������܂ł̊���
	bool                    isDestroy = false;
	bool					isCardAttacked = false; //�}�X�ɂ���J�[�h���U���������̔���
	Character* parent;

	struct TargetAttack
	{
		Character* targetChara;
		bool isAttacked = false;
	};
	std::vector<TargetAttack> targetAttack = {};
	/*std::vector<Character*> targetAttack = {};
	std::vector < std::map<Character*, bool> IsTargetAttacked;*/
};