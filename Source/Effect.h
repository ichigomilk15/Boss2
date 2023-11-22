#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

//エフェクト
class Effect
{
public:
	Effect(const char* filename);
	~Effect() {};

	//再生
	Effekseer::Handle Play(const DirectX::XMFLOAT3& positionWorld, float scale = 1.0f);

	//停止
	void Stop(Effekseer::Handle handle);

	//座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& positionWorld);

	//スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	const Effekseer::Handle& GetHandle()const noexcept { return handle; }

private:
	Effekseer::EffectRef effekseerEffect;
	Effekseer::Handle handle;
	unsigned long long startTime = 0ull;
	unsigned long long animationTime = 0ull;
};