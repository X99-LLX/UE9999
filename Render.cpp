#include "stdafx.h"
#include "Render.h"
#include "DX12RHI.h"

Render::Render()
{
	mRHI = CreateRHI();
}

Render::~Render()
{
	delete mRHI;
}

RHI* Render::GetRHI()
{
	return mRHI;
}

void Render::BuildRes(std::vector<std::shared_ptr<RenderItem>> RI)
{
	mRHI->CreateRenderItem(RI);
}

void Render::run()
{
	mRHI->Update();
	mRHI->Draw();
}

RHI* Render::CreateRHI()
{
#ifdef _RHI_DX12
	return new DX12RHI();
#endif

#ifdef _RHI_OPENGL
	return new OPGLRHI();
#endif
}
