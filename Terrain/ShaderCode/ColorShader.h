#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_


#include "Shader.h"


#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;


class ColorShader : public Shader
{

public:
	ColorShader();
	ColorShader(const ColorShader&);
	~ColorShader();

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&);
	void RenderShader(ID3D11DeviceContext*, int);
};

#endif