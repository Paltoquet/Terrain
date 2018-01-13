#pragma once

#include <functional>
#include <d3d11.h>
#include <directxmath.h>
#include <Scene\Drawable\IModel.h>
#include "CommonStates.h"

using namespace DirectX;
using namespace std;

class Mesh : public IModel
{
public:
	bool Initialize(HWND hwnd, ID3D11Device* d3dDevice, IEffectFactory* effectFactory, const wchar_t* filename);
	virtual void Render(ID3D11DeviceContext* deviceContext, const std::function<void()>& draw) override;
	virtual int GetIndexCount() const override;


private:
	std::unique_ptr<CommonStates> m_states;
};

