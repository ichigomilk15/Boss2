#pragma once
#include "Graphics//Graphics.h"
#include "Graphics\Sprite.h"
#include "SceneGame.h"
#include "CameraController.h"
#include "Scene.h"
#include "Player.h"
#include "CardList.h"

//void DrawDebugGUI(Player* player, CameraController* cameraController);

class SaveData
{
public:
	static SaveData& Instance() { static SaveData instance; return instance; }
private:
	SaveData() = default;
	~SaveData() = default;

public:
	const bool Save();
	const bool ReSet();
	const bool Load();

public:
	int StageLevel = 1;
	int playerHp = -1;
	std::pair<int,int> playerpos{-1,-1};
	int PhaseTurn = 0;
};

// ゲームシーン
class SceneGame : public Scene
{
public:

	SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	void DrawDebugGUI();

	const CameraController* GetCameraController() const { return cameraController; }

private:
	//エネミーHPゲージ描画
	void RenderEnemyGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	void SetGlobalDirection();

private://members
	//Player* player = nullptr;
	CameraController* cameraController = &(CameraController::Instance());
	std::unique_ptr<Sprite> playerHP[3];
#ifdef _DEBUG
	Sprite sprite;
#endif // _DEBUG

};
