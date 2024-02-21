#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include "Graphics/Shader.h"
#include "Graphics/DebugRenderer.h"
#include "Graphics/LineRenderer.h"
#include "Graphics/ImGuiRenderer.h"

enum Shader3D
{
	Lambert,
	LambertMask,
	Max
};

// �O���t�B�b�N�X
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// �C���X�^���X�擾
	static Graphics& Instance() { return *instance; }

	void DrawDebugGUI();

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// �V�F�[�_�[�擾
	Shader* GetShader(int shaderId) const { return shaders[shaderId].get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	//��ʃT�C�Y�̎擾
	const DirectX::XMFLOAT2 GetScreenSize()const noexcept { return DirectX::XMFLOAT2{ screenWidth,screenHeight }; }

	// �f�o�b�O�����_���擾
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

#ifdef _DEBUG
	// ImGui�����_���擾
	ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }
#endif // _DEBUG

	//�~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

	const HWND& GetHWnd()const noexcept { return hwnd; }

	void Quit()const noexcept;

	const bool IsFullScreen() { return isFullScreen; }

	const float& GetScreenScale()const noexcept{return screenScale;}

private:
	static Graphics*								instance;

	const HWND hwnd;
	float screenScale = 1.0f;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	std::unique_ptr<Shader>							shaders[Shader3D::Max];
	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;
#ifdef _DEBUG
	std::unique_ptr<ImGuiRenderer>					imguiRenderer;
#endif // _DEBUG

	float	screenWidth;
	float	screenHeight;

	BOOL isFullScreen = FALSE;

	std::mutex mutex;
};

