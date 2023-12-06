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

	//������
	 void Initialize()override;

	//�I����
	 void Finalize() override;

	//�X�V����
	 void Update(float elapsedTime)override;

	//�`�揈��
	 void Render()override;

private:
	std::unique_ptr<Sprite> back;
	UI GoTitle;
	UI Retry;
};