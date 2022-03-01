#pragma once

#include "BaseWindow.h"


class BaseApp
{
protected:
	BaseApp();
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;
	virtual ~BaseApp();
	
public:
	virtual void Initialize() = 0;
	virtual BaseWindow* CreateWindow() = 0;

protected:
	virtual void OnResize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool	mAppPaused = false;  
	bool	mMinimized = false;  
	bool	mMaximized = false;  
	bool	mResizing = false;   

};

