#pragma once


#include "shader.h"

class ClipPlaneShader : public Shader
{

public:
	struct ClipPlaneBufferType
	{
		XMFLOAT4 clipPlane;
	};
public:
	ClipPlaneShader();
	ClipPlaneShader(const ClipPlaneShader&);
	~ClipPlaneShader();

protected:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_clipPlaneBuffer;

public:
	bool Initialize(ID3D11Device*, HWND) override;
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4 plane);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void ShutdownShader() override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4 plane);
	void RenderShader(ID3D11DeviceContext*, int) override;

};

