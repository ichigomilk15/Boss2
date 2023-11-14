#pragma once
#include "Graphics//Graphics.h"
#include "Graphics\Sprite.h"
#include "SceneGame.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"

//void DrawDebugGUI(Player* player, CameraController* cameraController);

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	/*static SceneGame& Instance()
	{
		static SceneGame sceneGame;
		return sceneGame;
	}*/

	SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

	void DrawDebugGUI();

	const CameraController* GetCameraController() const { return cameraController; }

private:
	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
};
