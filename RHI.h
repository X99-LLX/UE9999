#pragma once

#include "Scene.h"
#include "GameTimer.h"

class RHI 
{
public:
	virtual ~RHI();

	virtual bool Init() = 0;
	virtual void Update(Scene* mScene, Primitive* actor) = 0;
	virtual void DrawCall(Primitive* actor) = 0;
	virtual void BuildGeo(Primitive* actor) = 0;
	virtual void CreateRenderItem(Primitive* actor) = 0;
	
	virtual void ResetViewportsAndScissorRects() = 0;
	virtual void ClearRTVAndDSV() = 0;
	virtual void SetRTVAndDSV() = 0;
	virtual void SetRootSignature() = 0;
	virtual void OpenRtv() = 0;
	virtual void CloseRtv() = 0;
	virtual void Swapchain() = 0;
	virtual void DrawallShadow(Primitive* actor) = 0;
	virtual void UpdateShadowPassCB(const GameTimer& gt) = 0;

	virtual void OpenShadowMapDsv() = 0;
	virtual void CloseShadowMapDsv() = 0;
	virtual void InitDrawShadow() = 0;
};

