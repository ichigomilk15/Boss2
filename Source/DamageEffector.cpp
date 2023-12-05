
#include <string>

#include "Graphics/Graphics.h"
#include "../External/ilib/BetterFunction.h"
#include "Graphics/NumberSprite.h"

#include "DamageEffector.h"

bool DamageEffector::EffectData::operator<(const EffectData& dst)
{
	return this->timer < dst.timer;
}

bool DamageEffector::EffectData::operator<(const float& value)
{
	return this->timer < value;
}

DamageEffector::~DamageEffector()
{
}

void DamageEffector::Update(float elapsedTime)
{
	using namespace DirectX::ope;
	for (auto& data : datas)
	{
		data.pos = data.pos + data.velocity*elapsedTime;
		data.timer -= elapsedTime;
	}

	std::erase_if(datas, [&](const EffectData& dst) {return dst.timer < .0f; });
}

void DamageEffector::Render(ID3D11DeviceContext* dc) const
{
	using namespace DirectX::ope;
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
	const float aspect = ScreenSize.y / ScreenSize.x;
	DirectX::XMFLOAT2 normalSize = { ScreenSize.x * 0.05f * aspect,ScreenSize.x * 0.05f };
	for (auto& data : datas)
	{
		NumberSprite::Instance().NumberOut(std::to_string(data.damage).c_str(), dc, data.pos, normalSize * data.scale, data.color);
	}
}

void DamageEffector::Reset()
{
	datas.clear();
}

void DamageEffector::Register(const EffectData& data)
{
	datas.emplace_back(data);
}
