#include <imgui.h>
#include "SceneGame.h"
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Stage.h"
#include "NormalAttack.h"
#include "AttackManager.h"
#include "PhaseManager.h"

Player::Player()
{
	model = new Model("Data/Model/Player/Player.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.1f;

	hitEffect = new Effect("Data/Effect/Hit.efk");

	attackPower = 10;
	health = 75;
	maxHealth = 75;
	attackAdjacentRange = 3;
	SetDirection(CommonClass::DirectionFace::BackRight);
}

Player::~Player()
{
	delete hitEffect;

	delete model;
}

void Player::Update(float elapsedTime)
{
	//ステート更新処理
	UpdateState(elapsedTime);

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		{
			//位置
			ImGui::InputInt2("Position Square", &position.x);
			ImGui::InputInt2("Target Pos Square", &targetMovePos.x);
			ImGui::InputFloat3("Position World", &positionWorld.x);

			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			//Status
			ImGui::InputInt("Shield", &shield, 0);
			ImGui::InputInt("HP", &health, 0);
		}
	}
	ImGui::End();
}

void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}

void Player::UpdateState(float elapsedTime)
{
	switch (state)
	{
	case State::Idle_Init:
		this->model->PlayAnimation(Animation::Idle, true);
		state = State::Idle;
		[[fallthrough]];
	case State::Idle:

		break;

	case State::Act_Init:
		if (!model->IsPlayAnimation(Animation::Idle))
			this->model->PlayAnimation(Animation::Idle, true);
		actTimer = 0.5f;
		SetState(State::Act);
		[[fallthrough]];
	case State::Act:
		if (actTimer > 0.0f)
		{
			actTimer -= elapsedTime;
		}
		else
		{
			SetState(ChooseAct(elapsedTime));
			break;
		}
		break;

	case State::Move_Init:
		Stage::Instance()->ResetAllSquare();
		this->targetMovePos = { -1, -1 };
		state = State::Move;
		[[fallthrough]];
	case State::Move:
		UpdateMove(elapsedTime);

		if (IsMoving())
		{
			SetState(State::Moving_Init);
			break;
		}
		break;
	case State::Moving_Init:
		this->model->PlayAnimation(Animation::Run, true);
		state = State::Moving;
		this->SetDirection(this->targetMovePos);
		[[fallthrough]];
	case State::Moving:
		if (!IsMoving())
		{
			SetState(State::Act_Init);
			Stage::Instance()->ResetAllSquare();
			break;
		}
		break;

	case State::Attack_Init:
		Stage::Instance()->ResetAllSquare();
		state = State::Attack;
		[[fallthrough]];
	case State::Attack:
		UpdateAttack(elapsedTime);
		if (attack && !attack->GetIsDestroy())
		{
			SetState(State::Attacking_Init);
			break;
		}
		break;

	case State::Attacking_Init:
		this->model->PlayAnimation(Animation::Attack, false);
		state = State::Attacking;
		[[fallthrough]];
	case State::Attacking:
		if (!attack || (attack && attack->GetIsDestroy()) || !model->IsPlayAnimation())
		{
			attack = nullptr;
			SetState(State::Act_Init);
		}
		break;

	case State::Defence_Init:
		this->model->PlayAnimation(Animation::Damage, false);
		actTimer = 0.5f;
		Stage::Instance()->ResetAllSquare();
		shield += 4;
		state = State::Defence;
		[[fallthrough]];
	case State::Defence:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			SetState(State::Act_Init);
			break;
		}
		break;

	case State::Damage_Init:
		model->PlayAnimation(Animation::Damage, false, 0.1f);
		actTimer = 0.5f;
		state = State::Defence;
		[[fallthrough]];
	case State::Damage:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f &&  !model->IsPlayAnimation())
		{
			SetState(State::Act_Init);
			break;
		}

	case State::Act_Finish_Init:
		actTimer = 0.5f;
		state = State::Act_Finish;
		//CardManager::Instance().PopAndGetUseCard();
		[[fallthrough]];
	case State::Act_Finish:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			SetState(State::Idle_Init);
			break;
		}
		break;
	}
}

void Player::UpdateMove(float elapsedTime)
{
	Stage::Instance()->SetSquareTypeMove(position, moveRange, { Square::Type::Inaccessible });

	Mouse& mouse = Input::Instance().GetMouse();
	auto dc = Graphics::Instance().GetDeviceContext();
	Camera& camera = Camera::Instance();

	DirectX::XMFLOAT3 startMousePos = CommonClass::GetWorldStartPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());
	DirectX::XMFLOAT3 endMousePos = CommonClass::GetWorldEndPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());

	HitResult hit;
	std::shared_ptr<Square> foundSq = nullptr;
	for (auto& sq : Stage::Instance()->GetSquareTypeMove())
	{
		if (sq->Raycast(startMousePos, endMousePos, hit))
		{
			sq->SetType(Square::Type::MoveAreaChosen);
			foundSq = sq;
			this->SetDirection(sq->GetPos());
		}
	}
	if (foundSq && mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		this->targetMovePos = foundSq->GetPos();
	}
}

void Player::UpdateAttack(float elapsedTime)
{
	std::vector<Square*> squares = Stage::Instance()->GetSquaresEdgeAdjacent(this->position.x, this->position.y, attackAdjacentRange);

	for (auto& square : squares)
	{
		square->SetType(Square::Type::AttackArea);
	}

	Mouse& mouse = Input::Instance().GetMouse();
	auto dc = Graphics::Instance().GetDeviceContext();
	Camera& camera = Camera::Instance();

	DirectX::XMFLOAT3 startMousePos = CommonClass::GetWorldStartPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());
	DirectX::XMFLOAT3 endMousePos = CommonClass::GetWorldEndPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());

	HitResult hit;
	std::vector<Square*> attackSq;

	for (auto& sq : squares)
	{
		if (sq->Raycast(startMousePos, endMousePos, hit))
		{
			SetDirection(sq->GetPos());
			attackSq = Stage::Instance()->GetSquaresByDirection(this->position.x, this->position.y, 3, this->GetDirection());
			for (auto& attSq : attackSq)
			{
				attSq->SetType(Square::Type::AttackAreaChosen);
			}
		}
	}

	if (!attackSq.empty() && mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		Stage::Instance()->ResetAllSquare();
		std::vector<DirectX::XMINT2> posVec;
		for (auto& sq : attackSq)
		{
			posVec.emplace_back(sq->GetPos());
		}
		attack = new NormalAttack(this, attackPower, TargetAttackEnum::Target_Enemy, posVec, 0.5f);
		AttackManager::Instance().Register(attack);
	}
}

State Player::ChooseAct(float elapsedTime)
{
	Card::Type cardType = CardManager::Instance().PopAndGetUseCard();
	switch (cardType)
	{
	case Card::Type::MOVE:
		return State::Move_Init;
		break;
	case Card::Type::ATTACK:
		return State::Attack_Init;
		break;
	case Card::Type::DEFENCE:
		return State::Defence_Init;
		break;
	case Card::Type::NONE:
		return State::Act_Finish_Init;
		break;
	default:
		return State::Act_Init;
		break;
	}
}

void Player::OnDamaged()
{
	state = State::Damage_Init;
}
