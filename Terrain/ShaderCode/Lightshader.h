#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "shader.h"

using namespace DirectX;
using namespace std;


class LightShader : public Shader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColor;
	};

public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device*, HWND) override;
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4 ambient,
		XMFLOAT4 diffuse, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);

	/*/bool Render(ID3D11DeviceContext*, int indexCount, int instanceCount, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4 ambient,
		XMFLOAT4 diffuse, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);*/

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void ShutdownShader() override;

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4 ambient, XMFLOAT4 diffuse,
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext*, int) override;
	//void RenderShader(ID3D11DeviceContext*, int indexCount, int instanceCount);

protected:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};
