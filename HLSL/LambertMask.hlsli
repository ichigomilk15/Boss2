struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
};

cbuffer CbMask : register(b3) //0番にある定数バッファをこれで利用する
{
    float dissolveThreshold;
    float edgeThreshold;
    float2 dummy;
    float4 edgeColor;
}