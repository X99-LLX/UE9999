#pragma once
#include "Shader.h"


enum class ShaderParamType
{
    ConstantBuffer = 0,
    CBVHeap = 1,
    SRVHeap = 2,
    UAVHeap = 3,
};

struct ShaderParameter
{
	std::string name;
	ShaderParamType type;
	UINT descriptorNums;
	UINT baseRegister;
	UINT registerSpace;

	ShaderParameter(
		const std::string& name,
		ShaderParamType type,
		UINT descriptorNums,
		UINT baseRegister,
		UINT registerSpace) :
		name(name),
		type(type),
		descriptorNums(descriptorNums),
		baseRegister(baseRegister),
		registerSpace(registerSpace) {}
};

class DX12Shader :
    public Shader
{
public:
	DX12Shader() {}
	DX12Shader(const std::string ShaderName, const std::wstring FilePath, std::string VS = "VS", std::string PS = "PS");

	void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& RootSignature) { RootSignature = RootSignature; }
	void SetVS(Microsoft::WRL::ComPtr<ID3DBlob>& VS) { mVS = VS; }
	void SetPS(Microsoft::WRL::ComPtr<ID3DBlob>& PS) { mPS = PS; }
	void SetRasterizerState(D3D12_RASTERIZER_DESC RasterizerState) { PRasterizerState = RasterizerState; }
	void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC DepthStencilState) { PDepthStencilState = DepthStencilState; }
	void SetBlendState(D3D12_BLEND_DESC BlendState) { PBlendState = BlendState; }

	void SetPSODesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc);
	std::unordered_map<UINT, UINT> GetParamMap() { return ParamMap; }
	std::vector<ShaderParameter> GetParams() { return Params; }

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return RootSignature; }
protected:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> mVS;
	Microsoft::WRL::ComPtr<ID3DBlob> mPS;
	std::unordered_map<UINT, UINT> ParamMap;
	std::vector<ShaderParameter> Params;
	D3D12_RASTERIZER_DESC PRasterizerState;
	D3D12_DEPTH_STENCIL_DESC PDepthStencilState;
	D3D12_BLEND_DESC PBlendState;
};

