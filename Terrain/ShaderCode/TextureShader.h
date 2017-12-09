#pragma once

#include "shader.h"

class TextureShader: public Shader
{
public:
	TextureShader();
	TextureShader(const TextureShader&);
	~TextureShader();

protected:
	ID3D11SamplerState* m_sampleState;

public:
	bool Initialize(ID3D11Device*, HWND) override;
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void ShutdownShader() override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int) override;

};

