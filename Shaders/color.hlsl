//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "ShaderH.hlsli"
Texture2D	gDiffuseMap	:	register(t0);

Texture2D	gNormalMap	:	register(t1);

SamplerState	gSamLinear	:	register(s0);

float4	CameraLoc	:	register(b0);

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj; 
	float4x4 gScale3D;
	float4x4  gRotate;
	float gOffset;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;
	float2 TexCoord    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;
	float2 TexCoord    : TEXCOORD;
};

[RootSignature(Test_RootSig)]
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vin.PosL = mul(vin.PosL, 0.2f * sin(gOffset) + 1);
	float4 ScalePos =mul(mul(float4(vin.PosL, 1.0f), gScale3D), gRotate);
	vout.PosH = mul(ScalePos, gWorldViewProj);
    vout.Color = vin.Color;
	vout.Normal = mul(vin.Normal,gRotate);
	vout.TexCoord = vin.TexCoord;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear  , pin.TexCoord);
    return diffuseAlbedo;
}


