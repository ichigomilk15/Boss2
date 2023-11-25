#include "PhaseManager.h"

#include "Input/Input.h"
#include "EnemyManager.h"
#include "CardList.h"
#include "PlayerManager.h"
#include "Stage.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG
#include <EnemyBoss1.h>
#include <Stage.h>

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

		//todo : playerの位置を初期位置に戻す
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_NextStage:
	{
		NextPhase();//次のフェーズへ
	}
	//***********************************************************************************
	case PhaseManager::Phase::Phase_Start_Init:
	{
		//カードを最大値まで引く
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();

		//todo : enemyの次の行動の決定

		phaseTimer = NEXT_PHASE_WAIT_TIMER;
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//条件を見たし続けたら次のフェーズへ
		if (IsNextPhase(elapsedTime,!CardManager::Instance().IsMoving()))
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
		PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//todo : ここの条件にプレイヤーとエネミーの行動終了判定を追加
		if (mouse.GetButtonDown()&Mouse::BTN_LEFT&&
			okButtonCollision.Hit(mouse.GetPosition()) && CardManager::Instance().IsFillSetCards())
		{
			NextPhase();//次のフェーズへ
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//todo : ここの判定式をプレイヤーとエネミーが行動中じゃなければに変更
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT && okButtonCollision.Hit(mouse.GetPosition()))
		{
			CardManager::Instance().PopAndGetUseCard();
		}

		UpdatePlayerAct(elapsedTime); 

		//todo : enemyが全員死んでいたらフェーズをphase_nextstage_init　に変更
		if (/*IsNextPhase(elapsedTime, true)*/false)
		{
			phase = Phase::Phase_NextStage_Init;
		}


		//todo : ここにもプレイヤーとエネミーの行動終了判定を追加
		//カード置き場のカードがなくなれば
		if (IsNextPhase(elapsedTime,CardManager::Instance().IsSetCardsEmpty()))
		{
			NextPhase();//次のフェーズへ
		}

	}
		break;

		//***********************************************************************************
	case PhaseManager::Phase::Phase_Enemy_Init:
	{
		//todo : ここで行動順番を変えるならばソート?
		for (auto& enemy : EnemyManager::Instance().GetList())
		{
			
		}
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Enemy:
	{
		//todo : enemyの行動を呼び出す


		if (IsNextPhase(elapsedTime,true))
		{
			NextPhase();//次のフェーズへ
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_End_Init:
	{
		//todo : playerのシールドのリセット
		NextPhase();//次のフェーズへ
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{
		//todo : 何らかの演出など
		if (IsNextPhase(elapsedTime, true))
		{
			NextPhase();//次のフェーズへ
			break;
		}
	}
	break;
	default:
		break;
	}
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
	phase = Phase::Phase_GameStart_Init;
	trunCount = 0u;
	phaseTimer = .0f;
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
	phase = static_cast<Phase>(static_cast<int>(phase) + 1);
}

void PhaseManager::SetGameStart()
{
	//playerの配置
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	player->SetPositionWorld({ 3, 3 });
	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	//enemyのスポーン
	EnemyBoss1* enemy = new EnemyBoss1(player);
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

const bool PhaseManager::IsNextPhase(float elapsedTime, const bool flag)
{
	phaseTimer = flag ? phaseTimer - elapsedTime : NEXT_PHASE_WAIT_TIMER;
	if (phaseTimer < .0f)
	{
		phaseTimer = NEXT_PHASE_WAIT_TIMER;
		return true;
	}
	return false;
}
