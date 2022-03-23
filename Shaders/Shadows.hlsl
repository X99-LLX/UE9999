
#include "ShadowH.hlsli"


Texture2D	ShadowMap	:	register(t0);

Texture2D	SomeTex	:	register(t1);

cbuffer cbPerObject : register(b0)
{
	float4x4	gWorld;
	float4x4	gView;
	float4x4	gProj;
};


struct VertexIn
{
	float3 PosL			: POSITION;
	float4 Color		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct VertexOut
{
	float3 PosL			: POSITION;
	float4 Color		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

[RootSignature(Shadow_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	
    return vout;
}

void PS(VertexOut pin) 
{
	
}


