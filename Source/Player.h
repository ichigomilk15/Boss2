#pragma once

#include "Common.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Camera.h"
#include "CameraController.h"
#include "Effect.h"
#include "CardList.h"
#include "Audio\AudioSource.h"

class Player : public Character
{
public:
	Player();
	//Player(CameraController* cameraController);
	~Player() override;

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	//void Render2D(RenderContext& rc,ID3D11DeviceContext* dc);//2D�֌W�̕`��
	void DrawDebugGUI();

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	void SetTurnPosInit(const DirectX::XMINT2& pos) { this->turnPosInit = pos; }

	const float GetPlayerDeadTime() const { return playerDeadTime; }
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
	//�V�[���h�J�[�h�̃A�N�V����
	void SetShieldAction();
	//�f�o�t�J�[�h�̃A�N�V����
	void SetDebuffAction();

	//�Z�b�g�J�[�h�ɂ��A�N�V���������߂�X�V����
	State ChooseAct(float elapsedTime);

	//�_���[�W���󂯂���
	void OnDamaged() override;
	
	//���S�������ɌĂ΂��
	void OnDead() override;

	//�X�e�[�W����̃J�[�h���擾����
	void GetCard(Card* getCard);

	//�G�̏ڍׂ�����X�V����
	void UpdateViewEnemyDetail();

	//�I�[�f�B�I�̏�����
	void InitializeAudio();

private:
	//Model* model = nullptr;
	Effect* hitEffect = nullptr;
	std::unique_ptr<Effect> buffEffect;
	std::unique_ptr<Effect> shieldEffect;
	CardComboDataBase* cardComboDataBase;
	DirectX::XMINT2 turnPosInit;			//�^�[���J�n�̈ʒu

	struct LookAtEnemyDetail
	{
		bool isLookAtEnemyDetail = false;
		Character* target = nullptr;
		std::vector<DirectX::XMINT2> targetAttackRange{};
	} lookAtEnemyDetail;
	struct PlayerSes
	{
		std::unique_ptr<AudioSource> attackSe;
		std::unique_ptr<AudioSource> damageSe;
		std::unique_ptr<AudioSource> deadSe;
		std::unique_ptr<AudioSource> shieldSe;
		std::unique_ptr<AudioSource> moveSe;

		std::unique_ptr<AudioSource> buff;
	}playerSes;

	struct CardSes
	{
		std::unique_ptr<AudioSource> cardDecideSe;
		std::unique_ptr<AudioSource> cardDraw;
		std::unique_ptr<AudioSource> cardSet;
	}cardSes;

	bool playerDeadTime = 0.0f;
};