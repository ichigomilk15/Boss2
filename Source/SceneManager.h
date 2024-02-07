#pragma once

#include "Scene.h"
#include "Graphics/Sprite.h"

//シーンマネジャー
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	//唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render();

	//シーンクリア
	void Clear();

	//シーン切り替え
	void ChangeScene(Scene* scene,bool isFadeOut = false);
	const bool& IsFade()const { return isFadeOut; };

	//シーン変更の画像設定
	void SetSceneChangeSprite(std::unique_ptr<Sprite>&& sprite);

	bool IsWaitSceneChange()const { return nextScene!=nullptr; };

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
	std::unique_ptr<Sprite>	sceneChangeSprite;
	float SceneChangeTime= 1.0f;
	float timer = .0f;
	bool isFadeOut = false;
};