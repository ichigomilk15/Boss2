#include "PhaseManager.h"

#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"
#include "EnemyMinion1.h"
#include "Stage.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG


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
		phase = Phase::Phase_GameStart;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_GameStart:
	{
		phase = Phase::Phase_Start_Init;
	}
	break;
	case PhaseManager::Phase::Phase_Start_Init:
	{
		//�J�[�h���ő�l�܂ň���
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();

		PlayerManager::Instance().GetFirstPlayer()->ResetStatus();
		EnemyManager::Instance().ResetTurnEnemies();

		phase = Phase::Phase_Start;
		phaseTimer = 1.0f;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//�����𖞂����Ă�Ԏ��Ԃ����炷
		if (!CardManager::Instance().IsMoving())
		{
			phaseTimer -= elapsedTime;
		}
		//�����𖞂����Ȃ������烊�Z�b�g
		else
		{
			phaseTimer = 1.0f;
		}

		//�������������������玟�̃t�F�[�Y��
		if (phaseTimer < 0.0f)
		{
			CardManager::Instance().SetIsMoveable(true);
			phase = Phase::Phase_Player_Init;
		}
	}
	break;
	case PhaseManager::Phase::Phase_Player_Init:
	{
		phase = Phase::Phase_Player;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		if (CardManager::Instance().IsSetCardsFinished())
		{
			phase = Phase::Phase_PlayerAct_Init;
			PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
			break;
		}
	}
	break;
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		useCardIndex = 0u;
		phase = Phase::Phase_PlayerAct;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		UpdatePlayerAct(elapsedTime);
		if (PlayerManager::Instance().GetFirstPlayer()->GetState() == State::Act_Finish)
		{
			phase = Phase::Phase_Enemy_Init;
			break;
		}
	}
	break;
	case PhaseManager::Phase::Phase_Enemy_Init:
	{
		phaseTimer = 1.0f;
		Stage::Instance()->ResetAllSquare();
		phase = Phase::Phase_Enemy;
		break;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Enemy:
	{
		phaseTimer -= elapsedTime;
		if (phaseTimer < 0.0f)
		{
			phase = Phase::Phase_EnemyAct_Init;
			break;
		}
	}
	break;
	case PhaseManager::Phase::Phase_EnemyAct_Init:
	{
		Stage::Instance()->ResetAllSquare();
		phase = Phase::Phase_EnemyAct;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_EnemyAct:
	{
		if (EnemyManager::Instance().GetIsAllActEnd())
		{
			phase = Phase::Phase_End_Init;
			break;
		}
	}
	break;

	case PhaseManager::Phase::Phase_End_Init:
	{
		phaseTimer = 3.0f;
		phase = Phase::Phase_End;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{
		phaseTimer -= elapsedTime;
		if (phaseTimer < 0.0f)
		{
			ResetTurn();
			break;
		}
	}
	break;

	default:
		break;
	}
}

void PhaseManager::ResetTurn()
{
	phase = Phase::Phase_Start_Init;
	phaseTimer = 0.0f;
	++turnCount;
}

void PhaseManager::Reset()
{
	phase = Phase::Phase_GameStart_Init;
	turnCount = 0u;
	StageLevel = 0;
	phaseTimer = .0f;
}

void PhaseManager::DrawDebugGUI()
{
#ifdef _DEBUG
	if (ImGui::Begin("PhaseManager", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
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
	}
	ImGui::End();
#endif // _DEBUG
}

void PhaseManager::SetGameStart()
{
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	player->SetPositionWorld({ 3, 3 });
	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	EnemyMinion1* enemy = new EnemyMinion1(player);
	EnemyManager::Instance().Register(enemy);
	enemy->SetPositionWorld({ 1, 1 });
	enemy->SetTargetMovePosition({ -1, -1 });
	enemy->SetState(State::Idle_Init);

	Stage::Instance()->ResetAllSquare();
}

void PhaseManager::UpdatePlayerAct(float elapsedTime)
{
	/*CardManager::Instance().GetIsMoveable
	switch()*/
}
