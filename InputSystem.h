#pragma once
class InputSystem
{
public:
	void KeyDown(UINT8 KeyCode);
	void KeyUp(UINT8 KeyCode);

	bool IsKeyDown(UINT8 KeyCode);
	
	void MouseDown(WPARAM btnState, int x, int y, HWND hwnd);
	void MouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	bool Keys[256];
	POINT mLastMousePos;
};

