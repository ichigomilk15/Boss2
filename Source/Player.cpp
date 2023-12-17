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
#include "Audio\AudioLoader.h"

Player::Player() :Character()
{
	this->model = std::make_unique<Model>("Data/Model/Player/Player.mdl");
	icon = std::make_unique<Sprite>("./Data/Sprite/icon_player.png");

	//マスクシェーダー色
	maskShaderDetails.edgeColor = { 0.0f, 0.0f, 1.0f, 1.0f };

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.1f;

	//effectたちの設定
	buffEffect = std::make_unique<Effect>("./Data/Effect/powerup.efk");
	debuffEffect = std::make_unique<Effect>("./Data/Effect/powerdown.efk");
	shieldEffect = std::make_unique<Effect>("./Data/Effect/sheild.efk");
	effects.attack = std::make_unique<Effect>("./Data/Effect/attack.efk");
	effects.damage = std::make_unique<Effect>("./Data/Effect/damage.efk");

	attackPower = 10;
	maxHealth = 75;
	health = 75;
	maxHealth = 75;
	//attackAdjacentRange = 3;
	SetDirection(CommonClass::DirectionFace::BackRight);
	InitializeAudio();
}

Player::~Player()
{
	delete hitEffect;

	this->playerSes.attackSe->Stop();
	this->playerSes.damageSe->Stop();
	this->playerSes.deadSe->Stop();
	this->playerSes.shieldSe->Stop();
	this->playerSes.moveSe->Stop();
}

void Player::Update(float elapsedTime)
{
	//死亡処理
	UpdateDeath(elapsedTime);

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

void Player::Render(ID3D11DeviceContext* dc, Shader* shader, RenderContext& rc)
{
	rc.maskData.maskTexture = maskShaderDetails.maskTexture->GetShaderResourceView().Get();
	rc.maskData.dissolveThreshold = maskShaderDetails.dissolveThreshold;
	rc.maskData.edgeColor = maskShaderDetails.edgeColor;
	rc.maskData.edgeThreshold = maskShaderDetails.edgeThreshold;
	shader->Draw(dc, model.get(), rc);
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

			//Mask Shader
			ImGui::DragFloat("Dissolve Value", &maskShaderDetails.dissolveThreshold, 0.01f, 0.0f, 1.0f);
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
		UpdateViewEnemyDetail();
		break;

	case State::Act_Init:
		if (!model->IsPlayAnimation(Animation::Idle))
			this->model->PlayAnimation(Animation::Idle, true);
		actTimer = 0.5f;
		Stage::Instance()->ResetAllSquare();
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
		playerSes.moveSe.get()->Play(true);
		this->SetDirection(this->targetMovePos);
		[[fallthrough]];
	case State::Moving:
	{
		if (!IsMoving())
		{
			playerSes.moveSe.get()->Stop();
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
		playerSes.attackSe.get()->Play(false);
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
		shieldEffect->Play(positionWorld, 2.f);

		playerSes.shieldSe.get()->Stop();
		playerSes.shieldSe.get()->Play(false);
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
		actTimer = 1.0f;
		Stage::Instance()->ResetAllSquare();
		state = State::Special;
		cardComboDataBase = nullptr;
		buffEffect->Play(positionWorld, 1.0f);//effect再生
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
		debuffEffect->Play(positionWorld, 1.0f);
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
			Stage::Instance()->ResetAllSquare();
			SetState(State::Idle_Init);
			break;
		}
		break;

	case State::Death_Init:
		model->PlayAnimation(Animation::Death, false);
		state = State::Death;
		[[fallthrough]];
	case State::Death:

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
	if(Card* getCard = square->GetCard())
	{
		if ((getCard->GetType() == Card::Type::SPECIAL && CardManager::Instance().GetHaveSpecial() < CardManager::SPECIAL_CARD_MAX) 
			|| getCard->GetType() == Card::Type::DEBUFF)
		{
			GetCard(getCard);
			square->ResetCard();
		}
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
	Square* hitSquare = nullptr;
	std::vector<Square*> attackSq;

	for (auto& sq : squares)
	{
		if (sq->Raycast(startMousePos, endMousePos, hit))
		{
			hitSquare = sq;
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
		if(hitSquare)
			effects.attack->Play(hitSquare->GetWorldPos(),static_cast<float>(attackDetail->AreaAttackCost+1));

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
	if(defenceDetail->getShield)
		ShowDamageNumber(shieldGet, { 0.0f, 1.0f, 1.0f, 1.0f });
	if (defenceDetail->GetBlock)
		ShowDamageNumber(defenceDetail->GetBlock, { .5f,.5f,1.0f,1.0f });
	//if (defenceDetail->heal)
	//	ShowDamageNumber(defenceDetail->heal, { .5f,1.0f,0.5f,1.0f });
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
	//effects.damage->Play(positionWorld, 1.0f);
	state = State::Damage_Init;
	playerSes.damageSe.get()->Play(false);
}

void Player::OnDead()
{
	playerSes.deadSe.get()->Play(false);
	state = State::Death_Init;
	destroyedStatus.destroyedTime = 3.0f;
	//playerDeadTime = 4.0f;
}

void Player::GetCard(Card* getCard)
{
	switch (getCard->GetType())
	{
	case Card::Type::SPECIAL:
		CardManager::Instance().AddCardFront(std::make_shared<Card>(CardManager::Instance().GetCardSpawnPos(), CardManager::CARD_SIZE, getCard->GetType()));
		playerSes.buff.get()->Play(false);
		break;
	case Card::Type::DEBUFF:
		CardManager::Instance().AddCardReserved(std::make_shared<Card>(CardManager::Instance().GetCardSpawnPos(), CardManager::CARD_SIZE, Card::Type::DEBUFF));
		break;
	}
}

void Player::UpdateViewEnemyDetail()
{
	Mouse& mouse = Input::Instance().GetMouse();
	Camera& camera = Camera::Instance();
	auto dc = Graphics::Instance().GetDeviceContext();
	if (lookAtEnemyDetail.isLookAtEnemyDetail)
	{
		if (!lookAtEnemyDetail.target || (mouse.GetButtonDown() & mouse.BTN_LEFT))
		{
			lookAtEnemyDetail.isLookAtEnemyDetail = false;
			Stage::Instance()->ResetAllSquareDrawType(Square::DrawType::NormalAttackView);
			return;
		}
	}
	else
	{
		DirectX::XMFLOAT3 startMousePos = CommonClass::GetWorldStartPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());
		DirectX::XMFLOAT3 endMousePos = CommonClass::GetWorldEndPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());

		HitResult hit;
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
		{
			for (auto& e : EnemyManager::Instance().GetList())
			{
				if (e->RayCast(startMousePos, endMousePos, hit))
				{
					lookAtEnemyDetail.isLookAtEnemyDetail = true;
					lookAtEnemyDetail.target = e;

					for (auto& sq : e->GetSquaresPosition())
					{
						auto atkPos = Stage::Instance()->GetSquaresEdgeAdjacent(sq.x, sq.y, e->GetAttackRange());
						for (auto& sq : atkPos)
						{
							auto sqPos = sq->GetPos();
							Stage::Instance()->GetSquare(sqPos.x, sqPos.y)->InputDrawType(Square::DrawType::NormalAttackView);
						}
					}
					return;
				}
			}

			for (int y = 0; y < Common::SQUARE_NUM_Y; y++)
			{
				for (int x = 0; x < Common::SQUARE_NUM_X; x++)
				{
					auto sq = Stage::Instance()->GetSquare(x, y);
					auto findChara = sq->GetCharacter();
					if (!findChara) continue;

					if (sq->Raycast(startMousePos, endMousePos, hit))
					{
						auto e = dynamic_cast<Enemy*>(const_cast<Character*>(findChara));
						if (e)
						{
							lookAtEnemyDetail.isLookAtEnemyDetail = true;
							lookAtEnemyDetail.target = e;
							/*auto atkPos = Stage::Instance()->GetSquaresEdgeAdjacent(e->GetPosition().x, e->GetPosition().y, e->GetAttackRange());*/
							for (auto& ePos : e->GetSquaresPosition())
							{
								auto atkPos = Stage::Instance()->GetSquaresEdgeAdjacent(ePos.x, ePos.y, e->GetAttackRange());

								for (auto& sq : atkPos)
								{
									auto sqPos = sq->GetPos();
									Stage::Instance()->GetSquare(sqPos.x, sqPos.y)->InputDrawType(Square::DrawType::NormalAttackView);
								}
							}
							return;
						}
					}
				}
			}
		}
	}
}

void Player::InitializeAudio()
{
	AudioLoader::Load(AUDIO::SE_PLAYER_ATTACK, playerSes.attackSe);
	AudioLoader::Load(AUDIO::SE_PLAYER_DAMAGE, playerSes.damageSe);
	AudioLoader::Load(AUDIO::SE_PLAYER_DEAD, playerSes.deadSe);
	AudioLoader::Load(AUDIO::SE_PLAYER_SHIELD, playerSes.shieldSe);
	AudioLoader::Load(AUDIO::SE_PLAYER_MOVE, playerSes.moveSe);
	AudioLoader::Load(AUDIO::SE_CARD_GETBUFF, playerSes.buff);

	AudioLoader::Load(AUDIO::SE_CARD_DECIDE, cardSes.cardDecideSe);
	AudioLoader::Load(AUDIO::SE_CARD_DRAW, cardSes.cardDraw);
	AudioLoader::Load(AUDIO::SE_CARD_SET, cardSes.cardSet);
}

void Player::UpdateDeath(float elapsedTime)
{
	if (!GetIsDead())
		return;

	destroyedStatus.destroyedTime -= elapsedTime;
	 maskShaderDetails.dissolveThreshold = max(0.0f, maskShaderDetails.dissolveThreshold - (elapsedTime / 2.0f));

	if (destroyedStatus.destroyedTime <= 0.0f)
	{
		destroyedStatus.isDestroyed = true;
	}
}
