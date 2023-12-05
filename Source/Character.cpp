#define NOMINMAX
#include "../ilib/BetterFunction.h"
#include "Character.h"
#include "CustomMathf.h"
#include "Stage.h"
#undef NOMINMAX

Character::Character()
{
	hpBar[0] = std::make_unique<Sprite>("./Data/Sprite/life_waku.png");
	hpBar[1] = std::make_unique<Sprite>("./Data/Sprite/life_1.png");
	hpBar[2] = std::make_unique<Sprite>("./Data/Sprite/life_2.png");
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
		}
	}
	return true;
}

void Character::Heal(const int hp)
{
	this->health += hp; 
	this->health = std::min(health, maxHealth);
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

	damage -= block;
	if (damage <= 0)
		return false;

	if (shield > 0)
	{
		int tempDamage = (shield >= damage) ? damage : shield;
		shield -= tempDamage;
		damage -= tempDamage;
		if (damage <= 0)
			return false;
	}

	//ダメージ処理
	health -= damage;

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

void Character::Render2D(ID3D11DeviceContext* dc, const HitBox2D& box)
{
	using namespace DirectX::ope;
	const DirectX::XMFLOAT2& leftTop = box.GetLeftTop();
	const DirectX::XMFLOAT2& BoxSize = box.GetBoxSize();
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
	const float HpParsent[] = { 1.0f,health / (float)maxHealth,shield / (float)maxHealth };


	const float min = std::min(BoxSize.x, BoxSize.y);//描画の辺の小さいほうをとる
	const DirectX::XMFLOAT2 iconSize = { min,min };//正方形のサイズを作る
	const DirectX::XMFLOAT2 HPBarSize = {BoxSize.x - iconSize.x,BoxSize.y * 0.4f};//HPバーのサイズを計算
	const DirectX::XMFLOAT2 scale = hpBar[1]->GetTextureSize()/hpBar[0]->GetTextureSize();//HPバーの背景とのサイズ差を計算
	const DirectX::XMFLOAT2 HpBarPos = { leftTop.x + iconSize.x,leftTop.y+BoxSize.y - HPBarSize.y };//HPバーの描画位置
	const DirectX::XMFLOAT2 zoomScale = HPBarSize / hpBar[0]->GetTextureSize();

	//キャラクターのアイコンを描画
	icon->Render(dc, box.GetLeftTop(), iconSize, .0f, DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });

	//HPバーの背景を描画
	hpBar[0]->Render(dc, HpBarPos, HPBarSize, .0f, { 1.0f,1.0f,1.0f,1.0f });
	for (size_t i = 1; i < std::size(hpBar); i++)
	{
		const DirectX::XMFLOAT2 sub = { hpBar[0]->GetTextureSize() - hpBar[1]->GetTextureSize() };
		const DirectX::XMFLOAT2 pos = { HpBarPos+((sub*0.5f)*zoomScale)};
		const DirectX::XMFLOAT2 size = { (HPBarSize.x*scale.x)*HpParsent[i],(HPBarSize.y*scale.y) };
		hpBar[i]->Render(dc, pos, size , .0f, DirectX::XMFLOAT4{1.0f,1.0f,1.0f,1.0f});
	}
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
