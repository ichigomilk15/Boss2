
#define NOMINMAX
#include "SceneManager.h"
#include "Graphics/Graphics.h"
#undef NOMINMAX

void SceneManager::Update(float elapsedTime)
{
	if (nextScene != nullptr)
	{
		timer -= elapsedTime;
		if (timer < .0f)
		{
			//古いシーンを終了処理
			Clear();

			//新しいシーンを設定
			currentScene = nextScene;
			nextScene = nullptr;


			//シーン初期化処理
			if (!(currentScene->IsReady()))
			{
				currentScene->Initialize();
			}
		}
	}
	else if(isFadeOut)
	{
		timer = std::max(timer+elapsedTime,SceneChangeTime);
	}

	if (currentScene != nullptr)
	{
		currentScene->Update(elapsedTime);
	}
}

void SceneManager::Render()
{
	if (currentScene != nullptr)
	{
		currentScene->Render();
	}

	if (sceneChangeSprite&& timer != SceneChangeTime)
	{
		sceneChangeSprite->Render(Graphics::Instance().GetDeviceContext(), DirectX::XMFLOAT2{ .0f,.0f }, Graphics::Instance().GetScreenSize(), .0f, { 1.0f,1.0f,1.0f,1.0f-(timer/SceneChangeTime) });
	}
}

void SceneManager::Clear()
{
	if (currentScene != nullptr)
	{
		currentScene->Finalize();
		delete currentScene;
		currentScene = nullptr;
	}
}

void SceneManager::ChangeScene(Scene* scene, bool isFadeOut)
{
	if (nextScene)return;
	//新しいシーンを設定
	nextScene = scene;
	this->isFadeOut = isFadeOut;
	if(isFadeOut)timer = SceneChangeTime;
}

void SceneManager::SetSceneChangeSprite(std::unique_ptr<Sprite>&& sprite)
{
	sceneChangeSprite.swap(sprite);
}
