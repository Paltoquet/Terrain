#pragma once

#include <stdlib.h>
#include <time.h>

#include <directxmath.h>
using namespace DirectX;

class SceneGraph
{
private:
	struct ModelInfoType
	{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};

public:
	SceneGraph();
	SceneGraph(const SceneGraph&);
	~SceneGraph();

	bool Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMFLOAT4&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;
};
