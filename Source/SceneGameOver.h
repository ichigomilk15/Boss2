#pragma once

#include <memory>

#include "Scene.h"
#include "UI.h"

#include "Graphics/Sprite.h"


class SceneGameOver : public Scene
{
public:
	SceneGameOver();
	~SceneGameOver();

	//‰Šú‰»
	 void Initialize()override;

	//I—¹‰»
	 void Finalize() override;

	//XVˆ—
	 void Update(float elapsedTime)override;

	//•`‰æˆ—
	 void Render()override;

private:
	std::unique_ptr<Sprite> back;
	UI GoTitle;
	UI Retry;
};