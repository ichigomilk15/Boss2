#include <imgui.h>
#include "SceneGame.h"
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Stage.h"

Player::Player()
{
	model = new Model("Data/Model/Jammo/Jammo.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	hitEffect = new Effect("Data/Effect/Hit.efk");

	state = State::Move_Init;

	//type = 1;
}

Player::~Player()
{
	delete hitEffect;

	delete model;
}

void Player::Update(float elapsedTime)
{
#if 0
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

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	//�X�e�[�g�X�V����
	UpdateState(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);

	//���f���s��X�V
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		//if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			ImGui::InputInt2("Position Square", &position.x);
			ImGui::InputFloat3("Position World", &positionWorld.x);

			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
		}
	}
	ImGui::End();
}

void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}


DirectX::XMINT2 Player::GetMoveVec() const
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	/*float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();*/
	DirectX::XMINT2 move = { 0, 0 };
	switch (gamePad.GetButtonDown() & (GamePad::BTN_LEFT | GamePad::BTN_RIGHT | GamePad::BTN_UP | GamePad::BTN_DOWN))
	{
	case GamePad::BTN_LEFT:
		move.x = -1;
		break;
	case GamePad::BTN_RIGHT:
		move.x = 1;
		break;
	case GamePad::BTN_UP:
		move.y = -1;
		break;
	case GamePad::BTN_DOWN:
		move.y = 1;
		break;
	}
	return move;
}

void Player::InputMove(float elapsedTime)
{
	//�i�s�}�X�̈ʒu�擾
	DirectX::XMINT2 movePos = GetMoveVec();
	//�ړ�����
	Move(movePos.x, movePos.y);
}

void Player::UpdateState(float elapsedTime)
{
	float elapsedFrame = 60.0f * elapsedTime;

	switch (state)
	{
	case State::Idle_Init:

		state = State::Idle;
		[[fallthrough]];
	case State::Idle:

		break;

	case State::Act_Init:

		state = State::Act;
		[[fallthrough]];
	case State::Act:

		break;

	case State::Move_Init:
		this->targetMovePos = { -1, -1 };
		state = State::Move;
		[[fallthrough]];
	case State::Move:
		UpdateMove(elapsedTime);

		if (Stage::Instance()->IsInArea(this->targetMovePos.x, this->targetMovePos.y))
		{
			this->state = State::Moving_Init;
			break;
		}
		break;
	case State::Moving_Init:

		state = State::Moving;
		[[fallthrough]];
	case State::Moving:
		if (!IsMoving())
		{
			this->state = State::Move_Init;
			break;
		}
		break;
	}
}

void Player::UpdateMove(float elapsedTime)
{
	for (int y = 0; y < Common::SQUARE_NUM_Y; ++y)
	{
		for (int x = 0; x < Common::SQUARE_NUM_X; x++)
		{
			Stage::Instance()->GetSquare(x, y).get()->ResetSquare();
		}
	}

	Mouse& mouse = Input::Instance().GetMouse();
	auto dc = Graphics::Instance().GetDeviceContext();
	Camera& camera = Camera::Instance();

	const int moveRange = 2;

	auto squares = Stage::Instance()->GetSquares(this->position.x, this->position.y, moveRange);

	for (auto& square : squares)
	{
		square->SetType(Square::Type::MoveArea);
	}

	DirectX::XMFLOAT3 startMousePos = CommonClass::GetWorldStartPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());
	DirectX::XMFLOAT3 endMousePos = CommonClass::GetWorldEndPosition(dc, mouse.GetPositionX(), mouse.GetPositionY(), camera.GetView(), camera.GetProjection());

	HitResult hit;
	Square* foundSq = nullptr;
	for (auto& sq : squares)
	{
		if (sq->Raycast(startMousePos, endMousePos, hit))
		{
			sq->SetType(Square::Type::MoveAreaChosen);
			foundSq = sq;
		}
	}
	if (foundSq && mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		Stage::Instance()->ResetAllSquare();
		this->targetMovePos = foundSq->GetPos();
	}
}
