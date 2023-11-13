#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"

// 初期化
void SceneGame::Initialize()
{
	//カメラコントローラー初期化
	cameraController = new CameraController();
	player = new Player();

	//ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	//EnemyManager::Instance().Register(new EnemySlime());
	for (int i = 0; i < 1; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(-3 + i * 3.0f, 0, 5 * (i % 3)));
		enemyManager.Register(slime);
	}
	
	//ゲージスプライト
	gauge = new Sprite();
}

// 終了化l
void SceneGame::Finalize()
{
	//ゲージスプライト終了化
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	EnemyManager::Instance().Clear();
	//カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	/*if (enemyManager != nullptr)
	{
		delete enemyManager;
		enemyManager = nullptr;
	}*/
	//ステージ終了化
	StageManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->setTarget(target);
	cameraController->Update(elapsedTime);

	StageManager::Instance().Update(elapsedTime);
	player->Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime);

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		//ステージ描画
		StageManager::Instance().Render(dc, shader);

		//プレイヤー描画
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);

	}

	//3Dモデル描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		graphics.GetDebugRenderer()->Render(dc, rc.view, player->GetTransform());
		player->DrawDebugPrimitive();
		//player->DrawDebugGUI();
		EnemyManager::Instance().DrawDebugPrimitive();
		//player->getProjectileManagaer().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2DデバッグGUI描画
	{
		//プレイヤーデバッグ描画
		//player->DrawDebugGUI();
		DrawDebugGUI();
		//DrawDebugGUI(player, cameraController);
	}
}

//void DrawDebugGUI(Player* player, CameraController* cameraController)
void SceneGame::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Window 1", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			////位置
			//ImGui::InputFloat3("Position", &(player->GetPosition()).x);

			////回転
			DirectX::XMFLOAT3 a;
			DirectX::XMFLOAT3 playerAngle = player->GetAngle();
			a.x = DirectX::XMConvertToDegrees(playerAngle.x);
			a.y = DirectX::XMConvertToDegrees(playerAngle.y);
			a.z = DirectX::XMConvertToDegrees(playerAngle.z);
			ImGui::InputFloat3("Angle", &a.x);
			playerAngle.x = DirectX::XMConvertToRadians(a.x);
			playerAngle.y = DirectX::XMConvertToRadians(a.y);
			playerAngle.z = DirectX::XMConvertToRadians(a.z);

			////スケール
			//ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float playerSpeed = player->getMoveSpeed();
			ImGui::InputFloat("MoveSpeed", &playerSpeed);
			player->setMoveSpeed(playerSpeed);

			float playerTurnSpeed = DirectX::XMConvertToDegrees(player->getTurnSpeed());
			ImGui::InputFloat("TurnSpeed", &playerTurnSpeed);
			player->setTurnSpeed(DirectX::XMConvertToRadians(playerTurnSpeed));

			DirectX::XMFLOAT3 playerVelocity = player->getVelocity();
			float lengthVel = sqrtf(playerVelocity.x * playerVelocity.x + playerVelocity.z * playerVelocity.z);
			ImGui::InputFloat("Movement Speed", &lengthVel);
		}

		if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			//DirectX::XMFLOAT3 tempAngle = CameraController::Instance().getAngle();
			DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
			//ImGui::InputFloat3("Angle", &tempAngle.x);

			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(tempAngle.x);
			a.y = DirectX::XMConvertToDegrees(tempAngle.y);
			a.z = DirectX::XMConvertToDegrees(tempAngle.z);
			ImGui::InputFloat3("Angle", &a.x);
		}

		if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(player->groundNormal.x);
			a.y = DirectX::XMConvertToDegrees(player->groundNormal.y);
			a.z = DirectX::XMConvertToDegrees(player->groundNormal.z);
			ImGui::InputFloat3("Angle", &a.x);
		}
		ImGui::End();
	}
}

void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc, 
	const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//全ての敵の頭上にHPゲージを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	float enemyHpSizeX = 15;

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		//ワールド　-> ビュー変換　-> クリップ座標　-> 正規化座標　->　スクリーン座標
		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();
		DirectX::XMVECTOR EnemyPos;

		EnemyPos = DirectX::XMVectorSet(
			enemyPos.x - enemy->GetRadius(), enemyPos.y + enemy->GetHeight() + 0.1f, enemyPos.z, 0);
		DirectX::XMVECTOR ScreenPositionEnemyLeft = DirectX::XMVector3Project(
			EnemyPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 screenPosEnemyLeft;
		DirectX::XMStoreFloat3(&screenPosEnemyLeft, ScreenPositionEnemyLeft);
		if (screenPosEnemyLeft.z > 1.0f) continue;

		EnemyPos = DirectX::XMVectorSet(
			enemyPos.x + enemy->GetRadius(), enemyPos.y + enemy->GetHeight() + 0.1f, enemyPos.z, 0);
		DirectX::XMVECTOR ScreenPositionEnemyRight = DirectX::XMVector3Project(
			EnemyPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 screenPosEnemyRight;
		DirectX::XMStoreFloat3(&screenPosEnemyRight, ScreenPositionEnemyRight);

		//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
		float enemyHealthWidth = (screenPosEnemyRight.x - screenPosEnemyLeft.x) * enemy->GetHealth() / enemy->GetMaxHealth();
		int enmHealthReduction = enemy->GetMaxHealth() - enemy->GetHealth();
		gauge->Render(
			dc, screenPosEnemyLeft.x + enemyHealthWidth * enmHealthReduction / enemy->GetMaxHealth(), screenPosEnemyLeft.y,
			enemyHealthWidth, 8, 
			0, 0, 0, 0, 0,
			1, 0, 0, 1
		);
	}

	//エネミー配置処理
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		DirectX::XMVECTOR ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		DirectX::XMVECTOR StartRay2D = DirectX::XMVectorSet(
			mouse.GetPositionX(), 
			mouse.GetPositionY(),
			viewport.MinDepth, 0);
		DirectX::XMVECTOR StartRayVec = DirectX::XMVector3Unproject(
			StartRay2D,
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
		);
		DirectX::XMVECTOR EndRay2D = DirectX::XMVectorSet(
			mouse.GetPositionX(),
			mouse.GetPositionY(),
			viewport.MaxDepth, 0);
		DirectX::XMVECTOR EndRayVec = DirectX::XMVector3Unproject(
			EndRay2D,
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
		);

		DirectX::XMFLOAT3 startRay, endRay;
		DirectX::XMStoreFloat3(&startRay, StartRayVec);
		DirectX::XMStoreFloat3(&endRay, EndRayVec);

		HitResult hit;
		if (StageManager::Instance().RayCast(startRay, endRay, hit))
		{
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(DirectX::XMFLOAT3(hit.position.x, hit.position.y, hit.position.z));
			enemyManager.Register(slime);
		}
	}
}
