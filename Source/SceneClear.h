#pragma once

#include "Scene.h"
#include "UI.h"

class SceneClear : public Scene
{
public:
	SceneClear();
	~SceneClear() override;

	//������
	void Initialize()override;

	//�I����
	void Finalize()override;

	//�X�V����
	void Update(float elapseTime)override;

	//�`�揈��
	void Render()override;

private:
	std::unique_ptr<Sprite> back;
};