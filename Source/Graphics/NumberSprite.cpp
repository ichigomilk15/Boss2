#include "NumberSprite.h"

NumberSprite::NumberSprite():numbers("./Data/Font/numbers.png")
{
}

void NumberSprite::NumberOut(const char* nums, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color)const
{
	const DirectX::XMFLOAT2 numSize = { numbers.GetTextureWidthf() / 12.0f,numbers.GetTextureHeightf() };
	DirectX::XMFLOAT2 nowPos = pos;
	while (*nums!='\0')
	{ 
		const char num = *nums;
		if (num == '+')
		{
			DirectX::XMFLOAT2 texpos = { numSize.x * 10,numSize.y };
			numbers.Render(dc,nowPos,size,texpos,numSize,.0f,color);
		}
		else if (num == '-')
		{
			DirectX::XMFLOAT2 texpos = { numSize.x * 11,numSize.y };
			numbers.Render(dc, nowPos, size, texpos, numSize, .0f, color);
		}
		else if('0'<=num&&num<='9')
		{
			DirectX::XMFLOAT2 texpos = { numSize.x * (num - '0'),numSize.y };
			numbers.Render(dc, nowPos, size, texpos, numSize, .0f, color);
		}

		nowPos.x += size.x;
		++nums;
	}
}
