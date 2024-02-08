#include "Input/Mouse.h"
#include "Graphics/Graphics.h"

static const int KeyMap[] =
{
	VK_LBUTTON,		// 左ボタン
	VK_MBUTTON,		// 中ボタン
	VK_RBUTTON,		// 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;

	tapEffect = std::make_unique<Sprite>("./Data/Effect/tap.png");
}

// 更新
void Mouse::Update()
{
	// スイッチ情報
	MouseButton newButtonState = 0;

	if(Graphics::Instance().GetHWnd()== GetFocus())
		for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
		{
			if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
			{
				newButtonState |= (1 << i);
			}
		}

	// ホイール
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// ボタン情報更新
	buttonState[1] = buttonState[0];	// スイッチ履歴
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// 押した瞬間
	buttonUp = ~newButtonState & buttonState[1];	// 離した瞬間

	// カーソル位置の取得
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;
	float rateX = 1.0f;
	float rateY = 1.0f;

	// 画面補正
	positionX[1] = positionX[0];
	positionY[1] = positionY[0];
	positionX[0] = (LONG)((cursor.x) / static_cast<double>(viewportW) * static_cast<double>(screenW));
	positionY[0] = (LONG)((cursor.y) / static_cast<double>(viewportH) * static_cast<double>(screenH));

	for (auto& data : effectDatas)
	{
		data.animationFrame++;
	}
	//TIPS : マウスのエフェクトの画像変更した場合ここの値を変更
	std::erase_if(effectDatas,[](EffectData& data){return data.animationFrame > 4*9 * 2;});

	if (buttonDown & BTN_LEFT)
	{
		EffectData data;
		data.pos = {static_cast<float>(positionX[0]),static_cast<float>(positionY[0])};
		data.animationFrame = 0;
		effectDatas.emplace_back(data);
	}
}

//TIPS : マウスのエフェクトの画像変更した場合ここの値を変更
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
