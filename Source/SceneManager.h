#pragma once

#include "Scene.h"
#include "Graphics/Sprite.h"

//�V�[���}�l�W���[
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	//�B��̃C���X�^���X�擾
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render();

	//�V�[���N���A
	void Clear();

	//�V�[���؂�ւ�
	void ChangeScene(Scene* scene,bool isFadeOut = false);
	const bool& IsFade()const { return isFadeOut; };

	//�V�[���ύX�̉摜�ݒ�
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