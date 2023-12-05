#include <imgui.h>

#include "Input/Input.h"
#include "Graphics/NumberSprite.h"

#include "SceneGame.h"
#include "Player.h"
#include "Camera.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Stage.h"
#include "NormalAttack.h"
#include "KnockbackAttack.h"
#include "AttackManager.h"
#include "PhaseManager.h"
#include "EnemyManager.h"

Player::Player():Character()
{
	model = new Model("Data/Model/Player/Player.mdl");
	icon = std::make_unique<Sprite>("./Data/Sprite/icon_player.png");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.1f;

	hitEffect = new Effect("Data/Effect/Hit.efk");

	attackPower = 10;
	maxHealth = 75;
	health = 75;
	maxHealth = 75;
	//attackAdjacentRange = 3;
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

//void Player::Render2D(RenderContext& rc, ID3D11DeviceContext* dc)
//{
//	Graphics& graphics = Graphics::Instance();
//	D3D11_VIEWPORT viewPort;
//	unsigned int viewNum = 1;
//	dc->RSGetViewports(&viewNum, &viewPort);
//
//	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
//	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
//
//	DirectX::XMVECTOR WorldPos = DirectX::XMVectorSet(positionWorld.x, positionWorld.y + height, positionWorld.z, .0f);
//	DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(WorldPos, viewPort.TopLeftX, viewPort.TopLeftY,
//		viewPort.Width, viewPort.Height, viewPort.MinDepth, viewPort.MaxDepth, Projection, View, DirectX::XMMatrixIdentity());
//
//	DirectX::XMFLOAT2 screenPos;
//	DirectX::XMStoreFloat2(&screenPos, ScreenPos);
//	DirectX::XMFLOAT2 size = { 60.0f,30.0f };
//	std::string str;
//	str = std::to_string(health);
//	//str += "/";
//	//str += std::to_string(maxHealth);
//	HitBox2D box = HitBox2D::CreateBoxFromCenter(screenPos, size);
//	NumberSprite::Instance().NumberOut(str.c_str(), dc, box.GetLeftTop(), box.GetBoxSize(), DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });
//}

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
			ImGui::InputInt("Block", &block, 0);
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
	//#if _DEBUG
	//	GamePad& gamePad = Input::Instance().GetGamePad();
	//	if (gamePad.GetButtonDown() & gamePad.BTN_UP)
	//	{
	//		targetMovePos.y = position.y - 1;
	//		SetState(State::Moving_Init);
	//	}
	//	if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
	//	{
	//		targetMovePos.y = position.y + 1;
	//		SetState(State::Moving_Init);
	//	}
	//	if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
	//	{
	//		targetMovePos.y = position.x - 1;
	//		SetState(State::Moving_Init);
	//	}
	//	if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
	//	{
	//		targetMovePos.y = position.x + 1;
	//		SetState(State::Moving_Init);
	//	}
	//#endif
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
	{
		if (!IsMoving())
		{
			Stage::Instance()->ResetAllSquare();
			SetState(MovingEnd());
			if (cardComboDataBase)
				cardComboDataBase = nullptr;
			break;
		}
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
		SetShieldAction();
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

	case State::Special_Init:
		actTimer = 0.5f;
		Stage::Instance()->ResetAllSquare();
		state = State::Special;
		cardComboDataBase = nullptr;
		[[fallthrough]];
	case State::Special:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			SetState(State::Act_Init);
			break;
		}
		break;

	case State::Debuff_Init:
		actTimer = 0.5f;
		Stage::Instance()->ResetAllSquare();
		state = State::Debuff;
		SetDebuffAction();
		[[fallthrough]];
	case State::Debuff:
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
		if (actTimer < 0.0f && !model->IsPlayAnimation())
		{
			SetState(State::Act_Init);
			break;
		}
		break;

	case State::KnockedBack_Init:
		if (targetMovePos.x > 0 || targetMovePos.y > 0)
			this->SetDirection({ position.x - targetMovePos.x, position.y - targetMovePos.y });
		actTimer = 0.5f;
		state = State::KnockedBack;
		[[fallthrough]];
	case State::KnockedBack:
		actTimer -= elapsedTime;
		if (!IsMoving() && actTimer < 0.0f)
		{
			SetState(State::Idle_Init);
			break;
		}
		break;

	case State::Act_Finish_Init:
		actTimer = 0.5f;
		state = State::Act_Finish;
		isActEnd = true;
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
	CardComboMove* moveDetail = dynamic_cast<CardComboMove*>(cardComboDataBase);
	if (!moveDetail)
		return;
	if (moveDetail->attackDamege > 0)
	{
		Stage::Instance()->SetSquareTypeMove(position, moveDetail->moveCost);
	}
	else
	{
		Stage::Instance()->SetSquareTypeMove(position, moveDetail->moveCost, { Square::Type::Inaccessible });
	}

	Mouse& mouse = Input::Instance().GetMouse();
	auto dc = Graphics::Instance().GetDeviceContext();
	Camera& camera = Camera::Instance();

	DirectX::XMFLOAT3 startMousePos = CommonClass::GetWorldStartPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());
	DirectX::XMFLOAT3 endMousePos = CommonClass::GetWorldEndPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());

	HitResult hit;
	std::shared_ptr<Square> foundSq = nullptr;
	for (auto& sq : Stage::Instance()->GetSquareType(Square::Type::MoveArea))
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
		//攻撃ｘ移動
		if (Stage::Instance()->GetSquare(foundSq->GetPos().x, foundSq->GetPos().y)->GetCharacter())
		{
			state = State::Attacking_Init;
			InitializeAttack(moveDetail->attackDamege, { foundSq->GetPos() }, 0.5f);
			return;
		}
		this->targetMovePos = foundSq->GetPos();
	}
}

State Player::MovingEnd()
{
	std::shared_ptr<Square> square = Stage::Instance()->GetSquare(position.x, position.y);
	Card* getCard = square->GetCard();
	if (getCard)
	{
		GetCard(getCard);
		square->ResetCard();
	}

	CardComboMove* moveDetail = dynamic_cast<CardComboMove*>(std::move(cardComboDataBase));
	if (!moveDetail)
		return State::Act_Init;
	if (moveDetail->knockbackCost <= 0)
		return State::Act_Init;

	//防御ｘ移動 処理
	bool IsFoundEnemy = false;
	for (auto& e : EnemyManager::Instance().GetList())
	{
		const int dir = Stage::Instance()->IsAdjacent(this, e);
		if (dir >= 0)
		{
			InitializeKnockbackAttack(moveDetail->knockbackTakeDamege, moveDetail->knockbackCost, dir, { e->GetPosition() }, 0.5f);
			SetDirection(dir);
			shield -= moveDetail->knockbackTakeDamege;
			return State::Attacking_Init;
		}
	}
	return State::Act_Init;
}

void Player::UpdateAttack(float elapsedTime)
{
	Stage::Instance()->ResetAllSquare();
	CardComboAttack* attackDetail = dynamic_cast<CardComboAttack*>(std::move(cardComboDataBase));
	//CardComboAttack* attackDetail = nullptr;
	if (!attackDetail)
		return;

	std::vector<Square*> squares = Stage::Instance()->GetSquaresEdgeAdjacent(this->position.x, this->position.y, attackDetail->Attackcost);

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
			int cost = (attackDetail->VAreaAttackCost) ? 2 : 1;
			attackSq = Stage::Instance()->GetSquaresByDirection(this->position.x, this->position.y, cost, this->GetDirection());
			for (auto& attSq : attackSq)
			{
				if (attackDetail->AreaAttackCost)
				{
					auto sqArea = Stage::Instance()->GetSquares(attSq->GetPos().x, attSq->GetPos().y, 1);
					for (auto& sq : sqArea)
					{
						sq->SetType(Square::Type::AttackAreaChosen);
					}
				}
				attSq->SetType(Square::Type::AttackAreaChosen);
			}
		}
	}

	if (!attackSq.empty() && mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//Stage::Instance()->ResetAllSquare();

		int attackDamage = attackDetail->AttackDamage;
		if (attackDetail->UseShield)
		{
			attackDamage += shield;
			shield = 0;
		}

		std::vector<DirectX::XMINT2> posVec;
		auto testSq = Stage::Instance()->GetSquareType(Square::Type::AttackAreaChosen);
		for (auto& sq : testSq)
		{
			posVec.emplace_back(sq->GetPos());
		}
		InitializeAttack(attackDamage, posVec, 0.5f);
	}
}

void Player::InitializeAttack(const int damage, const std::vector<DirectX::XMINT2>& posAttack, const float timer)
{
	attack = new NormalAttack(this, damage, TargetAttackEnum::Target_Enemy, posAttack, 0.5f);
	AttackManager::Instance().Register(attack);
}

void Player::InitializeKnockbackAttack(const int damage, const int knockbackCost, const int knockbackDir,
	const std::vector<DirectX::XMINT2>& posAttack, const float timer)
{
	attack = new KnockbackAttack(this, damage, knockbackCost, knockbackDir, TargetAttackEnum::Target_Enemy, posAttack, 0.5f);
	AttackManager::Instance().Register(attack);
}

void Player::SetShieldAction()
{
	CardComboDefence* defenceDetail = dynamic_cast<CardComboDefence*>(std::move(cardComboDataBase));
	if (!defenceDetail)
		return;

	if (defenceDetail->heal > 0)
		Heal(defenceDetail->heal);
	if (defenceDetail->GetBlock > 0)
		block = defenceDetail->GetBlock;

	int shieldGet = 0;
	if (defenceDetail->getShield > 0)
		shieldGet = defenceDetail->getShield;
	if (defenceDetail->movecostGetShield)
	{
		int moveRange = Stage::Instance()->GetTargetPosCost(position, turnPosInit);
		shieldGet += moveRange;
	}
	shield += shieldGet;
}

void Player::SetDebuffAction()
{
	CardComboDebuff* debuffDetail = dynamic_cast<CardComboDebuff*>(std::move(cardComboDataBase));
	if (!debuffDetail)
		return;

	if (debuffDetail->takeDamage > 0)
	{
		this->ApplyDamage(debuffDetail->takeDamage);
	}
	if (debuffDetail->attackDamage > 0)
	{
		for (auto& enemy : EnemyManager::Instance().GetList())
		{
			enemy->ApplyDamage(debuffDetail->attackDamage);
		}
	}

	if (debuffDetail->heal > 0)
	{
		Heal(debuffDetail->heal);
	}
}

State Player::ChooseAct(float elapsedTime)
{
	cardComboDataBase = nullptr;
	auto data = CardManager::Instance().PopAndGetUseCard();
	switch (data->type)
	{
	case Card::Type::MOVE:
		this->cardComboDataBase = dynamic_cast<CardComboMove*>(std::move(data));
		if (cardComboDataBase)
		{
			return State::Move_Init;
			break;
		}
		break;
	case Card::Type::ATTACK:
		this->cardComboDataBase = dynamic_cast<CardComboAttack*>(std::move(data));
		if (cardComboDataBase)
		{
			return State::Attack_Init;
			break;
		}
		break;
	case Card::Type::DEFENCE:
		this->cardComboDataBase = dynamic_cast<CardComboDefence*>(std::move(data));
		if (cardComboDataBase)
		{
			return State::Defence_Init;
			break;
		}
		break;
	case Card::Type::SPECIAL:
		this->cardComboDataBase = dynamic_cast<CardComboNoUseing*>(std::move(data));
		if (cardComboDataBase)
		{
			return State::Special_Init;
			break;
		}
		break;
	case Card::Type::DEBUFF:
		this->cardComboDataBase = dynamic_cast<CardComboDebuff*>(std::move(data));
		if (cardComboDataBase)
		{
			return State::Debuff_Init;
			break;
		}
		break;
	}
	return State::Act_Finish_Init;
}

void Player::OnDamaged()
{
	state = State::Damage_Init;
}

void Player::GetCard(Card* getCard)
{
	switch (getCard->GetType())
	{
	case Card::Type::SPECIAL:
		CardManager::Instance().AddCardFront(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CardManager::CARD_SIZE, getCard->GetType()));
		break;
	case Card::Type::DEBUFF:
		CardManager::Instance().AddCardReserved(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CardManager::CARD_SIZE, Card::Type::DEBUFF));
		break;
	}
}
