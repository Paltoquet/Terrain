#include "stdafx.h"
#include <Scene/Drawable/Land.h>

Land::Land()
{
	m_terrainFilename = 0;
	m_colorMapFilename = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
	m_TerrainCells = 0;
}


Land::Land(const Land& other)
{
}

bool Land::Initialize(ID3D11Device* device, char* configFile)
{
	bool result;

	// Get the terrain filename, dimensions, and so forth from the setup file.
	result = LoadSetupFile(configFile);
	if (!result)
	{
		return false;
	}

	/*
	// Initialize the terrain height map with the data from the bitmap file.
	result = LoadBitmapHeightMap();
	if (!result)
	{
		return false;
	}
	*/	

	// Initialize the terrain height map with the data from the raw file.
	result = LoadRawHeightMap();
	if (!result)
	{
		return false;
	}

	// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Load in the color map for the terrain.
	result = LoadColorMap();
	if (!result)
	{
		return false;
	}

	// Now build the 3D model of the terrain.
	result = BuildTerrainModel();
	if (!result)
	{
		return false;
	}

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	ShutdownHeightMap();

	// Calculate the tangent and binormal for the terrain model.
	CalculateTerrainVectors();

	// Load the rendering buffers with the terrain data.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Release the terrain model now that the rendering buffers have been loaded.
	ShutdownTerrainModel();

	return true;
}

bool Land::LoadSetupFile(char* filename)
{
	int stringLength;
	ifstream fin;
	char input;

	// Initialize the string that will hold the terrain file name.
	stringLength = 256;
	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename)
	{
		return false;
	}

	m_colorMapFilename = new char[stringLength];
	if (!m_colorMapFilename)
	{
		return false;
	}

	// Open the setup file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read up to the terrain file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain file name.
	fin >> m_terrainFilename;

	// Read up to the value of terrain height.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height.
	fin >> m_terrainHeight;

	// Read up to the value of terrain width.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain width.
	fin >> m_terrainWidth;

	// Read up to the value of terrain height scaling.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the terrain height scaling.
	fin >> m_heightScale;

	// Read up to the color map file name.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the color map file name.
	fin >> m_colorMapFilename;

	// Close the setup file.
	fin.close();

	return true;
}

bool Land::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;


	// Start by creating the array structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMap[index].y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the terrain filename now that is has been read in.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}


bool Land::LoadRawHeightMap()
{
	int error, i, j, index;
	FILE* filePtr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;


	// Create the float array to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Open the 16 bit raw height map file for reading in binary.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Calculate the size of the raw image data.
	imageSize = m_terrainHeight * m_terrainWidth;

	// Allocate memory for the raw image data.
	rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		return false;
	}

	// Read in the raw image data.
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Copy the image data into the height map array.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Store the height at this point in the height map array.
			m_heightMap[index].y = (float)rawImage[index];
		}
	}

	// Release the bitmap image data.
	delete[] rawImage;
	rawImage = 0;

	// Release the terrain filename now that it has been read in.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}

void Land::SetTerrainCoordinates()
{
	int i, j, index;


	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// Scale the height.
			m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
}

bool Land::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	XMFLOAT3 v1, v2, v3, sum;
	XMVECTOR vect1, vect2, vect3, r1, r2;
	XMFLOAT3* normals;


	// Create a temporary array to hold the face normal vectors.
	normals = new XMFLOAT3[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;		// Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;				// Upper left vertex.

			v1 = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			v2 = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			v3 = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vect1 = XMLoadFloat3(&v1);
			vect2 = XMLoadFloat3(&v2);
			vect3 = XMLoadFloat3(&v3);

			// Calculate the two vectors for this face.
			r1 = XMVectorSubtract(vect1, vect3);
			r2 = XMVectorSubtract(vect3, vect2);

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			vect1 = XMVector3Cross(r1, r2);
			// Normalize the final value for this face using the length.
			vect1 = XMVector3Normalize(vect1);
			index = (j * (m_terrainWidth - 1)) + i;
			XMStoreFloat3(&(normals[index]), vect1);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			sum		= XMFLOAT3(0.0f, 0.0f, 0.0f);
			vect1	= XMLoadFloat3(&sum);

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				vect2 = XMLoadFloat3(&(normals[index]));
				vect1 = XMVectorAdd(vect1, vect2);
			}

			// Bottom right face.
			if ((i<(m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				vect2 = XMLoadFloat3(&(normals[index]));
				vect1 = XMVectorAdd(vect1, vect2);
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				vect2 = XMLoadFloat3(&(normals[index]));
				vect1 = XMVectorAdd(vect1, vect2);
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				vect2 = XMLoadFloat3(&(normals[index]));
				vect1 = XMVectorAdd(vect1, vect2);
			}

			// Normalize the vector
			vect1 = XMVector3Normalize(vect1);
			XMStoreFloat3(&sum, vect1);

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = sum.x;
			m_heightMap[index].ny = sum.y;
			m_heightMap[index].nz = sum.z;
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

bool Land::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the color map file in binary.
	error = fopen_s(&filePtr, m_colorMapFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the color map filename now that is has been read in.
	delete[] m_colorMapFilename;
	m_colorMapFilename = 0;

	return true;
}

void Land::CalculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.pos		= XMFLOAT3(m_terrainModel[index].x, m_terrainModel[index].y, m_terrainModel[index].z);
		vertex1.uv		= XMFLOAT2(m_terrainModel[index].tu, m_terrainModel[index].tv);
		vertex1.normal	= XMFLOAT3(m_terrainModel[index].nx, m_terrainModel[index].ny, m_terrainModel[index].nz);
		index++;

		vertex2.pos		= XMFLOAT3(m_terrainModel[index].x, m_terrainModel[index].y, m_terrainModel[index].z);
		vertex2.uv		= XMFLOAT2(m_terrainModel[index].tu, m_terrainModel[index].tv);
		vertex2.normal	= XMFLOAT3(m_terrainModel[index].nx, m_terrainModel[index].ny, m_terrainModel[index].nz);
		index++;

		vertex3.pos		= XMFLOAT3(m_terrainModel[index].x, m_terrainModel[index].y, m_terrainModel[index].z);
		vertex3.uv		= XMFLOAT2(m_terrainModel[index].tu, m_terrainModel[index].tv);
		vertex3.normal	= XMFLOAT3(m_terrainModel[index].nx, m_terrainModel[index].ny, m_terrainModel[index].nz);
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_terrainModel[index - 1].tx = tangent.x;
		m_terrainModel[index - 1].ty = tangent.y;
		m_terrainModel[index - 1].tz = tangent.z;
		m_terrainModel[index - 1].bx = binormal.x;
		m_terrainModel[index - 1].by = binormal.y;
		m_terrainModel[index - 1].bz = binormal.z;

		m_terrainModel[index - 2].tx = tangent.x;
		m_terrainModel[index - 2].ty = tangent.y;
		m_terrainModel[index - 2].tz = tangent.z;
		m_terrainModel[index - 2].bx = binormal.x;
		m_terrainModel[index - 2].by = binormal.y;
		m_terrainModel[index - 2].bz = binormal.z;

		m_terrainModel[index - 3].tx = tangent.x;
		m_terrainModel[index - 3].ty = tangent.y;
		m_terrainModel[index - 3].tz = tangent.z;
		m_terrainModel[index - 3].bx = binormal.x;
		m_terrainModel[index - 3].by = binormal.y;
		m_terrainModel[index - 3].bz = binormal.z;
	}

	return;
}

void Land::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tuVector, tvVector;
	XMVECTOR tmp1, tmp2, tmp3;
	float den;
	float length;


	tmp1 = XMLoadFloat3(&vertex1.pos);
	tmp2 = XMLoadFloat3(&vertex2.pos);
	tmp3 = XMLoadFloat3(&vertex3.pos);

	// Calculate the two vectors for this face.
	tmp2 = XMVectorSubtract(tmp2, tmp1);
	tmp3 = XMVectorSubtract(tmp3, tmp1);

	XMStoreFloat3(&vector1, tmp2);
	XMStoreFloat3(&vector2, tmp3);

	// Calculate the tu and tv texture space vectors.
	tmp1 = XMLoadFloat2(&vertex1.uv);
	tmp2 = XMLoadFloat2(&vertex2.uv);
	tmp3 = XMLoadFloat2(&vertex3.uv);

	tmp2 = XMVectorSubtract(tmp2, tmp1);
	tmp3 = XMVectorSubtract(tmp3, tmp1);

	XMStoreFloat2(&tuVector, tmp2);
	XMStoreFloat2(&tvVector, tmp3);


	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

	tmp1 = XMLoadFloat3(&vector1);
	tmp1 = XMVectorScale(tmp1, tvVector.y);

	tmp2 = XMLoadFloat3(&vector2);
	tmp2 = XMVectorScale(tmp2, tvVector.x);

	tmp3 = XMVectorSubtract(tmp1, tmp2);
	tmp3 = XMVectorScale(tmp3, den);

	tmp3 = XMVector3Normalize(tmp3);
	XMStoreFloat3(&tangent, tmp3);

	tmp1 = XMLoadFloat3(&vector1);
	tmp1 = XMVectorScale(tmp1, tuVector.y);

	tmp2 = XMLoadFloat3(&vector2);
	tmp2 = XMVectorScale(tmp2, tuVector.x);

	tmp3 = XMVectorSubtract(tmp1, tmp2);
	tmp3 = XMVectorScale(tmp3, den);

	tmp3 = XMVector3Normalize(tmp3);
	XMStoreFloat3(&binormal, tmp3);

	return;
}

bool Land::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel)
	{
		return false;
	}

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
			index++;
		}
	}

	return true;
}

bool Land::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

void Land::Frame()
{
	m_renderCount = 0;
	m_cellsDrawn = 0;
	m_cellsCulled = 0;
	return;
}

bool Land::GetHeightAtPosition(float inputX, float inputZ, float& height)
{
	int i, cellId, index;
	float vertex1[3], vertex2[3], vertex3[3];
	bool foundHeight;
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;


	// Loop through all of the terrain cells to find out which one the inputX and inputZ would be inside.
	cellId = -1;
	for (i = 0; i<m_cellCount; i++)
	{
		// Get the current cell dimensions.
		m_TerrainCells[i].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

		// Check to see if the positions are in this cell.
		if ((inputX < maxWidth) && (inputX > minWidth) && (inputZ < maxDepth) && (inputZ > minDepth))
		{
			cellId = i;
			break;
		}
	}

	// If we didn't find a cell then the input position is off the terrain grid.
	if (cellId == -1)
	{
		return false;
	}

	// If this is the right cell then check all the triangles in this cell to see what the height of the triangle at this position is.
	for (i = 0; i<(m_TerrainCells[cellId].GetVertexCount() / 3); i++)
	{
		index = i * 3;

		vertex1[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex1[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex1[2] = m_TerrainCells[cellId].m_vertexList[index].z;
		index++;

		vertex2[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex2[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex2[2] = m_TerrainCells[cellId].m_vertexList[index].z;
		index++;

		vertex3[0] = m_TerrainCells[cellId].m_vertexList[index].x;
		vertex3[1] = m_TerrainCells[cellId].m_vertexList[index].y;
		vertex3[2] = m_TerrainCells[cellId].m_vertexList[index].z;

		// Check to see if this is the polygon we are looking for.
		foundHeight = CheckHeightOfTriangle(inputX, inputZ, height, vertex1, vertex2, vertex3);
		if (foundHeight)
		{
			return true;
		}
	}

	return false;
}

bool Land::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
	//float directionVector[3], edge1[3], edge2[3], normal[3];
	//float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	//float magnitude, D, denominator, numerator, t, determinant;

	XMFLOAT3 tmp;
	XMFLOAT3 vert0, vert1, vert2;
	XMFLOAT3 startVector;
	XMFLOAT3 lo, v0v1, v0v2, v1v2, v2v0, rayDir, faceNormal, faceTangent;
	XMFLOAT3 P;
	XMVECTOR tmp1, tmp2, tangent, res;
	float numerator, denominator, distance;
	float t;


	// Starting position of the ray that is being cast.
	//startVector[0] = x;
	//startVector[1] = 0.0f;
	//startVector[2] = z;

	vert0 = XMFLOAT3(v0[0], v0[1], v0[2]);
	vert1 = XMFLOAT3(v1[0], v1[1], v1[2]);
	vert2 = XMFLOAT3(v2[0], v2[1], v2[2]);
	lo = XMFLOAT3(x, 0.0f, z);
	rayDir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	
	tmp1 = XMLoadFloat3(&vert0);
	tmp2 = XMLoadFloat3(&vert1);
	res = XMVectorSubtract(tmp2, tmp1);
	XMStoreFloat3(&v0v1, res);

	tmp2 = XMLoadFloat3(&vert2);
	res = XMVectorSubtract(tmp2, tmp1);
	XMStoreFloat3(&v0v2, res);
	res = XMVectorScale(res, -1.0f),
	XMStoreFloat3(&v2v0, res);

	tmp1 = XMLoadFloat3(&vert1);
	res = XMVectorSubtract(tmp2, tmp1);
	XMStoreFloat3(&v1v2, res);

	tmp1 = XMLoadFloat3(&v0v1);
	tmp2 = XMLoadFloat3(&v0v2);
	res = XMVector3Cross(tmp1, tmp2);
	res = XMVector3Normalize(res);
	XMStoreFloat3(&faceNormal, res);

	tmp1 = XMLoadFloat3(&rayDir);
	tmp1 = XMVector3Dot(tmp1, res);
	XMStoreFloat3(&tmp, tmp1);
	denominator = tmp.x;

	tmp1 = XMLoadFloat3(&vert0);
	res = XMVectorScale(res, -1.0f);
	tmp2 = XMVector3Dot(tmp1, res);
	XMStoreFloat3(&tmp, tmp2);
	numerator = tmp.x;

	tmp1 = XMLoadFloat3(&lo);
	tmp2 = XMLoadFloat3(&faceNormal);
	tmp1 = XMVector3Dot(tmp1, tmp2);
	XMStoreFloat3(&tmp, tmp1);
	numerator = -1.0f * (numerator + tmp.x);

	t = numerator / denominator;

	P = XMFLOAT3(lo.x + rayDir.x * t, lo.y + rayDir.y * t, lo.z + rayDir.z * t);


	//check edge vov1
	tmp1 = XMLoadFloat3(&v0v1);
	tmp2 = XMLoadFloat3(&faceNormal);
	tangent = XMVector3Cross(tmp1, tmp2);

	tmp1 = XMLoadFloat3(&P);
	tmp2 = XMLoadFloat3(&vert0);
	res  = XMVectorSubtract(tmp1, tmp2);
	res = XMVector3Dot(tangent, res);
	XMStoreFloat3(&tmp, res);
	distance = tmp.x;

	if (distance > 0.001f) {
		return false;
	}

	//check edge v1v2
	tmp1 = XMLoadFloat3(&v1v2);
	tmp2 = XMLoadFloat3(&faceNormal);
	tangent = XMVector3Cross(tmp1, tmp2);

	tmp1 = XMLoadFloat3(&P);
	tmp2 = XMLoadFloat3(&vert1);
	res = XMVectorSubtract(tmp1, tmp2);
	res = XMVector3Dot(tangent, res);
	XMStoreFloat3(&tmp, res);
	distance = tmp.x;

	if (distance > 0.001f) {
		return false;
	}

	//check v2v0
	tmp1 = XMLoadFloat3(&v2v0);
	tmp2 = XMLoadFloat3(&faceNormal);
	tangent = XMVector3Cross(tmp1, tmp2);

	tmp1 = XMLoadFloat3(&P);
	tmp2 = XMLoadFloat3(&vert2);
	res = XMVectorSubtract(tmp1, tmp2);
	res = XMVector3Dot(tangent, res);
	XMStoreFloat3(&tmp, res);
	distance = tmp.x;

	if (distance > 0.001f) {
		return false;
	}

	height = P.y;

	return true;

	/*
	// The direction the ray is being cast.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// Calculate the two edges from the three points given.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// Calculate the normal of the triangle from the two edges.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// Find the distance from the origin to the plane.
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// Get the denominator of the equation.
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// Make sure the result doesn't get too close to zero to prevent divide by zero.
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// Get the numerator of the equation.
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// Calculate where we intersect the triangle.
	t = numerator / denominator;

	// Find the intersection vector.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// Find the three edges of the triangle.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// Calculate the normal for the first edge.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the second edge.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the third edge.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// Check if it is outside.
	if (determinant > 0.001f)
	{
		return false;
	}

	// Now we have our height.
	height = Q[1];

	return true;
	*/
}

bool Land::InitializeBuffers(ID3D11Device* device)
{
	return LoadTerrainCells(device);
}

bool Land::LoadTerrainCells(ID3D11Device* device)
{
	int cellHeight, cellWidth, cellRowCount, i, j, index;
	bool result;


	// Set the height and width of each terrain cell to a fixed 33x33 vertex array.
	cellHeight = 33;
	cellWidth = 33;

	// Calculate the number of cells needed to store the terrain data.
	cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);
	m_cellCount = cellRowCount * cellRowCount;

	// Create the terrain cell array.
	m_TerrainCells = new TerrainCell[m_cellCount];
	if (!m_TerrainCells)
	{
		return false;
	}

	// Loop through and initialize all the terrain cells.
	for (j = 0; j<cellRowCount; j++)
	{
		for (i = 0; i<cellRowCount; i++)
		{
			index = (cellRowCount * j) + i;

			result = m_TerrainCells[index].Initialize(device, m_terrainModel, i, j, cellHeight, cellWidth, m_terrainWidth);
			if (!result)
			{
				return false;
			}
		}
	}

	return true;
}

void Land::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	/*
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	*/

	return;
}

bool Land::RenderCell(ID3D11DeviceContext* deviceContext, int cellId, Frustum* Frustum)
{
	float maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth;
	bool result;


	// Get the dimensions of the terrain cell.
	m_TerrainCells[cellId].GetCellDimensions(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);

	// Check if the cell is visible.  If it is not visible then just return and don't render it.
	result = Frustum->CheckRectangle(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth);
	if (!result)
	{
		// Increment the number of cells that were culled.
		m_cellsCulled++;

		return false;
	}

	// If it is visible then render it.
	m_TerrainCells[cellId].Render(deviceContext);

	// Add the polygons in the cell to the render count.
	m_renderCount += (m_TerrainCells[cellId].GetVertexCount() / 3);

	// Increment the number of cells that were actually drawn.
	m_cellsDrawn++;

	return true;
}


void Land::RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId)
{
	m_TerrainCells[cellId].RenderLineBuffers(deviceContext);
	return;
}


int Land::GetCellIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetIndexCount();
}

int Land::GetCellLinesIndexCount(int cellId)
{
	return m_TerrainCells[cellId].GetLineBuffersIndexCount();
}

int Land::GetCellCount()
{
	return m_cellCount;
}

int Land::GetRenderCount()
{
	return m_renderCount;
}


int Land::GetCellsDrawn()
{
	return m_cellsDrawn;
}


int Land::GetCellsCulled()
{
	return m_cellsCulled;
}


void Land::Shutdown()
{
	// Release the terrain cells.
	ShutdownTerrainCells();

	// Release the terrain model.
	ShutdownTerrainModel();

	// Release the height map.
	ShutdownHeightMap();

	return;
}

void Land::ShutdownHeightMap()
{
	// Release the height map array.
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void Land::ShutdownTerrainModel()
{
	// Release the terrain model data.
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}

void Land::ShutdownTerrainCells()
{
	int i;


	// Release the terrain cell array.
	if (m_TerrainCells)
	{
		for (i = 0; i<m_cellCount; i++)
		{
			m_TerrainCells[i].Shutdown();
		}

		delete[] m_TerrainCells;
		m_TerrainCells = 0;
	}

	return;
}

Land::~Land()
{
}