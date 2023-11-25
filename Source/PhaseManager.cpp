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
	okButton = std::make_unique<Sprite>();//todo : ok�{�^���̉摜�ǂݍ���

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
		//todo : �X�e�[�W�̃��x�����Q�Ƃ���enemy���Z�b�g����
		//Stage::Instance()->GetStageLevel();

		//todo : player�̈ʒu�������ʒu�ɖ߂�
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_NextStage:
	{
		NextPhase();//���̃t�F�[�Y��
	}
	//***********************************************************************************
	case PhaseManager::Phase::Phase_Start_Init:
	{
		//�J�[�h���ő�l�܂ň���
		CardManager& cardManager = CardManager::Instance();
		cardManager.SetIsMoveable(false);
		cardManager.Replenish();

		//todo : enemy�̎��̍s���̌���

		phaseTimer = NEXT_PHASE_WAIT_TIMER;
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Start:
	{
		//�������������������玟�̃t�F�[�Y��
		if (IsNextPhase(elapsedTime,!CardManager::Instance().IsMoving()))
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
		PlayerManager::Instance().GetFirstPlayer()->SetState(State::Act_Init);
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Player:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//todo : �����̏����Ƀv���C���[�ƃG�l�~�[�̍s���I�������ǉ�
		if (mouse.GetButtonDown()&Mouse::BTN_LEFT&&
			okButtonCollision.Hit(mouse.GetPosition()) && CardManager::Instance().IsFillSetCards())
		{
			NextPhase();//���̃t�F�[�Y��
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_PlayerAct_Init:
	{
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_PlayerAct:
	{
		Mouse& mouse = Input::Instance().GetMouse();
		//todo : �����̔��莮���v���C���[�ƃG�l�~�[���s��������Ȃ���΂ɕύX
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT && okButtonCollision.Hit(mouse.GetPosition()))
		{
			CardManager::Instance().PopAndGetUseCard();
		}

		UpdatePlayerAct(elapsedTime); 

		//todo : enemy���S������ł�����t�F�[�Y��phase_nextstage_init�@�ɕύX
		if (/*IsNextPhase(elapsedTime, true)*/false)
		{
			phase = Phase::Phase_NextStage_Init;
		}


		//todo : �����ɂ��v���C���[�ƃG�l�~�[�̍s���I�������ǉ�
		//�J�[�h�u����̃J�[�h���Ȃ��Ȃ��
		if (IsNextPhase(elapsedTime,CardManager::Instance().IsSetCardsEmpty()))
		{
			NextPhase();//���̃t�F�[�Y��
		}

	}
		break;

		//***********************************************************************************
	case PhaseManager::Phase::Phase_Enemy_Init:
	{
		//todo : �����ōs�����Ԃ�ς���Ȃ�΃\�[�g?
		for (auto& enemy : EnemyManager::Instance().GetList())
		{
			
		}
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_Enemy:
	{
		//todo : enemy�̍s�����Ăяo��


		if (IsNextPhase(elapsedTime,true))
		{
			NextPhase();//���̃t�F�[�Y��
			break;
		}
	}
	break;

	//***********************************************************************************
	case PhaseManager::Phase::Phase_End_Init:
	{
		//todo : player�̃V�[���h�̃��Z�b�g
		NextPhase();//���̃t�F�[�Y��
	}
	[[fallthrough]];
	case PhaseManager::Phase::Phase_End:
	{
		//todo : ���炩�̉��o�Ȃ�
		if (IsNextPhase(elapsedTime, true))
		{
			NextPhase();//���̃t�F�[�Y��
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

	//�����{�^���`��
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
	//player�̔z�u
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	player->SetPositionWorld({ 3, 3 });
	player->SetTargetMovePosition({ -1, -1 });
	player->SetState(State::Idle_Init);

	//enemy�̃X�|�[��
	EnemyBoss1* enemy = new EnemyBoss1(player);
	EnemyManager::Instance().Register(enemy);
	enemy->SetPositionWorld({ 1, 1 });
	enemy->SetTargetMovePosition({ -1, -1 });
	enemy->SetState(State::Idle_Init);

	//�Ֆʂ̃��Z�b�g
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
