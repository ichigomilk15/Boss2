#pragma once

#include "Scene.h"
#include "UI.h"

class SceneClear : public Scene
{
public:
	SceneClear();
	~SceneClear() override;

	//‰Šú‰»
	void Initialize()override;

	//I—¹‰»
	void Finalize()override;

	//XVˆ—
	void Update(float elapseTime)override;

	//•`‰æˆ—
	void Render()override;

private:
	std::unique_ptr<Sprite> back;
};