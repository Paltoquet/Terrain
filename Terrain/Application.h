#pragma once

#include <memory>
#include <d3dclass.h>
#include <Controls/Input.h>
#include <ShaderCode/ShaderManager.h>
#include <ImageProcessing/TextureManager.h>
#include <Scene/World.h>
#include <Profilling/FpsCounter.h>
#include <Profilling/Timer.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	Input* m_Input;
	D3DClass* m_Direct3D;
	ShaderManager* m_ShaderManager;
	TextureManager* m_TextureManager;
	FpsCounter* m_Fps;
	Timer* m_Timer;
	World* m_World;

};
