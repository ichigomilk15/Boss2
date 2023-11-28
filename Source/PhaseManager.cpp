#include "PhaseManager.h"

#include "Input/Input.h"
#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"
#include "EnemyMinion1.h"
#include "Stage.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG

PhaseManager::PhaseManager()
{
	const DirectX::XMFLOAT2& screenSize = Graphics::Instance().GetScreenSize();
	okButtonCollision = HitBox2D(DirectX::XMFLOAT2(screenSize.x * 0.8f, screenSize.y * 0.85f),
		DirectX::XMFLOAT2(screenSize.x * 0.1f, screenSize.y * 0.1f));
	okButton = std::make_unique<Sprite>();//todo : okボタンの画像読み込み

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
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_GameStart:
	{
		NextPhase();//次のフェーズへ
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_NextStage_Init:
	{
		//todo : ステージのレベルを参照してenemyをセットする
		//Stage::Instance()->GetStageLevel();

		PlayerManager::Instance().GetFirstPlayer()->SetPositionWorld(Common::PlayerPosInit);
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_NextStage:
	{
		NextPhase();//次のフェーズへ
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_Start_Init:
	{
		//カードを最大値まで引く
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();

		PlayerManager::Instance().GetFirstPlayer()->ResetStatus();
		EnemyManager::Instance().ResetTurnEnemies();

		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//条件を見たし続けたら次のフェーズへ
		if (IsSlowNextPhase(!CardManager::Instance().IsMoving()))
		{
			CardManager::Instance().SetIsMoveable(true);
			NextPhase();//次のフェーズへ
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_Player_Init:
	{
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//todo : ここの条件にプレイヤーとエネミーの行動終了判定を追加
		if (IsQuickNextPhase(mouse.GetButtonDown()&Mouse::BTN_LEFT&&
			okButtonCollision.Hit(mouse.GetPosition()) && CardManager::Instance().IsFillSetCards()))
		{
			PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
			NextPhase();//次のフェーズへ
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		useCardIndex = 0u;
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		const bool isPlayseActFinished = PlayerManager::Instance().GetFirstPlayer()->GetState() == State::Act_Finish;

		UpdatePlayerAct(elapsedTime); 

		//todo : enemyが全員死んでいたらフェーズをphase_nextstage_init　に変更
		if (/*IsSlowNextPhase(elapsedTime, true)*/false)
		{
			ChangePhase(Phase::Phase_NextStage_Init);
		}


		//カード置き場のカードがなくなれば
		if (IsQuickNextPhase(CardManager::Instance().IsSetCardsEmpty()&&isPlayseActFinished))
		{
			NextPhase();//次のフェーズへ
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
		//一定時間待機して次のフェーズへ
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
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_EnemyAct:
	{
		//enemyが全員行動を完了していたら
		if (IsSlowNextPhase(EnemyManager::Instance().GetIsAllActEnd()))
		{
			NextPhase();
		}
	}
	break;
	//***********************************************************************************
	case PhaseManager::Phase::Phase_End_Init:
	{
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{
		//todo : 何らかの演出など
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

	//全てのフェーズで実行するもの
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

	//完了ボタン描画
	{
		const DirectX::XMFLOAT2& lefttop = okButtonCollision.GetLeftTop();
		const DirectX::XMFLOAT2& Size = okButtonCollision.GetBoxSize();
		okButton->Render(dc,
			lefttop.x, lefttop.y,
			Size.x, Size.y,
			.0f, .0f,
			okButton->GetTextureWidthf(), okButton->GetTextureHeightf(),
			.0f,
			1.0f, .0f, .0f, 1.0f);
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
		//フェーズの表示
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
	//playerの配置
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	player->SetPositionWorld(Common::PlayerPosInit);
	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	//enemyの配置
	EnemyMinion1* enemy = new EnemyMinion1(player);
	EnemyManager::Instance().Register(enemy);
	enemy->SetPositionWorld({ 1, 1 });
	enemy->SetTargetMovePosition({ -1, -1 });
	enemy->SetState(State::Idle_Init);

	//盤面のリセット
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
