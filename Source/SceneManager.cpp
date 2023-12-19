
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
			//�Â��V�[�����I������
			Clear();

			//�V�����V�[����ݒ�
			currentScene = nextScene;
			nextScene = nullptr;


			//�V�[������������
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
	//�V�����V�[����ݒ�
	nextScene = scene;
	this->isFadeOut = isFadeOut;
	if(isFadeOut)timer = SceneChangeTime;
}

void SceneManager::SetSceneChangeSprite(std::unique_ptr<Sprite>&& sprite)
{
	sceneChangeSprite.swap(sprite);
}
