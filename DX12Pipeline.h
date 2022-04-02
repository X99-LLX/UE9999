#pragma once
#include "Pipeline.h"
class DX12Pipeline :
    public Pipeline
{
public:
	DX12Pipeline() {}
	DX12Pipeline(Shader* Shader);
	virtual ~DX12Pipeline();
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO) { PipelineState = PSO; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() { return PipelineState; }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
};

