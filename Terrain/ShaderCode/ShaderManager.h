#pragma once

#include "d3dclass.h"
#include "Effects.h"
#include "textureShader.h"
#include "lightshader.h"
#include "clipPlaneShader.h"
#include "LandShader.h"
#include "FontShader.h"
#include "ColorShader.h"
#include "SkyDomeShader.h"

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&);
	~ShaderManager();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	bool RenderColorShader(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&);

	bool RenderTextureShader(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*);

	bool RenderLightShader(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4 ambient,
		XMFLOAT4 diffuse, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);
	
	bool RenderTerrainShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4);

	bool RenderClipPlaneShader(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT4 plane);

	bool RenderFontShader(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT4 color);

	bool RenderSkyDomeShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4);


public: 
	EffectFactory* m_EffectFactory;

private:
	ColorShader* m_ColorShader;
	TextureShader* m_TextureShader;
	LightShader* m_LightShader;
	LandShader* m_LandShader;
	ClipPlaneShader* m_ClipPlaneShader;
	FontShader* m_FontShader;
	SkyDomeShader* m_SkyDomeShader;

};

