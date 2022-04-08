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
	DX12Shader(Shader* s) : Shader(s) {}
	~DX12Shader() {}

	void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& RootSignature) { mRootSignature = RootSignature; }
	void SetVS(Microsoft::WRL::ComPtr<ID3DBlob>& VS) { mVS = VS; }
	void SetPS(Microsoft::WRL::ComPtr<ID3DBlob>& PS) { mPS = PS; }

	Microsoft::WRL::ComPtr<ID3DBlob> GetVS() { return mVS; }
	Microsoft::WRL::ComPtr<ID3DBlob> GetPS() { return mPS; }
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return mRootSignature; }

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayOut;
protected:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> mVS;
	Microsoft::WRL::ComPtr<ID3DBlob> mPS;
};

