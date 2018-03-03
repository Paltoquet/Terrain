#include "stdafx.h"
#include <Application.h>


Application::Application()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_ShaderManager = 0;
	m_TextureManager = 0;
	m_Fps = 0;
	m_Timer = 0;
	m_World = 0;
}

Application::Application(const Application& other)
{

}



bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	// Create the input object.
	m_Input = new Input();
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass();
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the shader manager object.
	m_ShaderManager = new ShaderManager();
	if (!m_ShaderManager)
	{
		return false;
	}

	// Initialize the shader manager object.
	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture manager object.
	m_TextureManager = new TextureManager();
	if (!m_TextureManager)
	{
		return false;
	}

	// Initialize the texture manager object.
	result = m_TextureManager->Initialize(10);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture manager object.", L"Error", MB_OK);
		return false;
	}

	// Load textures into the texture manager.
	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "Ressources/Texture/dirt01d.tga", 0);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "Ressources/Texture/dirt01n.tga", 1);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "Ressources/Texture/snow01n.tga", 2);
	if (!result)
	{
		return false;
	}

	result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "Ressources/Texture/distance01n.tga", 3);
	if (!result)
	{
		return false;
	}

	// Create the timer object.
	m_Timer = new Timer();
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the fps object.
	m_Fps = new FpsCounter();
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the zone object.
	m_World = new World();
	if (!m_World)
	{
		return false;
	}

	// Initialize the zone object.
	result = m_World->Initialize(m_Direct3D, hwnd, m_ShaderManager, screenWidth, screenHeight, SCREEN_DEPTH);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the zone object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Application::Frame()
{
	bool result;

	// Update the system stats.
	m_Fps->Frame();
	m_Timer->Frame();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Do the zone frame processing.
	result = m_World->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), m_Fps->GetFps());
	if (!result)
	{
		return false;
	}

	return result;
}


void Application::Shutdown()
{
	// Release the zone object.
	if (m_World)
	{
		m_World->Shutdown();
		delete m_World;
		m_World = 0;
	}

	// Release the shader manager object.
	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	// Release the texture manager object.
	if (m_TextureManager)
	{
		m_TextureManager->Shutdown();
		delete m_TextureManager;
		m_TextureManager = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

Application::~Application()
{
}
