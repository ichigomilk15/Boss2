#include "NumberSprite.h"

NumberSprite::NumberSprite():numbers("./Data/Font/numbers.png")
{
}

void NumberSprite::NumberOut(const char* nums, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color)const
{
	const DirectX::XMFLOAT2 texNumSize = { numbers.GetTextureWidthf() / 12.0f,numbers.GetTextureHeightf() };
	const int numCount = strlen(nums);
	const DirectX::XMFLOAT2 numSize = { size.x / numCount,size.y };
	DirectX::XMFLOAT2 nowPos = pos;
	while (*nums!='\0')
	{ 
		const char num = *nums;
		if (num == '+')
		{
			DirectX::XMFLOAT2 texpos = { texNumSize.x * 10,texNumSize.y };
			numbers.Render(dc,nowPos,numSize,texpos,texNumSize,.0f,color);
		}
		else if (num == '-')
		{
			DirectX::XMFLOAT2 texpos = { texNumSize.x * 11,texNumSize.y };
			numbers.Render(dc, nowPos, numSize, texpos, texNumSize, .0f, color);
		}
		else if('0'<=num&&num<='9')
		{
			DirectX::XMFLOAT2 texpos = { texNumSize.x * (num - '0'),texNumSize.y };
			numbers.Render(dc, nowPos, numSize, texpos, texNumSize, .0f, color);
		}

		nowPos.x += numSize.x;
		++nums;
	}
}
