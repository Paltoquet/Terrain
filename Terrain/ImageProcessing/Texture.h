#pragma once

#include <d3d11.h>
#include <stdio.h>

class Texture
{
public:

	Texture();
	Texture(const Texture&);
	~Texture();

public:
	enum TextureType {
		TGA,
		DDS
	};

private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;

public:

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool InitializeDDS(ID3D11Device* device, WCHAR* filename);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

};

