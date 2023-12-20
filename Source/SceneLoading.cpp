#include "Graphics\Graphics.h"
#include "Input\Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <SceneGame.h>

void SceneLoading::Initialize()
{
	//�X�v���C�g������
	sprite = new Sprite("Data/Sprite/LoadingIcon.png");
	back = std::make_unique<Sprite>("./Data/Sprite/loading_back.png");

	//�X���b�h�J�n
	/*using Mover = void(Scene* scene);
	Mover move;
	move = SceneLoading::LoadingThread;*/

	thread = new std::thread(LoadingThread, this);
}

void SceneLoading::Finalize()
{
	//�X���b�h�I����
	if (thread != nullptr)
	{
		if (thread->joinable())
			thread->join();
		delete thread;
		thread = nullptr;
	}

	//�X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//���̃V�[���̏���������������V�[����؂�ւ���
	if (!SceneManager::Instance().IsWaitSceneChange()&&this->nextScene->IsReady())
	{
		this->thread->detach();
		SceneManager::Instance().ChangeScene(this->nextScene,SceneManager::Instance().IsFade());
	}
}

void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//��ʃN���A���^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	//2D�X�v���C�g�`��
	{
		//��ʉE���Ƀ��[�f�B���O�A�C�R����`��
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		float positionX = screenWidth - textureWidth;
		float positionY = screenHeight - textureHeight;

		back->Render(dc,
			.0f, .0f, graphics.GetScreenWidth(), graphics.GetScreenHeight(), .0f, .0f, back->GetTextureWidthf(), back->GetTextureHeightf(), .0f, 1.0f, 1.0f, 1.0f, 1.0f);

		sprite->Render(dc,
			positionX, positionY, textureWidth, textureHeight,
			0, 0, textureWidth, textureHeight,
			angle,
			1, 1, 1, 1);
	}
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	//���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	//�X���b�h���I���O�ɂb�n�l�֘A�̏I����
	CoUninitialize();

	//���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}
