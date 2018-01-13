#include "stdafx.h"

#include <Scene\Drawable\IModel.h>

void IModel::InitializeSDKMeshes(ID3D11Device* d3dDevice, const wchar_t* fileName, IEffectFactory* effectFactory, bool ccw, bool pmalpha)
{
	m_model = std::move(Model::CreateFromSDKMESH(d3dDevice, fileName, *effectFactory, ccw, pmalpha));
}

