#include "LambertMask.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);
Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = pin.color * diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    
    float mask = maskTexture.Sample(diffuseMapSamplerState, pin.texcoord).r;
    float alpha = step(mask, dissolveThreshold);
    float edgeValue = step(mask - dissolveThreshold, dissolveThreshold) * step(dissolveThreshold, mask)
		* step(mask, dissolveThreshold + edgeThreshold);
    color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);
    color.a *= alpha;
    clip(color.a - 0.01f);
    
    return color;
}

