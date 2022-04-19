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

	float temp = gOffset[0];
	float2 v = Tex - float2(0.5, 0.5);
	float2 Offset = sin((length(v) * 5 + temp) * 3.14 * 2) * normalize(v) * 0.1;

	float4 Color = gSceneColor.Sample(gSceneColorSampler, Tex + Offset);

	return Color;
}


