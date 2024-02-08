#include "Input/Mouse.h"
#include "Graphics/Graphics.h"

static const int KeyMap[] =
{
	VK_LBUTTON,		// ���{�^��
	VK_MBUTTON,		// ���{�^��
	VK_RBUTTON,		// �E�{�^��
};

// �R���X�g���N�^
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;

	tapEffect = std::make_unique<Sprite>("./Data/Effect/tap.png");
}

// �X�V
void Mouse::Update()
{
	// �X�C�b�`���
	MouseButton newButtonState = 0;

	if(Graphics::Instance().GetHWnd()== GetFocus())
		for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
		{
			if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
			{
				newButtonState |= (1 << i);
			}
		}

	// �z�C�[��
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// �{�^�����X�V
	buttonState[1] = buttonState[0];	// �X�C�b�`����
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// �������u��
	buttonUp = ~newButtonState & buttonState[1];	// �������u��

	// �J�[�\���ʒu�̎擾
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// ��ʂ̃T�C�Y���擾����B
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;
	float rateX = 1.0f;
	float rateY = 1.0f;

	// ��ʕ␳
	positionX[1] = positionX[0];
	positionY[1] = positionY[0];
	positionX[0] = (LONG)((cursor.x) / static_cast<double>(viewportW) * static_cast<double>(screenW));
	positionY[0] = (LONG)((cursor.y) / static_cast<double>(viewportH) * static_cast<double>(screenH));

	for (auto& data : effectDatas)
	{
		data.animationFrame++;
	}
	//TIPS : �}�E�X�̃G�t�F�N�g�̉摜�ύX�����ꍇ�����̒l��ύX
	std::erase_if(effectDatas,[](EffectData& data){return data.animationFrame > 4*9 * 2;});

	if (buttonDown & BTN_LEFT)
	{
		EffectData data;
		data.pos = {static_cast<float>(positionX[0]),static_cast<float>(positionY[0])};
		data.animationFrame = 0;
		effectDatas.emplace_back(data);
	}
}

//TIPS : �}�E�X�̃G�t�F�N�g�̉摜�ύX�����ꍇ�����̒l��ύX
void Mouse::Render(ID3D11DeviceContext* dc)
{
	DirectX::XMFLOAT2 size = {75.0f,75.0f};
	DirectX::XMFLOAT2 texSize = {tapEffect->GetTextureWidthf()/4.0f,tapEffect->GetTextureHeightf()/9.0f};
	for (auto& data : effectDatas)
	{
		int animationFrame = data.animationFrame/2;
		DirectX::XMFLOAT2 texPos = { texSize.x * (animationFrame % 4),texSize.y * (animationFrame / 4) };
		tapEffect->Render(dc,
			data.pos.x - size.x*0.5f,
			data.pos.y - size.y*0.5f,
			size.x,size.y,
			texPos.x,texPos.y,
			texSize.x,texSize.y,
			.0f,
			1.0f,1.0f,1.0f,1.0f
		);
	}
}
