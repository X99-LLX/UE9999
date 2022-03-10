#pragma once
class InputSystem
{
public:
	void KeyDown(UINT8 KeyCode);
	void KeyUp(UINT8 KeyCode);

	bool IsKeyDown(UINT8 KeyCode);
	
	void MouseDown();
	void MouseUp();
	void OnMouseMove();
	void Update();
private:
	bool Keys[256];
	POINT mLastMousePos;
};

