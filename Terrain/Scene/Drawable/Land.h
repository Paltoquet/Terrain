#pragma once

#include <fstream>
#include <stdio.h>

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;
using namespace std;

class Land
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct HeightMapType
	{
		float x, y, z;
	};

	struct ModelType
	{
		float x, y, z;
	};


public:
	Land();
	Land(const Land&);
	~Land();

	bool Initialize(ID3D11Device*, char* configFile);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);

	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	bool BuildTerrainModel();

	void RenderBuffers(ID3D11DeviceContext*);

	void ShutdownTerrainModel();
	void ShutdownBuffers();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;

};
