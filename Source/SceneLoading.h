#pragma once

#include <thread>
#include <mutex>
#include "Graphics\Sprite.h"
#include "Scene.h"

//ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	//SceneLoading() {}
	~SceneLoading() override {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

	std::mutex& GetLoadingMutex(){return loadingMutex;}

private:
	//ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	Sprite* sprite = nullptr;
	std::unique_ptr<Sprite> back;
	float angle = 0.0f;
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;
	std::mutex loadingMutex;
	float timer = .0f;
};