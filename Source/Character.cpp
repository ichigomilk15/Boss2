#define NOMINMAX
#include "../ilib/BetterFunction.h"
#include "Graphics/NumberSprite.h"

#include "Character.h"
#include "CustomMathf.h"
#include "Stage.h"
#include "EnemyManager.h"
#include "DamageEffector.h"
#include "Camera.h"
#include "CameraController.h"
#undef NOMINMAX

Character::Character()
{
	hpBar[0] = std::make_unique<Sprite>("./Data/Sprite/life_waku.png");
	hpBar[1] = std::make_unique<Sprite>("./Data/Sprite/life_1.png");
	hpBar[2] = std::make_unique<Sprite>("./Data/Sprite/life_2.png");

	maskShaderDetails.maskTexture = std::make_unique<Texture>("Data/Textures/dissolve_animation.png");
	maskShaderDetails.dissolveThreshold = 1.0f;
	maskShaderDetails.edgeThreshold = 0.3f;
	maskShaderDetails.edgeColor = { 1.0f, 0.0f, 0.0f, 1.0f };
}

Character::~Character()
{
	/*if (attack)
	{
		delete attack;
		attack = nullptr;
	}*/
}

void Character::Update(float elapsedTime)
{
	hpBarData.timer = std::max(.0f, hpBarData.timer - elapsedTime);
}

void Character::UpdateTransform()
{
	SetAngle(CommonClass::directionMaps.find(direction)->second);
	float posWorldFinalX = positionWorld.x + pivotAdjustPosWorld.x;
	float posWorldFinalY = positionWorld.y + pivotAdjustPosWorld.y;
	float posWorldFinalZ = positionWorld.z + pivotAdjustPosWorld.z;

	//スケール行列
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	//位置行列を作る
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(posWorldFinalX, posWorldFinalY, posWorldFinalZ);

	//三つの行物を組み合わせ, ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行れつを取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::SetPositionWorld(const DirectX::XMINT2& position)
{
	this->position = position;
	this->positionWorld = Stage::Instance()->GetSquare(position.x, position.y)->GetWorldPos();
}

void Character::SetTargetMoveTranslation(const DirectX::XMINT2& moveDir)
{
	if (IsTargetMovePosValid({ position.x + moveDir.x, position.y + moveDir.y }))
	{
		targetMovePos.x = position.x + moveDir.x;
		targetMovePos.y = position.y + moveDir.y;
	}
}

const std::vector<DirectX::XMINT2> Character::GetSquaresPosition() const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int y = position.y; y < position.y + size.y; ++y)
	{
		for (int x = position.x; x < position.x + size.x; ++x)
		{
			squaresPos.emplace_back(DirectX::XMINT2{ x, y });
		}
	}
	return squaresPos;
}
const std::vector<DirectX::XMINT2> Character::GetSquaresPositionX(const int row) const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int x = position.x; x < position.x + size.x; ++x)
	{
		squaresPos.emplace_back(DirectX::XMINT2{ x, row });
	}
	return squaresPos;
}

const std::vector<DirectX::XMINT2> Character::GetSquaresPositionY(const int column) const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int y = position.y; y < position.y + size.y; ++y)
	{
		squaresPos.emplace_back(DirectX::XMINT2{ column, y });
	}
	return squaresPos;
}

void Character::SetDirection(int dir)
{
	dir %= CommonClass::directionMaps.size();
	direction = dir;
}

void Character::SetDirection(const DirectX::XMINT2 targetPos)
{
	for (int y = position.y; y < position.y + size.y; y++)
	{
		for (int x = position.x; x < position.x + size.x; x++)
		{
			if ((x == targetPos.x || y == targetPos.y) && !(x == targetPos.x && y == targetPos.y))
			{
				SetDirection(CommonClass::GetDirectionTarget({ x, y }, targetPos));
				return;
			}
		}
	}
	SetDirection(CommonClass::GetDirectionTarget(position, targetPos));
}

void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;


	{
		isGround = false;
		positionWorld.y += velocity.y * elapsedFrame;
		velocity.y += Common::gravity * elapsedTime;

		if (positionWorld.y <= pivotAdjustPosWorld.y)
		{
			positionWorld.y = pivotAdjustPosWorld.y;
			velocity.y = 0.0f;
			isGround = true;
		}
	}

	if (IsMoving())
	{
		const DirectX::XMFLOAT3 targetPosWorld = Stage::Instance()->GetWorldPos(targetMovePos);
		const DirectX::XMFLOAT3 formerPosWorld = Stage::Instance()->GetWorldPos(position);
		float rangeX = targetPosWorld.x - formerPosWorld.x;
		float rangeZ = targetPosWorld.z - formerPosWorld.z;
		this->positionWorld.x += rangeX * 1 / (60.0f * Common::moveSpeed) * elapsedFrame;
		this->positionWorld.z += rangeZ * 1 / (60.0f * Common::moveSpeed) * elapsedFrame;

		if (moveTimer >= Common::moveSpeed)
		{
			this->position = this->targetMovePos;
			this->positionWorld = Stage::Instance()->GetWorldPos(this->position);
			moveTimer = 0.0f;
		}
		moveTimer += elapsedTime;
	}
}

bool Character::GetIsDead()
{
	return (health <= 0);
}

bool Character::IsTargetMovePosValid(const DirectX::XMINT2& targetPos)
{
	for (int y = targetPos.y; y < targetPos.y + size.y; ++y)
	{
		for (int x = targetPos.x; x < targetPos.x + size.x; ++x)
		{
			if (!Stage::Instance()->IsInArea(x, y))
				return false;
			auto findChara = Stage::Instance()->GetSquare(x, y)->GetCharacter();
			if (findChara != nullptr && findChara != this)
			{
				return false;
			}

			for (auto& e : EnemyManager::Instance().GetList())
			{
				if (!e->IsMoving()) continue;

				for (int ey = e->GetTargetMovePosition().y; ey < e->GetTargetMovePosition().y + e->GetSize().y; ey++)
				{
					for (int ex = e->GetTargetMovePosition().x; ex < e->GetTargetMovePosition().x + e->GetSize().x; ex++)
					{
						if (ex == x && ey == y)
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

bool Character::IsTargetMoveAttackPosValid(const DirectX::XMINT2& targetPos)
{
	for (int y = targetPos.y; y < targetPos.y + size.y; ++y)
	{
		for (int x = targetPos.x; x < targetPos.x + size.x; ++x)
		{
			if (!Stage::Instance()->IsInArea(x, y))
				return false;
		}
	}
	return true;
}

bool Character::IsInTheSameRow(const Character& target)
{
	bool isSameRow = false;
	for (int y = GetPosition().y; y < GetPosition().y + GetSize().y; y++)
	{
		for (int y2 = target.GetPosition().y; y2 < target.GetPosition().y + target.GetSize().y; y2++)
		{
			if (y == y2)
			{
				isSameRow = true;
				break;
			}
		}
	}
	return isSameRow;
}
bool Character::IsInTheSameColumn(const Character& target)
{
	bool isSameColumn = false;
	for (int x = GetPosition().x; x < GetPosition().x + GetSize().x; x++)
	{
		for (int x2 = target.GetPosition().x; x2 < target.GetPosition().x + target.GetSize().x; x2++)
		{
			if (x == x2)
			{
				isSameColumn = true;
			}
		}
	}
	return isSameColumn;
}

void Character::Heal(const int hp)
{
	this->health += hp;
	this->health = std::min(health, maxHealth);
	ShowDamageNumber(hp,false, { .1f,1.0f,.1f,1.0f }, CommonClass::Left);
}

int Character::GetWhichHorizontalSide(const DirectX::XMINT2& pos)
{
	int pivotX = position.x + ((size.x + 1) / 2 - 1);
	return pos.x - pivotX;
}

int Character::GetWhichVerticalSide(const DirectX::XMINT2& pos)
{
	int pivotY = position.y + ((size.y + 1) / 2 - 1);
	return pos.y - pivotY;
}

void Character::CancelChargeAttack()
{
	isAttackCharging = false;
	Stage::Instance()->ResetAllSquareDrawType();
	attack->Destroy();
	attack = nullptr;
}

void Character::ResetStatus()
{
	shield = 0;
	block = 0;
	isActEnd = false;
}

bool Character::ApplyDamage(int damage)
{
	//ダメージを０の場合は健康状態を変更する必要がない
	if (damage <= 0) return false;
	//死亡している場合は健康状態を変更しない
	if (health <= 0) return false;

	//スタン状態ならダメージ2倍
	if (state == State::Stunned)
		damage *= 2;

	if (damage >= 10 && !CameraController::Instance().IsCameraShaking())
		CameraController::Instance().ShakeCamera(0.25f, 1);

	damage -= block;
	/*if (damage <= 0)
		return false;*/

	if (shield > 0)
	{
		int tempDamage = (shield >= damage) ? damage : shield;
		shield -= tempDamage;
		damage -= tempDamage;

		tempDamage *= -1;
		ShowDamageNumber(tempDamage, true, { .0f,1.0f,1.0f,1.0f }, CommonClass::DirectionFace::Left);
	}

	damage = std::min(damage, health);
	//ダメージ処理
	hpBarData.oldHp = health;
	health -= damage;
	hpBarData.shake = std::clamp(damage / static_cast<float>(maxHealth), .1f, .3f) * 20.0f;
	hpBarData.timer = 1.0f;

	//ダメージを出力
	ShowDamageNumber(damage, false, {1.0f, 0.0f, 0.0f, 1.0f}, CommonClass::DirectionFace::Right);

	//死亡通知
	if (health <= 0)
	{
		OnDead();
	}
	//ダメージ通知
	else
	{
		OnDamaged();
	}

	//健康状態が変更した場合はtrueを返す
	return true;
}

bool Character::IsMoving() const
{
	return ((position.x != targetMovePos.x || position.y != targetMovePos.y) &&
		Stage::Instance()->IsInArea(targetMovePos.x, targetMovePos.y));
}

//bool Character::IsAttacking() const
//{
//	return false;
//}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//速力に力を与える
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

void Character::Render2D(ID3D11DeviceContext* dc, const HitBox2D& box)const
{
	using namespace DirectX::ope;
	std::uniform_real_distribution<float> random(-hpBarData.shake,hpBarData.shake);
	const DirectX::XMFLOAT2& pos = box.GetLeftTop();
	HitBox2D tempBox = hpBarData.timer > .0f?
		HitBox2D::CreateBoxFromTopLeft({ pos.x+random(CommonClass::random),pos.y+random(CommonClass::random) }, box.GetBoxSize()):
		box;
	const DirectX::XMFLOAT2& leftTop = tempBox.GetLeftTop();
	const DirectX::XMFLOAT2& BoxSize = tempBox.GetBoxSize();
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
	const float HpParsent[] = { 1.0f,health / (float)maxHealth,shield / (float)maxHealth };


	const float min = std::min(BoxSize.x, BoxSize.y);//描画の辺の小さいほうをとる
	const DirectX::XMFLOAT2 iconSize = { min,min };//正方形のサイズを作る
	const DirectX::XMFLOAT2 HPBarSize = { BoxSize.x - iconSize.x,BoxSize.y * 0.4f };//HPバーのサイズを計算
	const DirectX::XMFLOAT2 scale = hpBar[1]->GetTextureSize() / hpBar[0]->GetTextureSize();//HPバーの背景とのサイズ差を計算
	const DirectX::XMFLOAT2 HpBarPos = { leftTop.x + iconSize.x,leftTop.y + BoxSize.y - HPBarSize.y };//HPバーの描画位置
	const DirectX::XMFLOAT2 zoomScale = HPBarSize / hpBar[0]->GetTextureSize();

	//キャラクターのアイコンを描画
	icon->Render(dc,leftTop, iconSize, .0f, DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });

	const DirectX::XMFLOAT2 NumSize = { BoxSize.x * 0.3f,BoxSize.y * 0.6f };
	//体力の数値を描画
	{
		std::string str;
		str += std::to_string(health);
		str += "/";
		str += std::to_string(maxHealth);
		NumberSprite::Instance().NumberOut(str.c_str(), dc, DirectX::XMFLOAT2{ BoxSize.x - NumSize.x,.0f } + leftTop, NumSize, { 1.0f,1.0f,1.0f,1.0f });
	}
	//シールドの値を描画
	if (shield > 0)
	{
		std::string str = "+" + std::to_string(shield);
		NumberSprite::Instance().NumberOut(str.c_str(), dc, DirectX::XMFLOAT2{ BoxSize.x - NumSize.x * 2.0f,.0f } + leftTop, { NumSize.x * 0.5f,NumSize.y }, { .0f,1.0f,1.0f,1.0f });
	}

	//HPバーの背景を描画
	hpBar[0]->Render(dc, HpBarPos, HPBarSize, .0f, { 1.0f,1.0f,1.0f,1.0f });
	for (size_t i = 1; i < std::size(hpBar); i++)
	{
		if (HpParsent[i] <= .0f)continue;
		const DirectX::XMFLOAT2 sub = { hpBar[0]->GetTextureSize() - hpBar[1]->GetTextureSize() };
		const DirectX::XMFLOAT2 pos = { HpBarPos + ((sub * 0.5f) * zoomScale) };
		const DirectX::XMFLOAT2 size = { (HPBarSize.x * scale.x) * HpParsent[i],(HPBarSize.y * scale.y) };
		hpBar[i]->Render(dc, pos, size, .0f, DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });
	}
}


void Character::ShowDamageNumber(const int damageNumber, bool isDrawSymbol, const DirectX::XMFLOAT4& color, int direction)
{
	DamageEffector::EffectData data;
	data.color = color;
	data.damage = damageNumber;
	data.scale = 2.5f;
	data.timer = 1.0f;
	data.isDrawSymbol = isDrawSymbol;

	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	auto dc = Graphics::Instance().GetDeviceContext();
	dc->RSGetViewports(&numViewports, &viewport);

	auto& camera = Camera::Instance();

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR CharaPos = DirectX::XMVectorSet(
		positionWorld.x, positionWorld.y + 2.0f, positionWorld.z, 0);
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		CharaPos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	DirectX::XMFLOAT3 screenPos;
	DirectX::XMStoreFloat3(&screenPos, ScreenPosition);

	data.pos = { screenPos.x, screenPos.y };

	//方向
	const int offset = 40;
	switch (direction)
	{
	case CommonClass::FrontLeft:
	case CommonClass::Left:
		data.velocity = { -30.0f,-80.0f };
		data.pos.x -= offset;
		break;

	case CommonClass::FrontRight:
	case CommonClass::Right:
		data.velocity = { 30.0f,-80.0f };
		data.pos.x += offset;
		break;

	case CommonClass::Back:
		data.velocity = { 0.0f, 80.0f };
		data.pos.y += offset;
		break;
	case CommonClass::Front:
	default:
		data.velocity = { 0.0f, -80.0f };
		data.pos.y -= offset;
		break;
	}


	DamageEffector::Instance().Register(data);
}

bool Character::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;

	HitResult findClosest;
	if (Collision::IntersectRayVsModel(start, end, this->model.get(), hit))
	{
		if (findClosest.distance < hit.distance)
		{
			hit = findClosest;
			result = true;
		}
	}

	return result;
}

void Character::Move(int vx, int vy)
{
	int x = position.x + vx;
	int y = position.y + vy;
	if (Stage::Instance()->IsInArea(x, y))
	{
		//移動方向ベクトルを設定
		position.x = x;
		position.y = y;
	}
}
