#pragma once
#include "BaseApp.h"

class Win32App :
    public BaseApp
{
public:
    virtual BaseWindow* CreateWindow() override;
    virtual void Initialize() override;

};

