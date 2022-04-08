#pragma once
#include "Pipeline.h"
class DX12Pipeline :
    public Pipeline
{
public:
	DX12Pipeline() {}
	DX12Pipeline(Pipeline* p) : Pipeline(p) {}
	virtual ~DX12Pipeline();

	void SetPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO) { PipelineState = PSO; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() { return PipelineState; }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
};

