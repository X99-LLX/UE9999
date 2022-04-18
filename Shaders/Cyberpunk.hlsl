#include "Cyberpunk.hlsli"

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


[RootSignature(Cyberpunk_RootSig)]

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

	float2 Offset;
	float temp;

	temp = 5.0f * sin(gOffset.x);
	
	
	Offset.x = temp / Width;
	Offset.y = temp / Height;

	float R = gSceneColor.Sample(gSceneColorSampler, Tex + float2(-Offset.x, -Offset.y)).x;
	float G = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+Offset.x, +Offset.y)).y;
	float B = gSceneColor.Sample(gSceneColorSampler, Tex + float2(+0, -Offset.y)).z;
	float A = 1.0f;

	float4 Color = { R,G,B,A };
	return Color;
}


