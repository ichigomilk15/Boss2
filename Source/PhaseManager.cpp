
#define NOMINMAX
#include "PhaseManager.h"
#include "Audio\AudioLoader.h"
#include "Input/Input.h"
#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"
#include "EnemyMinion1.h"
#include "EnemyBoss1.h"
#include "Stage.h"
#include "SceneManager.h"
#include "SceneGameOver.h"
#include "SceneClear.h"
#include "SceneGame.h"
#include "DamageEffector.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG
#undef NOMINMAX

PhaseManager::PhaseManager()
{
	const DirectX::XMFLOAT2& screenSize = Graphics::Instance().GetScreenSize();
	okButtonCollision = HitBox2D(DirectX::XMFLOAT2(screenSize.x * 0.1f, screenSize.y * 0.87f),
		DirectX::XMFLOAT2(screenSize.x * 0.1f, screenSize.y * 0.1f));
	okButton = std::make_unique<Sprite>("./Data/Sprite/OK.png");
	phaseTimer = NEXT_PHASE_WAIT_TIMER;

	waveSprites[0] = std::make_unique<Sprite>("./Data/Sprite/WAVE_01.png");
	waveSprites[1] = std::make_unique<Sprite>("./Data/Sprite/WAVE_02.png");
	waveSprites[2] = std::make_unique<Sprite>("./Data/Sprite/WAVE_03.png");

	waveChangeData.sprite = std::make_unique<Sprite>("./Data/Sprite/wave_change.png");
	waveChangeData.offsetY = screenSize.y * 0.2f;
	waveChangeData.DepthNum = screenSize.y / waveChangeData.offsetY;

	InitializeAudio();
}

void PhaseManager::Initialize()
{
	Reset();
}

void PhaseManager::Update(float elapsedTime)
{
	if (SceneManager::Instance().IsWaitSceneChange())return;
	switch (phase)
	{
	case PhaseManager::Phase::Phase_GameStart_Init:
	{
		SetGameStart();
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_GameStart:
	{
		NextPhase();//���̃t�F�[�Y��
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_NextStage_Init:
	{
		const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
		//PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);


		unsigned int stageLevel = Stage::Instance()->StageLevelStepUp();
		if (!SceneManager::Instance().IsWaitSceneChange()&& stageLevel > Stage::STAGE_LEVEL_MAX)
		{
			SceneManager::Instance().SetSceneChangeSprite(std::make_unique<Sprite>("./Data/Sprite/SceneBack/clear_Back.png"));
			SceneManager::Instance().ChangeScene(new SceneClear,true);
			return;
		}



		if (!SaveData::Instance().Save())
		{
			//�Z�[�u�Ɏ��s������
		}



		StartWaveChange();
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_NextStage:
	{
		if (waveChangeData.oldIsLoadOk != waveChangeData.isLoadOk)
		{
			//�X�e�[�W�̃��x�����Q�Ƃ���enemy���Z�b�g����
			StageInit(Stage::Instance()->GetStageLevel());
			Stage::Instance()->ResetAllSquare();
		}

		if (IsQuickNextPhase(waveChangeData.isChangeOk))
		{
			NextPhase();//���̃t�F�[�Y��
		}
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_Start_Init:
	{
		//�J�[�h���ő�l�܂ň���
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();
		cardManager.ResetPrevType();

		Player* player = PlayerManager::Instance().GetFirstPlayer();
		player->ResetStatus();
		player->SetTurnPosInit(player->GetPosition());

		EnemyManager::Instance().ResetTurnEnemies();

		//�z�u���Ă���J�[�h�̈ړ�
		Stage::Instance()->ReFleshCard();


		//�ŏ��̃^�[���ł͎��s���Ȃ�
		if (turnCount > 0)
		{
		}

		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//�������������������玟�̃t�F�[�Y��
		if (IsSlowNextPhase(!CardManager::Instance().IsMoving()))
		{
			CardManager::Instance().SetIsMoveable(true);
			NextPhase();//���̃t�F�[�Y��
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_Player_Init:
	{
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//ok�{�^�����������玟�̃t�F�[�Y��
		if (IsQuickNextPhase(mouse.GetButtonDown() & Mouse::BTN_LEFT &&
			okButtonCollision.Hit(mouse.GetPosition()) && CardManager::Instance().IsFillSetCards()))
		{
			CardManager::Instance().SetIsMoveable(false);
			PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
			NextPhase();//���̃t�F�[�Y��
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		useCardIndex = 0u;
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		const bool isPlayseActFinished = PlayerManager::Instance().GetFirstPlayer()->GetIsActEnd();

		UpdatePlayerAct(elapsedTime);

		if (EnemyManager::Instance().GetIsAllDead())
		{
			auto player = PlayerManager::Instance().GetFirstPlayer();
			player->SetState(State::Act_Finish_Init);
			CardManager::Instance().SetCardsClear();

			//enemy���S������ł�����t�F�[�Y��phase_nextstage_init�@�ɕύX
			if (IsSlowNextPhase(EnemyManager::Instance().GetIsAllDestroyed()))
			{
				ChangePhase(Phase::Phase_NextStage_Init);
			}
		}
		//�J�[�h�u����̃J�[�h���Ȃ��Ȃ��
		else if (IsQuickNextPhase(CardManager::Instance().IsSetCardsEmpty() && isPlayseActFinished))
		{
			NextPhase();//���̃t�F�[�Y��
		}

	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_Enemy_Init:
	{
		Stage::Instance()->ResetAllSquare();
		NextPhase();
		break;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Enemy:
	{
		//��莞�ԑҋ@���Ď��̃t�F�[�Y��
		if (IsSlowNextPhase(true))
		{
			NextPhase();
		}
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_EnemyAct_Init:
	{
		Stage::Instance()->ResetAllSquare();
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_EnemyAct:
	{
		//�Q�[���I�[�o�[����
		bool isGameOver = false;
		bool isPlayerDead = false;
		for (auto& player : *PlayerManager::Instance().GetPlayerVector())
		{
			if (player->GetIsDead())
			{
				if (player->GetIsDestroyed()) {
					isGameOver = true; 
					break;
				}
				isPlayerDead = true;
			}
		}
		if (!SceneManager::Instance().IsWaitSceneChange()&& isGameOver)
			SceneManager::Instance().ChangeScene(new SceneGameOver);

		//enemy���S���s�����������Ă�����
		if (!isPlayerDead && IsQuickNextPhase(EnemyManager::Instance().GetIsAllActEnd()))
		{
			NextPhase();
		}
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_End_Init:
	{
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{
		//todo : ���炩�̉��o�Ȃ�
		if (IsSlowNextPhase(true))
		{
			ResetTurn();
			break;
		}
	}
	break;
	default:
		break;
	}

	//�S�Ẵt�F�[�Y�Ŏ��s�������
	phaseTimer = (isNextPhase ? phaseTimer - elapsedTime : NEXT_PHASE_WAIT_TIMER);
	isNextPhase = false;
	auto&& data = waveChangeData;
	if (!data.isChangeOk)
	{
		data.timer += elapsedTime;
		data.oldIsLoadOk = data.isLoadOk;
		if (data.timer > data.TIMER_MAX * 0.5f)
		{
			data.isLoadOk = true;
		}
		if (data.timer > data.TIMER_MAX)
		{
			data.timer = .0f;
			data.isChangeOk = true;
		}
	}
}

void PhaseManager::ResetTurn()
{
	ChangePhase(Phase::Phase_Start_Init);
	++turnCount;
}

void PhaseManager::Render(ID3D11DeviceContext* dc)
{
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();



	//�����{�^���`��
	{
		const DirectX::XMFLOAT2& lefttop = okButtonCollision.GetLeftTop();
		const DirectX::XMFLOAT2& Size = okButtonCollision.GetBoxSize();
		//�J�[�h���S�ăZ�b�g����Ă�����
		const DirectX::XMFLOAT4 color = phase == Phase::Phase_Player && CardManager::Instance().IsFillSetCards() ?
			DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f } ://���܂��Ă���Ƃ�
			DirectX::XMFLOAT4{ .5f,.5f,.5f,1.0f };//���܂��Ă��Ȃ��Ƃ�
		okButton->Render(dc, lefttop, Size, .0f, color);
		
	}
}

void PhaseManager::RenderWaveChange(ID3D11DeviceContext* dc)const
{
	if (phase != Phase::Phase_NextStage)return;//�E�F�[�u�̕ύX���ȊO�`�悵�Ȃ�

	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();//��ʃT�C�Y

	float parsent = waveChangeData.timer / waveChangeData.TIMER_MAX;
	DirectX::XMFLOAT2 pos = { .0f,ScreenSize.y - ScreenSize.y * 
		sinf(parsent * DirectX::XM_PI) };
	DirectX::XMFLOAT2 size = { ScreenSize.x ,ScreenSize.y * 0.4f };
	DirectX::XMFLOAT2 texSize = waveChangeData.sprite->GetTextureSize();

	for (size_t i = 0,end = waveChangeData.DepthNum; i < end; i++)
	{
		waveChangeData.sprite->Render(dc,
			pos, size, DirectX::XMFLOAT2{ texSize.x * parsent,.0f }, DirectX::XMFLOAT2{texSize.x *(end-i),texSize.y}, .0f, { 1.0f,1.0f,1.0f,1.0f });
		pos.y += waveChangeData.offsetY;
	}





	////wave�̐i�s�x�̕\��
	//HitBox2D box = HitBox2D::CreateBoxFromCenter({ ScreenSize.x * 0.5f,ScreenSize.y * 0.5f }, { ScreenSize.x,ScreenSize.y * 0.3f });
	//waveSprites[Stage::Instance()->GetStageLevel() - 1]->Render(dc,
	//	box.GetLeftTop(), box.GetBoxSize(), .0f, { 1.0f,1.0f,1.0f,1.0f });
}

void PhaseManager::Reset()
{
	turnCount = 0u;
	ChangePhase(Phase::Phase_GameStart_Init);
}

void PhaseManager::DrawDebugGUI()
{
#ifdef _DEBUG
	if (ImGui::Begin("PhaseManager", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		//�t�F�[�Y�̕\��
		const char* phaseName[static_cast<int>(Phase::Phase_Max)] =
		{
#define X(name) #name,
		PHASE_LIST
#undef X
		};
		if (ImGui::BeginCombo("PhaseState", phaseName[static_cast<int>(phase)], ImGuiWindowFlags_::ImGuiWindowFlags_None))
		{
			for (size_t i = 0; i < static_cast<size_t>(Phase::Phase_Max); i++)
			{
				bool is_select = i == static_cast<size_t>(phase);
				if (ImGui::Selectable(phaseName[i], is_select))
				{
					phase = static_cast<Phase>(i);
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::InputFloat("timer", &phaseTimer), .0f) {};
		if (ImGui::InputInt("turn", &this->turnCount)) {};
	}
	ImGui::End();
#endif // _DEBUG
}

void PhaseManager::NextPhase()
{
	ChangePhase(static_cast<Phase>(static_cast<int>(phase) + 1 % static_cast<int>(Phase::Phase_Max)));
}

void PhaseManager::ChangePhase(const Phase&& next)noexcept
{
	phaseTimer = NEXT_PHASE_WAIT_TIMER;
	phase = next;
}

void PhaseManager::SetGameStart()
{
	//player�̔z�u
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	DirectX::XMINT2 pos = { SaveData::Instance().playerpos.first,SaveData::Instance().playerpos.second };
	if (Stage::Instance()->IsInArea(pos.x, pos.y))
		player->SetPositionWorld(pos);
	else
		player->SetPositionWorld(Common::PlayerPosInit);

	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	//�Ֆʂ̃��Z�b�g
	Stage::Instance()->ResetAllSquare();
}

void PhaseManager::UpdatePlayerAct(float elapsedTime)
{

}

const bool PhaseManager::IsSlowNextPhase(const bool flag)
{
	isNextPhase |= flag;
	return phaseTimer < 0.f && flag;
}

const bool PhaseManager::IsQuickNextPhase(const bool flag)
{
	isNextPhase |= flag;
	return flag;
}

void PhaseManager::StageInit(const int level)
{
	switch (level)
	{
		//case 1:
		//{
		//	PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);
		//	//enemy�̔z�u
		//	EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		//	EnemyManager::Instance().Register(enemy);
		//	enemy->SetPositionWorld({ 1, 1 });
		//	enemy->SetTargetMovePosition({ -1, -1 });
		//	enemy->SetState(State::Idle_Init);
		//	enemy->SetAttackRange(1);
		//	enemy->SetHealth(30);
		//	enemy->SetMaxHealth(30);
		//	enemy->SetDirection(CommonClass::DirectionFace::Left);
		//	Stage::Instance()->GetSquare(0, 0)->SetCard(std::make_shared<Card>(CardManager::Instance().GetCardSpwnPos(), CardManager::CARD_SIZE, Card::Type::SPECIAL));
		//}
		//break;
	case 1:
	{
		PlayerManager::Instance().GetFirstPlayer()->SetTargetMovePosition({ 3,3 });
		PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld({ 3,3 });
		Stage::Instance()->ResetAllSquare();
		//enemy�̔z�u
		EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy);
		enemy->SetPositionWorld({ 5, 5 });
		enemy->SetTargetMovePosition({ -1, -1 });
		enemy->SetState(State::Idle_Init);
		enemy->SetAttackRange(1);
		enemy->SetHealth(30);
		enemy->SetMaxHealth(30);
	}
	break;
	case 2:
	{
		PlayerManager::Instance().GetFirstPlayer()->SetTargetMovePosition(Common::PlayerPosInit);
		PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);
		Stage::Instance()->ResetAllSquare();
		//enemy�̔z�u
		EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy);
		enemy->SetPositionWorld({ 1, 5 });
		enemy->SetTargetMovePosition({ -1, -1 });
		enemy->SetState(State::Idle_Init);
		enemy->SetAttackRange(1);
		enemy->SetHealth(30);
		enemy->SetMaxHealth(30);

		EnemyMinion1* enemy2 = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy2);
		enemy2->SetPositionWorld({ 5, 1 });
		enemy2->SetTargetMovePosition({ -1, -1 });
		enemy2->SetState(State::Idle_Init);
		enemy2->SetAttackRange(1);
		enemy2->SetHealth(30);
		enemy2->SetMaxHealth(30);
	}
	break;
	case 3:
	{
		PlayerManager::Instance().GetFirstPlayer()->SetTargetMovePosition({ 1,3 });
		PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld({ 1,3 });
		Stage::Instance()->ResetAllSquare();
		auto player = PlayerManager::Instance().GetFirstPlayer();
		EnemyBoss1* boss1 = new EnemyBoss1(player);
		EnemyManager::Instance().Register(boss1);
		DirectX::XMINT2 pos;
		pos.x = (player->GetPosition().x > 4) ? 0 : 5;
		pos.y = (player->GetPosition().y > 4) ? 0 : 5;
		//boss1->SetPositionWorld(pos);
		boss1->SetPositionWorld({ 3,3 });
		boss1->SetTargetMovePosition({ -1, -1 });
		boss1->SetSize({ 2, 2 });
		DirectX::XMFLOAT3 pivot = { Common::SquareWidth / 2, 1.0f, -Common::SquareHeight / 2 };
		boss1->SetPivotAdjustPosWorld(pivot);
		boss1->SetState(State::Attack_Init);
		boss1->SetHealth(150);
#ifdef _DEBUG//todo : debug �{�X�̗̑͂̏����̗͂�ݒ�
		boss1->SetHealth(150);
#endif // _DEBUG

		boss1->SetMaxHealth(150);
		boss1->SetAttackRange(4);
	}
	break;

	}


	EnemyManager::Instance().SetStartEnemyNum();
}

void PhaseManager::InitializeAudio()
{
}

void PhaseManager::StartWaveChange(float startTime)
{
	waveChangeData.timer = std::clamp(startTime,.0f,waveChangeData.TIMER_MAX);
	waveChangeData.isChangeOk = false;
	waveChangeData.isLoadOk = false;
	waveChangeData.oldIsLoadOk = false;
}
