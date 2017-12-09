#include "stdafx.h"
#include "ShaderManager.h"


ShaderManager::ShaderManager()
{
	m_TextureShader = 0;
	m_LightShader = 0;
	m_ClipPlaneShader = 0;
	m_FontShader = 0;
}

ShaderManager::ShaderManager(const ShaderManager& other)
{

}

bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Create the color shader object.
	m_ColorShader = new ColorShader();
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShader();
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShader();
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_ClipPlaneShader = new ClipPlaneShader();
	if (!m_ClipPlaneShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_ClipPlaneShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the clip plane shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_FontShader = new FontShader();
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


bool ShaderManager::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix)
{
	bool result;

	// Render the model using the color shader.
	result = m_ColorShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;


	// Render the model using the texture shader.
	result = m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuseColor,
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
	bool result;


	// Render the model using the light shader.
	result = m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambient, diffuseColor, cameraPosition,
		specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderClipPlaneShader(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 plane)
{
	bool result;


	// Render the model using the clip plane shader.
	result = m_ClipPlaneShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, plane);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color)
{
	bool result;


	// Render the model using the font shader.
	result = m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
	if (!result)
	{
		return false;
	}

	return true;
}



void ShaderManager::Shutdown()
{
	// Release the bump map shader object.
	if (m_ClipPlaneShader)
	{
		m_ClipPlaneShader->Shutdown();
		delete m_ClipPlaneShader;
		m_ClipPlaneShader = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the texture shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the texture shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}
	return;
}
ShaderManager::~ShaderManager()
{

}
