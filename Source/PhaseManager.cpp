#include "PhaseManager.h"

#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG
#include <EnemyBoss1.h>
#include <Stage.h>


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
		//カードを最大値まで引く
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();

		phase = Phase::Phase_Start;
		phaseTimer = 1.0f;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//条件を満たしてる間時間を減らす
		if (!CardManager::Instance().IsMoving())
		{
			phaseTimer -= elapsedTime;
		}
		//条件を満たさなかったらリセット
		else
		{
			phaseTimer = 1.0f;
		}

		//条件を見たし続けたら次のフェーズへ
		if (phaseTimer < 0.0f)
		{
			CardManager::Instance().SetIsMoveable(true);
			phase = Phase::Phase_Player_Init;
		}
	}
	break;
	case PhaseManager::Phase::Phase_Player_Init:
	{
		PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
		phase = Phase::Phase_Player;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		auto& manager = CardManager::Instance();
		manager.GetIsMoveable();
	}
	break;
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		phase = Phase::Phase_PlayerAct;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		UpdatePlayerAct(elapsedTime); 
	}
		break;
	case PhaseManager::Phase::Phase_Enemy_Init:
	{
		for (auto& enemy : EnemyManager::Instance().GetList())
		{

		}
		phase = Phase::Phase_Enemy;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Enemy:
	{

	}
	break;
	case PhaseManager::Phase::Phase_End_Init:
	{

		phase = Phase::Phase_End;
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{

	}
	break;
	default:
		break;
	}
}

void PhaseManager::Reset()
{
	phase = Phase::Phase_GameStart_Init;
	trunCount = 0u;
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
	//todo : playerの配置
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	player->SetPositionWorld({ 3, 3 });
	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	//todo : enemyのスポーン
	EnemyBoss1* enemy = new EnemyBoss1(player);
	EnemyManager::Instance().Register(enemy);
	enemy->SetPositionWorld({ 1, 1 });
	enemy->SetTargetMovePosition({ -1, -1 });
	enemy->SetState(State::Idle_Init);

	Stage::Instance()->ResetAllSquare();
}

void PhaseManager::UpdatePlayerAct(float elapsedTime)
{

}
