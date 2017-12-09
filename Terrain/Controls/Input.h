#ifndef _INPUT_H_
#define _INPUT_H_


#define DIRECTINPUT_VERSION 0x0800


#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class Input
{
public:
	Input();
	Input(const Input&);
	~Input();
	
	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpwardPressed();
	bool IsDownwardPressed();
	bool IsAPressed();
	bool IsEPressed();
	bool IsUpPressed();
	bool IsDownPressed();

	bool IsF1Toggled();
	bool IsF2Toggled();

	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	bool m_F1_released;
	bool m_F2_released;
};

#endif

