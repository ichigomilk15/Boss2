#include "Input\Input.h"
#include <Graphics\Graphics.h>
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");
}

void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	//なにかボタンを押したらローディングシーンを挟んでゲームシーン切り替え
	if (gamePad.GetButtonDown()||
		fabsf(gamePad.GetAxisLX())>=0.2f||
		fabsf(gamePad.GetAxisLY())>=0.2f||
		fabsf(gamePad.GetAxisRX())>=0.2f||
		fabsf(gamePad.GetAxisRY())>= 0.2f)
	{
		//SceneManager::Instance().ChangeScene(new SceneGame());
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
	}
}

void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//画面クリア＆レンダーターゲット設定i
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		//タイトルスプライト描画
		sprite->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}
