#pragma once

#include "Graphics\Sprite.h"
#include "Scene.h"

//タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

private:
	Sprite* back = nullptr;
	Sprite* title = nullptr;
	Sprite* pressClick = nullptr;
	Sprite* mask = nullptr;

	struct TitlePosDetail
	{
		DirectX::XMFLOAT2 pos{ 360.0f, 0.0f };
		float scale = 0.89f;
	};
	TitlePosDetail titlePosDetail{};

	float startTimer;
	float timer;
	float SceneTimer = .0f;
};