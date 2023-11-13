#include <imgui.h>
#include "SceneGame.h"
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

Player::Player()
{
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	model = new Model("Data/Model/Jammo/Jammo.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	hitEffect = new Effect("Data/Effect/Hit.efk");

	//type = 1;
}

Player::~Player()
{
	delete hitEffect;

	delete model;
}

void Player::Update(float elapsedTime)
{
	/*
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//移動操作
	float moveSpeed = 5.0f * elapsedTime;
	{
		//左スティックの入力情報をもとにXZ平面への移動処理


		/*position.x += ax * moveSpeed;
		position.z += ay * moveSpeed;*/
		/*
			/*float speedX = ay * moveSpeed * -sinf(tempAngle.y) + ax * moveSpeed * -cosf(tempAngle.y);
			float speedZ = ay * moveSpeed * -cosf(tempAngle.y) + ax * moveSpeed * sinf(tempAngle.y); */
			/*
				//自分で考えた方法
				/*DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
				position.x += ay * moveSpeed * sinf(tempAngle.y) + ax*moveSpeed * cosf(tempAngle.y);
				position.z += ay * moveSpeed * cosf(tempAngle.y) + ax*moveSpeed * -sinf(tempAngle.y);
			}

			//回転操作
			float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
			if (gamePad.GetButton() & GamePad::BTN_A)
			{
				//X軸回転操作
				angle.x += rotateSpeed;
			}
			if (gamePad.GetButton() & GamePad::BTN_B)
			{
				//Y軸回転操作
				angle.y += rotateSpeed;
			}
			if (gamePad.GetButton() & GamePad::BTN_X)
			{
				//Z軸回転操作
				angle.z += rotateSpeed;
			}
			*/

			//進行ベクトルを取得
			//DirectX::XMFLOAT3 moveVec = GetMoveVec();
			//移動処理
			//float moveSpeed = this->moveSpeed * elapsedTime;
			//position.x += moveSpeed * moveVec.x;
			//position.z += moveSpeed * moveVec.z;

#if 1
	GamePad& gamePad = Input::Instance().GetGamePad(); 
	//Bボタン押下でワンショットアニメーション再生
	if (gamePad.GetButtonDown() & GamePad::BTN_B) 
	{ 
		model->PlayAnimation(0, false, 0.1f); 
	} 
	// ワンショットアニメーション再生が終わったらループアニメーション再生 
	if (!model->IsPlayAnimation()) 
	{ 
		model->PlayAnimation(1, true); 
	}
#endif

	//移動入力しょり
	InputMove(elapsedTime);

	// ジャンプ入力処理
	InputJump();

	// 弾丸入力処理
	InputProjectile();

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();

	//オブジェクト行列を更新
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);



	//if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	//{
	//	//トランスフォーム
	//	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		//位置
	//		ImGui::InputFloat3("Position", &position.x);

	//		//回転
	//		DirectX::XMFLOAT3 a;
	//		a.x = DirectX::XMConvertToDegrees(angle.x);
	//		a.y = DirectX::XMConvertToDegrees(angle.y);
	//		a.z = DirectX::XMConvertToDegrees(angle.z);
	//		ImGui::InputFloat3("Angle", &a.x);
	//		angle.x = DirectX::XMConvertToRadians(a.x);
	//		angle.y = DirectX::XMConvertToRadians(a.y);
	//		angle.z = DirectX::XMConvertToRadians(a.z);

	//		//スケール
	//		ImGui::InputFloat3("Scale", &scale.x);
	//	}
	//}

	/*
	if (ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		//if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	//位置
		//	ImGui::InputFloat3("Position", &position.x);

		//	//回転
		//	DirectX::XMFLOAT3 a;
		//	a.x = DirectX::XMConvertToDegrees(angle.x);
		//	a.y = DirectX::XMConvertToDegrees(angle.y);
		//	a.z = DirectX::XMConvertToDegrees(angle.z);
		//	ImGui::InputFloat3("Angle", &a.x);
		//	angle.x = DirectX::XMConvertToRadians(a.x);
		//	angle.y = DirectX::XMConvertToRadians(a.y);
		//	angle.z = DirectX::XMConvertToRadians(a.z);

		//	//スケール
		//	ImGui::InputFloat3("Scale", &scale.x);
		//}

		//if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	//位置
		//	//DirectX::XMFLOAT3 tempAngle = CameraController::Instance().getAngle();
		//	DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
		//	ImGui::InputFloat3("Angle", &tempAngle.x);

		//	//回転
		//	DirectX::XMFLOAT3 a;
		//	a.x = DirectX::XMConvertToDegrees(tempAngle.x);
		//	a.y = DirectX::XMConvertToDegrees(tempAngle.y);
		//	a.z = DirectX::XMConvertToDegrees(tempAngle.z);
		//	ImGui::InputFloat3("Angle", &a.x);
		//}
	}
	ImGui::End();
	*/
}

void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();
}



void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (jumpCount < jumpLimit)
		{
			Jump(jumpSpeed);
			++jumpCount;
		}
	}
}

void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectCylinderVsCylinder(
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				//10.0f,
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition
			))
			{
				//ダメージを与える
				//enemy->ApplyDamage(1);
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse{ 0, 0, 0 };
#if 0
						DirectX::XMVECTOR Dir = DirectX::XMVectorSet(enemy->GetPosition().x - projectile->GetPosition().x,
							0, enemy->GetPosition().z - projectile->GetPosition().z, 0);
						DirectX::XMVECTOR Impulse = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Dir), 10);

						DirectX::XMStoreFloat3(&impulse, Impulse);
						//impulse = { 15.0f, 0, 15.0f }; //Debug
						//impulse.x = impulse.x;
						impulse.y = 5;
						//impulse.z = impulse.z;
#else
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();

						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;
#endif

						enemy->AddImpulse(impulse);
					}

					//ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}

void Player::OnLanding()
{
	isGround = true;
	jumpCount = 0;
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルを取得はXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力をカメラ右方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRightX + ay * cameraFrontX;
	vec.z = ax * cameraRightZ + ay * cameraFrontZ;
	vec.y = 0.0f;

	return vec;
}

void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	//移動処理
	Move(/*elapsedTime, */moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		//if (Collision::IntersectSphereVsSphere(
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			if (normal.y > 0.8f/*velocity.y < 0 *//*&& position.y >= (enemy->GetPosition().y + enemy->GetHeight()) - (enemy->GetHeight()*0.5f)*/)
			{
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				enemy->SetPosition(outPosition);
			}
		}
	}
}

void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		/*dir.x = transform._31;
		dir.y = transform._32;
		dir.z = transform._33;*/
		DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&dir);
		Dir = DirectX::XMVector3Normalize(Dir);
		DirectX::XMStoreFloat3(&dir, Dir);
		// 発射位置 (プレイヤーの腕あたり)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// 発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);
	}

	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = transform._31 / scale.x;
		dir.y = transform._32 / scale.y;
		dir.z = transform._33 / scale.z;
		/*float dirL = sqrtf(transform._31 * transform._31 + transform._32 * transform._32 + transform._33 * transform._33);
		dir.x /= dirL;
		dir.y /= dirL;
		dir.z /= dirL;*/

		//発射位置（プレイヤーの腕当たり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = this->transform._31;
		target.y = this->transform._32;
		target.z = this->transform._33;
		//target = {50, 50, 50};
		//target = { dir.x + pos.x, dir.y + pos.y, dir.z + pos.z };

		//一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		bool enemyFound = false;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//敵との距離判定
			Enemy* enemy = enemyManager.GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
				enemyFound = true;
			}
		}
		if (!enemyFound)
		{
#if 0
			ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
			projectile->Launch(dir, pos);
#else
			DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
			Target = DirectX::XMVector3Normalize(Target);
			Target = DirectX::XMVectorScale(Target, 1000.0f);
			DirectX::XMStoreFloat3(&target, Target);
#endif
		}
		else
		{
			//発射
			ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
			projectile->Launch(dir, pos, target);
		}
	}
}
