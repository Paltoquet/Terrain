#include "stdafx.h"
#include <UI/UserInterface.h>


UserInterface::UserInterface()
{
	m_Font1 = 0;
	m_FpsString = 0;
}


UserInterface::UserInterface(const UserInterface& other)
{
}


UserInterface::~UserInterface()
{
}


bool UserInterface::Initialize(D3DClass* Direct3D, int screenHeight, int screenWidth)
{
	bool result;
	char videoCard[128];
	int videoMemory;
	char videoString[144];
	char memoryString[32];
	char tempString[16];
	int i;


	// Create the first font object.
	m_Font1 = new Font();
	if (!m_Font1)
	{
		return false;
	}

	// Initialize the first font object.
	result = m_Font1->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "Ressources/Font/fontdata.txt", 
								 L"Ressources/Font/font.DDS", 32.0f, 3);
	if (!result)
	{
		return false;
	}

	// Create the text object for the fps string.
	m_FpsString = new Text();
	if (!m_FpsString)
	{
		return false;
	}

	// Initialize the fps text string.
	result = m_FpsString->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1, 
									 "Fps: 0", 10, 50, 0.0f, 1.0f, 0.0f);
	if (!result)
	{
		return false;
	}

	// Initial the previous frame fps.
	m_previousFps = -1;

	// Setup the video card strings.
	Direct3D->GetVideoCardInfo(videoCard, videoMemory);
	strcpy_s(videoString, "Video Card: ");
	strcat_s(videoString, videoCard);

	_itoa_s(videoMemory, tempString, 10);

	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	return true;
}


void UserInterface::Shutdown()
{
	// Release the fps text string.
	if(m_FpsString)
	{
		m_FpsString->Shutdown();
		delete m_FpsString;
		m_FpsString = 0;
	}

	// Release the font object.
	if(m_Font1)
	{
		m_Font1->Shutdown();
		delete m_Font1;
		m_Font1 = 0;
	}

	return;
}


bool UserInterface::Frame(ID3D11DeviceContext* deviceContext, int fps, float posX, float posY, float posZ, 
							   float rotX, float rotY, float rotZ)
{
	bool result;


	// Update the fps string.
	result = UpdateFpsString(deviceContext, fps);
	if(!result)
	{
		return false;
	}

	return true;
}


bool UserInterface::Render(D3DClass* Direct3D, ShaderManager* ShaderManager, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, 
								const XMMATRIX& orthoMatrix)
{
	int i;


	// Turn off the Z buffer and enable alpha blending to begin 2D rendering.
	Direct3D->TurnZBufferOff();
	Direct3D->TurnOnAlphaBlending();

	// Render the fps string.
	m_FpsString->Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());

	// Turn off alpha blending now that the text has been rendered.
	Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that the 2D rendering has completed.
	Direct3D->TurnZBufferOn();

	return true;
}


bool UserInterface::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
{
	char tempString[16];
	char finalString[16];
	float red, green, blue;
	bool result;


	// Check if the fps from the previous frame was the same, if so don't need to update the text string.
	if(m_previousFps == fps)
	{
		return true;
	}

	// Store the fps for checking next frame.
	m_previousFps = fps;

	// Truncate the fps to below 100,000.
	if(fps > 99999)
	{
		fps = 99999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	// If fps is 60 or above set the fps color to green.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 60 set the fps color to yellow.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	result = m_FpsString->UpdateSentence(deviceContext, m_Font1, finalString, 10, 50, red, green, blue);
	if(!result)
	{
		return false;
	}

	return true;
}