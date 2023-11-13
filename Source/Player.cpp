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

	//���f�����傫���̂ŃX�P�[�����O
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
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�ړ�����
	float moveSpeed = 5.0f * elapsedTime;
	{
		//���X�e�B�b�N�̓��͏������Ƃ�XZ���ʂւ̈ړ�����


		/*position.x += ax * moveSpeed;
		position.z += ay * moveSpeed;*/
		/*
			/*float speedX = ay * moveSpeed * -sinf(tempAngle.y) + ax * moveSpeed * -cosf(tempAngle.y);
			float speedZ = ay * moveSpeed * -cosf(tempAngle.y) + ax * moveSpeed * sinf(tempAngle.y); */
			/*
				//�����ōl�������@
				/*DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
				position.x += ay * moveSpeed * sinf(tempAngle.y) + ax*moveSpeed * cosf(tempAngle.y);
				position.z += ay * moveSpeed * cosf(tempAngle.y) + ax*moveSpeed * -sinf(tempAngle.y);
			}

			//��]����
			float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
			if (gamePad.GetButton() & GamePad::BTN_A)
			{
				//X����]����
				angle.x += rotateSpeed;
			}
			if (gamePad.GetButton() & GamePad::BTN_B)
			{
				//Y����]����
				angle.y += rotateSpeed;
			}
			if (gamePad.GetButton() & GamePad::BTN_X)
			{
				//Z����]����
				angle.z += rotateSpeed;
			}
			*/

			//�i�s�x�N�g�����擾
			//DirectX::XMFLOAT3 moveVec = GetMoveVec();
			//�ړ�����
			//float moveSpeed = this->moveSpeed * elapsedTime;
			//position.x += moveSpeed * moveVec.x;
			//position.z += moveSpeed * moveVec.z;

#if 1
	GamePad& gamePad = Input::Instance().GetGamePad(); 
	//B�{�^�������Ń����V���b�g�A�j���[�V�����Đ�
	if (gamePad.GetButtonDown() & GamePad::BTN_B) 
	{ 
		model->PlayAnimation(0, false, 0.1f); 
	} 
	// �����V���b�g�A�j���[�V�����Đ����I������烋�[�v�A�j���[�V�����Đ� 
	if (!model->IsPlayAnimation()) 
	{ 
		model->PlayAnimation(1, true); 
	}
#endif

	//�ړ����͂����
	InputMove(elapsedTime);

	// �W�����v���͏���
	InputJump();

	// �e�ۓ��͏���
	InputProjectile();

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փˏ���
	CollisionProjectilesVsEnemies();

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);

	//���f���s��X�V
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);



	//if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	//{
	//	//�g�����X�t�H�[��
	//	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		//�ʒu
	//		ImGui::InputFloat3("Position", &position.x);

	//		//��]
	//		DirectX::XMFLOAT3 a;
	//		a.x = DirectX::XMConvertToDegrees(angle.x);
	//		a.y = DirectX::XMConvertToDegrees(angle.y);
	//		a.z = DirectX::XMConvertToDegrees(angle.z);
	//		ImGui::InputFloat3("Angle", &a.x);
	//		angle.x = DirectX::XMConvertToRadians(a.x);
	//		angle.y = DirectX::XMConvertToRadians(a.y);
	//		angle.z = DirectX::XMConvertToRadians(a.z);

	//		//�X�P�[��
	//		ImGui::InputFloat3("Scale", &scale.x);
	//	}
	//}

	/*
	if (ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		//if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	//�ʒu
		//	ImGui::InputFloat3("Position", &position.x);

		//	//��]
		//	DirectX::XMFLOAT3 a;
		//	a.x = DirectX::XMConvertToDegrees(angle.x);
		//	a.y = DirectX::XMConvertToDegrees(angle.y);
		//	a.z = DirectX::XMConvertToDegrees(angle.z);
		//	ImGui::InputFloat3("Angle", &a.x);
		//	angle.x = DirectX::XMConvertToRadians(a.x);
		//	angle.y = DirectX::XMConvertToRadians(a.y);
		//	angle.z = DirectX::XMConvertToRadians(a.z);

		//	//�X�P�[��
		//	ImGui::InputFloat3("Scale", &scale.x);
		//}

		//if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	//�ʒu
		//	//DirectX::XMFLOAT3 tempAngle = CameraController::Instance().getAngle();
		//	DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
		//	ImGui::InputFloat3("Angle", &tempAngle.x);

		//	//��]
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

	//�Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
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

	//�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
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
				//�_���[�W��^����
				//enemy->ApplyDamage(1);
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//������΂�
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

					//�q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//�e�۔j��
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
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g�����擾��XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//�X�e�B�b�N�̐������͂��J�����E�����ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRightX + ay * cameraFrontX;
	vec.z = ax * cameraRightZ + ay * cameraFrontZ;
	vec.y = 0.0f;

	return vec;
}

void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	//�ړ�����
	Move(/*elapsedTime, */moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
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

	// ���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// �O����
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
		// ���ˈʒu (�v���C���[�̘r������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// ����
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);
	}

	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = transform._31 / scale.x;
		dir.y = transform._32 / scale.y;
		dir.z = transform._33 / scale.z;
		/*float dirL = sqrtf(transform._31 * transform._31 + transform._32 * transform._32 + transform._33 * transform._33);
		dir.x /= dirL;
		dir.y /= dirL;
		dir.z /= dirL;*/

		//���ˈʒu�i�v���C���[�̘r������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//�^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
		DirectX::XMFLOAT3 target;
		target.x = this->transform._31;
		target.y = this->transform._32;
		target.z = this->transform._33;
		//target = {50, 50, 50};
		//target = { dir.x + pos.x, dir.y + pos.y, dir.z + pos.z };

		//��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		bool enemyFound = false;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//�G�Ƃ̋�������
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
			//����
			ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
			projectile->Launch(dir, pos, target);
		}
	}
}
