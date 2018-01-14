#pragma once

#include <fstream>
#include <stdio.h>

#include <d3d11.h>
#include <directxmath.h>

#include <Scene/Drawable/TerrainCell.h>

using namespace DirectX;
using namespace std;

class Land
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
	};

	struct TempVertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct VectorType
	{
		float x, y, z;
	};


public:
	Land();
	Land(const Land&);
	~Land();

	bool Initialize(ID3D11Device*, char* configFile);
	void Shutdown();

	bool Render(ID3D11DeviceContext* deviceContext);
	bool RenderCell(ID3D11DeviceContext*, int);
	void RenderCellLines(ID3D11DeviceContext*, int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

private:
	bool InitializeBuffers(ID3D11Device*);

	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	bool LoadRawHeightMap();
	bool LoadColorMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();

	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, XMFLOAT3&, XMFLOAT3&);
	bool CalculateNormals();
	bool BuildTerrainModel();

	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTerrainCells(ID3D11Device*);

	void ShutdownTerrainCells();
	void ShutdownTerrainModel();

private:
	//ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount;// , m_indexCount;

	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename, *m_colorMapFilename;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;

	TerrainCell* m_TerrainCells;
	int m_cellCount;
};
