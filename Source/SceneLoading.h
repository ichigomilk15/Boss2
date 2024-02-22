#pragma once

#include <thread>
#include <mutex>
#include "Graphics\Sprite.h"
#include "Scene.h"

//���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	//SceneLoading() {}
	~SceneLoading() override {}

	//������
	void Initialize() override;

	//�I����
	void Finalize() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render() override;

	std::mutex& GetLoadingMutex(){return loadingMutex;}

private:
	//���[�f�B���O�X���b�h
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