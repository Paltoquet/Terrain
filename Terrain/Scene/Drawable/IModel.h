#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include "Model.h"

using namespace DirectX;
using namespace std;

class IModel
{
protected:
	std::unique_ptr<Model> m_model;

public:
	void InitializeSDKMeshes(ID3D11Device* d3dDevice, const wchar_t* fileName, IEffectFactory* effectFactory, bool ccw = false, bool pmalpha = false);
	virtual void Render(ID3D11DeviceContext*, const std::function<void()>& draw) = 0;
	virtual int GetIndexCount() const = 0;
};

