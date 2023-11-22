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

protected:
	//�ړ��̍X�V����
	virtual bool UpdateMove(float elapsedTime);

	//�X�e�[�g�X�V����
	void UpdateState(float elapsedTime) override {}


protected:
	std::unique_ptr<Model> model;

	int moveMax = 2;

	struct DestroyedStatus
	{
		std::unique_ptr<Effect> destroyedEffect;
		float destroyedTime = 0.0f;
		bool isDestroyed = false;
	} destroyedStatus;
protected:
	Character* player;
};