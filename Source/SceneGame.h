#pragma once
#include "Graphics//Graphics.h"
#include "Graphics\Sprite.h"
#include "SceneGame.h"
#include "CameraController.h"
#include "Scene.h"
#include "Player.h"
#include "CardList.h"

//void DrawDebugGUI(Player* player, CameraController* cameraController);

// �Q�[���V�[��
class SceneGame : public Scene
{
public:

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
	//�G�l�~�[HP�Q�[�W�`��
	void RenderEnemyGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

private://members
	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	std::unique_ptr<Sprite> playerHP;
	std::unique_ptr<Card> testCard;
};
