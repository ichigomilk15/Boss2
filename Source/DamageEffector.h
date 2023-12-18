#pragma once

#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>


class DamageEffector
{
public:
	struct EffectData
	{
		int damage;
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT2 velocity;
		DirectX::XMFLOAT4 color;
		float scale;
		float timer;
		bool isDrawSymbol;

	public://functions
		bool operator<(const EffectData& dst);
		bool operator<(const float& value);
	};
public://singleton
	static DamageEffector& Instance() { static DamageEffector instance; return instance; }
private://constractor
	DamageEffector() = default;
	~DamageEffector();
	DamageEffector(const DamageEffector&) = delete;
	DamageEffector operator=(const DamageEffector&) = delete;

public://functions
	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc)const;
	void Reset();
	void Register(const EffectData& data);


private:
	std::vector<EffectData> datas;
};