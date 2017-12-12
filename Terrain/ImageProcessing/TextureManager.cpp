#include "stdafx.h"
#include <ImageProcessing\TextureManager.h>
#include <Utils\StringHelper.h>


TextureManager::TextureManager()
{
	m_TextureArray = 0;
}


TextureManager::TextureManager(const TextureManager& other)
{
}

bool TextureManager::Initialize(int count)
{
	m_textureCount = count;

	// Create the color texture object.
	m_TextureArray = new Texture[m_textureCount];
	if (!m_TextureArray)
	{
		return false;
	}

	return true;
}

bool TextureManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;


	// Initialize the color texture object.
	result = m_TextureArray[location].Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextureManager::LoadTextureDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location)
{
	bool result;


	// Initialize the color texture object.
	result = m_TextureArray[location].InitializeDDS(device, StringHelper::getWChar(filename));
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(int id)
{
	return m_TextureArray[id].GetTexture();
}

void TextureManager::Shutdown()
{
	int i;


	// Release the texture objects.
	if (m_TextureArray)
	{
		for (i = 0; i<m_textureCount; i++)
		{
			m_TextureArray[i].Shutdown();
		}
		delete[] m_TextureArray;
		m_TextureArray = 0;
	}

	return;
}

TextureManager::~TextureManager()
{
}
