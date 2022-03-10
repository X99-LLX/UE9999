#include "stdafx.h"
#include "Window.h"

Window::~Window()
{

}

void Window::SetWidgthAndHeigh(int w, int h)
{

}

bool Window::CreateMainWindow()
{
	return true;
}

bool Window::InitWnd()
{
	return true;
}

float Window::GetAspectRatio()
{
	return (float)Width / Height;
}

InputSystem* Window::GetInput()
{
	return &mInput;
}
