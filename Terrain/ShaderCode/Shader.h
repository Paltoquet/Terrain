#ifndef _SHADERCLASS_H_
#define _SHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class Shader
{
	public:
		Shader();
		Shader(const Shader&);
		~Shader();

		virtual bool Initialize(ID3D11Device*, HWND);
		virtual void Shutdown();
		virtual bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

	protected:

		struct MatrixBufferType
		{
			XMMATRIX world;
			XMMATRIX view;
			XMMATRIX projection;
		};

		virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
		virtual void ShutdownShader();
		virtual void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

		virtual bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
		virtual void RenderShader(ID3D11DeviceContext*, int);

	protected:

		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_matrixBuffer;
};

#endif