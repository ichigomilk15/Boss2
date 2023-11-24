#include "PhaseManager.h"

#include "EnemyManager.h"
#include "CardList.h"

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
        phase = Phase::Phase_GameStart;
        //todo : enemyのスポーン
        //todo : playerの配置
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
        if (!CardManager::Instance().IsMoveing())
        {
            phaseTimer -= elapsedTime;
        }
        //条件を満たさなかったらリセット
        else
        {
            phaseTimer = 1.0f;
        }

        //条件を見たし続けたら次のフェーズへ
        if (phaseTimer < 0)
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
        auto& manager = CardManager::Instance();
        manager.GetIsMoveable();
    }
        break;
    case PhaseManager::Phase::Phase_Enemy_Init:
    {

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
    if (ImGui::Begin("PhaseManager",nullptr,ImGuiWindowFlags_::ImGuiWindowFlags_None))
    {
        const char* phaseName[static_cast<int>(Phase::Phase_Max)] =
        {
            "Phase_GameStart_Init",
            "Phase_GameStart",
            "Phase_NextStage_Init",
            "Phase_NextStage",
            "Phase_Start_Init",
            "Phase_Start",
            "Phase_Player_Init",
            "Phase_Player",
            "Phase_Enemy_Init",
            "Phase_Enemy",
            "Phase_End_Init",
            "Phase_End",
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
