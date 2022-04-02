#include "stdafx.h"
#include "DX12Shader.h"

DX12Shader::DX12Shader(const std::string ShaderName, const std::wstring FilePath, std::string VS /*= "VS"*/, std::string PS /*= "PS"*/)
{

}

void DX12Shader::SetPSODesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc)
{
	if (mVS)
	{
		psoDesc->VS =
		{
			reinterpret_cast<BYTE*>(mVS->GetBufferPointer()),
			mVS->GetBufferSize()
		};
	}
	if (mPS)
	{
		psoDesc->PS =
		{
			reinterpret_cast<BYTE*>(mPS->GetBufferPointer()),
			mPS->GetBufferSize()
		};
	}

	psoDesc->pRootSignature = RootSignature.Get();
	psoDesc->RasterizerState = PRasterizerState;
	psoDesc->RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc->DepthStencilState = PDepthStencilState;
	psoDesc->BlendState = PBlendState;

	psoDesc->RasterizerState.DepthBias = 10000;
	psoDesc->RasterizerState.DepthBiasClamp = 0.f;
	psoDesc->RasterizerState.SlopeScaledDepthBias = 1.0f;
}
