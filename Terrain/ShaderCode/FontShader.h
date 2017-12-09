#pragma once

#include "shader.h"
#include <d3d11.h>
#include <directxmath.h>

class FontShader : public Shader
{

private:
	struct ConstantBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};

public:
	FontShader();
	FontShader(const FontShader&);
	~FontShader();

public:
	bool Initialize(ID3D11Device*, HWND) override;
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void ShutdownShader() override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int) override;

protected:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_constantBuffer;
	ID3D11Buffer* m_pixelBuffer;
};

