#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

//�G�t�F�N�g
class Effect
{
public:
	Effect(const char* filename);
	~Effect() {};

	//�Đ�
	Effekseer::Handle Play(const DirectX::XMFLOAT3& positionWorld, float scale = 1.0f);

	//��~
	void Stop(Effekseer::Handle handle);

	//���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& positionWorld);

	//�X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	const Effekseer::Handle& GetHandle()const noexcept { return handle; }

private:
	Effekseer::EffectRef effekseerEffect;
	Effekseer::Handle handle;
	unsigned long long startTime = 0ull;
	unsigned long long animationTime = 0ull;
};