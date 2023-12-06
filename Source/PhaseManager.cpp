#include "PhaseManager.h"

#include "Input/Input.h"
#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"
#include "EnemyMinion1.h"
#include "EnemyBoss1.h"
#include "Stage.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG

PhaseManager::PhaseManager()
{
	const DirectX::XMFLOAT2& screenSize = Graphics::Instance().GetScreenSize();
	okButtonCollision = HitBox2D(DirectX::XMFLOAT2(screenSize.x * 0.8f, screenSize.y * 0.85f),
		DirectX::XMFLOAT2(screenSize.x * 0.1f, screenSize.y * 0.1f));
	okButton = std::make_unique<Sprite>("./Data/Sprite/OK.png");
	phaseTimer = NEXT_PHASE_WAIT_TIMER;
}

void PhaseManager::Initialize()
{
	Reset();
}

void PhaseManager::Update(float elapsedTime)
{
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
		//PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);

		//�X�e�[�W�̃��x�����Q�Ƃ���enemy���Z�b�g����
		Stage::Instance()->StageLevelStepUp();
		StageInit(Stage::Instance()->GetStageLevel());
		Stage::Instance()->ResetAllSquare();


		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_NextStage:
	{
		NextPhase();//���̃t�F�[�Y��
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
		if (IsQuickNextPhase(mouse.GetButtonDown()&Mouse::BTN_LEFT&&
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

		//enemy���S������ł�����t�F�[�Y��phase_nextstage_init�@�ɕύX
		if (IsSlowNextPhase(EnemyManager::Instance().GetIsAllDead()))
		{
			ChangePhase(Phase::Phase_NextStage_Init);
		}


		//�J�[�h�u����̃J�[�h���Ȃ��Ȃ��
		if (IsQuickNextPhase(CardManager::Instance().IsSetCardsEmpty()&&isPlayseActFinished))
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
		//enemy���S���s�����������Ă�����
		if (IsQuickNextPhase(EnemyManager::Instance().GetIsAllActEnd()))
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
		const DirectX::XMFLOAT4 color = phase==Phase::Phase_Player&& CardManager::Instance().IsFillSetCards()?
			DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }://���܂��Ă���Ƃ�
			DirectX::XMFLOAT4{.5f,.5f,.5f,1.0f};//���܂��Ă��Ȃ��Ƃ�
		okButton->Render(dc, lefttop, Size, .0f, color);
		
	}
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
	ChangePhase(static_cast<Phase>(static_cast<int>(phase) + 1%static_cast<int>(Phase::Phase_Max)));
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
	case 3:
	{
		PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);
		//enemy�̔z�u
		EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy);
		enemy->SetPositionWorld({ 1, 1 });
		enemy->SetTargetMovePosition({ -1, -1 });
		enemy->SetState(State::Idle_Init);
		enemy->SetAttackRange(1);
		enemy->SetHealth(30);
		enemy->SetMaxHealth(30);
		enemy->SetDirection(CommonClass::DirectionFace::Left);
		Stage::Instance()->GetSquare(0, 0)->SetCard(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CardManager::CARD_SIZE, Card::Type::SPECIAL));
	}
	break;
	case 2:
	{
		//enemy�̔z�u
		EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy);
		enemy->SetPositionWorld({ 6, 6 });
		enemy->SetTargetMovePosition({ -1, -1 });
		enemy->SetState(State::Idle_Init);
		enemy->SetAttackRange(1);
		enemy->SetHealth(30);
		enemy->SetMaxHealth(30);
	}
	break;
	case 1:
	{
		//enemy�̔z�u
		EnemyMinion1* enemy = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy);
		enemy->SetPositionWorld({ 1, 1 });
		enemy->SetTargetMovePosition({ -1, -1 });
		enemy->SetState(State::Idle_Init);
		enemy->SetAttackRange(1);
		enemy->SetHealth(30);
		enemy->SetMaxHealth(30);

		EnemyMinion1* enemy2 = new EnemyMinion1(PlayerManager::Instance().GetFirstPlayer());
		EnemyManager::Instance().Register(enemy2);
		enemy2->SetPositionWorld({ 6, 6 });
		enemy2->SetTargetMovePosition({ -1, -1 });
		enemy2->SetState(State::Idle_Init);
		enemy2->SetAttackRange(2);
		enemy2->SetHealth(40);
		enemy2->SetMaxHealth(40);
	}
		break;
	case 4:
	{
		auto player = PlayerManager::Instance().GetFirstPlayer();
		EnemyBoss1* boss1 = new EnemyBoss1(player);
		EnemyManager::Instance().Register(boss1);
		DirectX::XMINT2 pos;
		pos.x = (player->GetPosition().x > 4) ? 0 : 5;
		pos.y = (player->GetPosition().y > 4) ? 0 : 5;
		//boss1->SetPositionWorld(pos);
		boss1->SetPositionWorld({0, 2});
		boss1->SetTargetMovePosition({ -1, -1 });
		boss1->SetSize({ 2, 2 });
		DirectX::XMFLOAT3 pivot = { Common::SquareWidth / 2, 1.0f, -Common::SquareHeight / 2 };
		boss1->SetPivotAdjustPosWorld(pivot);
		boss1->SetState(State::Attack_Init);
		boss1->SetHealth(200);
		boss1->SetMaxHealth(200);
		boss1->SetAttackRange(4);
	}	
		break;

	}

	EnemyManager::Instance().SetStartEnemyNum();
}
