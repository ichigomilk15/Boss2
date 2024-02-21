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

// グラフィックス
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// インスタンス取得
	static Graphics& Instance() { return *instance; }

	void DrawDebugGUI();

	// デバイス取得
	ID3D11Device* GetDevice() const { return device.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// シェーダー取得
	Shader* GetShader(int shaderId) const { return shaders[shaderId].get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	//画面サイズの取得
	const DirectX::XMFLOAT2 GetScreenSize()const noexcept { return DirectX::XMFLOAT2{ screenWidth,screenHeight }; }

	// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

#ifdef _DEBUG
	// ImGuiレンダラ取得
	ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }
#endif // _DEBUG

	//ミューテックス取得
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

