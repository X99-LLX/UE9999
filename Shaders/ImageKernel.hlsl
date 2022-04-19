#include "Water.hlsli"

Texture2D gSceneColor : register(t0);
SamplerState gSceneColorSampler : register(s0);

float2 RenderTargetSize : register(b0);

float2 gOffset			: register(b1);

struct VertexIn
{
	float3 PosL			: POSITION;
	float4 Tangent		: COLOR;
	float4 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH			: SV_POSITION;

};


[RootSignature(Water_RootSig)]

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	int X = floor(pin.PosH.x);
	int Y = floor(pin.PosH.y);
	float Width = RenderTargetSize[0];
	float Height = RenderTargetSize[1];
	float2 Tex;

	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;


	float x = 1.0f / Width;
	float y = 1.0f / Height;


	float4 Color1 = gSceneColor.Sample(gSceneColorSampler, Tex) * 8;
	float4 Color2 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-x , -y));
	float4 Color3 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(0.0f , -y));
	float4 Color4 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+x , -y));
	float4 Color5 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-x , 0.0f));
	float4 Color6 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+x , 0.0f));
	float4 Color7 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-x , +y));
	float4 Color8 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+x , +y));
	float4 Color9 = gSceneColor.Sample(gSceneColorSampler, Tex + float2(0.0f , +y));
	
	Color1 = Color1 - Color2;
	Color1 = Color1 - Color3;
	Color1 = Color1 - Color4;
	Color1 = Color1 - Color5;
	Color1 = Color1 - Color6;
	Color1 = Color1 - Color7;
	Color1 = Color1 - Color8;
	Color1 = Color1 - Color9;

	return Color1;
}


