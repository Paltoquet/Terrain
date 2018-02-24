#ifndef _USERINTERFACE_H_
#define _USERINTERFACE_H_

#include <D3dClass.h>
#include <ShaderCode\ShaderManager.h>
#include <Scene\Drawable\MiniMap.h>

#include <UI\Text.h>

#include <directxmath.h>
#include <d3d11.h>
using namespace DirectX;

class UserInterface
{
public:
	UserInterface();
	UserInterface(const UserInterface&);
	~UserInterface();

	bool Initialize(D3DClass*, int, int);
	void Shutdown();

	bool Frame(ID3D11DeviceContext*, int, float, float, float, float, float, float);
	bool Render(D3DClass* Direct3D, ShaderManager* ShaderManager, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& orthoMatrix);
	bool UpdateRenderCounts(ID3D11DeviceContext*, int, int, int);

private:
	bool UpdateFpsString(ID3D11DeviceContext*, int);

private:
	//TODO pass font pointer to Text
	Font* m_Font1;
	Text *m_FpsString;
	Text* m_RenderCountStrings;
	MiniMap* m_MiniMap;
	int m_previousFps;
};

#endif