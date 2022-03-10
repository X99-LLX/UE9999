#pragma once
class InputSystem
{
public:
	void MouseDown();
	void MouseUp();
	void OnMouseMove();
	void Update();
private:
	POINT mLastMousePos;
};

