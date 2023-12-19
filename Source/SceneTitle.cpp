#include <memory>

#include "Input\Input.h"
#include "Graphics\Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "HitCollisions.h"
#include "Audio\AudioLoader.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	back = new Sprite("Data/Sprite/title_back.png");
	title =  new Sprite("./Data/Sprite/SceneBack/title.png");
	pressClick = new Sprite("./Data/Sprite/SceneBack/press_click.png");

	AudioLoader::Load(AUDIO::BGM_GAMEOVER, gameOverSe);

	// BGM再生
	gameOverSe->Play(true);
	mask = new Sprite("Data/Sprite/title_back.png");
}

void SceneTitle::Finalize()
{
	//スプライト終了化
	if (back != nullptr)
	{
		delete back;
		back = nullptr;
	}
	if (title != nullptr)
	{
		delete title;
		title = nullptr;
	}
	if (pressClick != nullptr)
	{
		delete pressClick;
		pressClick = nullptr;
	}
	if (mask != nullptr)
	{
		delete mask;
		mask = nullptr;
	}
	gameOverSe->Stop();
}

void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	//ゲーム終了
	if (gamePad.GetButtonDown() & GamePad::BTN_ESCAPE)
	{
		Graphics::Instance().Quit();
	}

	//if (timer <= .0f)
	//{


	//	//なにかボタンを押したらローディングシーンを挟んでゲームシーン切り替え
	//	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	//	{

	//		startTimer = timer = 2.0f;
	//		SaveData::Instance().ReSet();
	//	}
	//}

	//if (timer > .0f)
	//{
	//	timer -= elapsedTime;
	//	if (timer < .0f)
	//	{
	//		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame()));
	//	}
	//}	
	auto& SceneManager = SceneManager::Instance();
	if (! SceneManager.IsWaitSceneChange()&& mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		SaveData::Instance().ReSet();
		SceneManager.SetSceneChangeSprite(std::make_unique<Sprite>("./Data/Sprite/loading_back.png"));
		SceneManager.ChangeScene(new SceneLoading(new SceneGame()));
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
		float textureWidth = static_cast<float>(back->GetTextureWidth());
		float textureHeight = static_cast<float>(back->GetTextureHeight());
		//タイトルスプライト描画
		back->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		//HitBox2D box = HitBox2D::CreateBoxFromCenter({ screenWidth * 0.7f,screenHeight * 0.3f }, { screenWidth * 0.6f,screenHeight * 0.6f });
		title->Render(dc,
			screenWidth*0.3f,screenHeight*.0f,screenWidth*0.6f,screenHeight*0.6f,
			.0f,.0f,title->GetTextureWidthf(),title->GetTextureHeightf(),
			.0f,
			1.0f,1.0f,1.0f,1.0f);

		HitBox2D box = HitBox2D::CreateBoxFromCenter({ screenWidth * 0.5f,screenHeight * 0.9f }, { screenWidth * 0.5f,screenHeight * 0.1f });
		pressClick->Render(dc, box.GetLeftTop(), box.GetBoxSize(), .0f, DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });

		if (timer!=.0f)
		{

			mask->Render(dc,
				.0f, .0f, screenWidth, screenHeight,
				.0f, .0f, mask->GetTextureWidthf(), mask->GetTextureHeightf(),
				.0f,
				1.0f, 1.0f, 1.0f,(startTimer- timer)/startTimer
			);
		}
	}
}
