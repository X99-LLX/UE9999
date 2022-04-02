#include "stdafx.h"
#include "DX12Pipeline.h"

DX12Pipeline::DX12Pipeline(Shader* Shader) : Pipeline(Shader)
{

}

DX12Pipeline::~DX12Pipeline()
{
	PipelineState = nullptr;
}
