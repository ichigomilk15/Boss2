#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

//�G�t�F�N�g�}�l�[�W���[
class EffectManager
{
private:
	EffectManager() {}
	~EffectManager() {}

public:
	//�B��̃C���X�^���X�擾
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	//������
	void Initialize();;

	//�I����
	void Finalize();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	//Effekseer�}�l�[�W���[�̎擾
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

	const unsigned long long& GetTimer()const noexcept { return timer; }

private:
	Effekseer::ManagerRef effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;
	unsigned long long timer=0ull;
};