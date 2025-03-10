////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"
#include <cmath>
#include <iostream>

using namespace std;


TerrainClass::TerrainClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	m_smoothMap = 0;
	m_terrainGeneratedToggle = false;
	grassTexture = 0;
	slopeTexture = 0;
	rockTexture = 0;

	for (int i = 0; i < 256; i++) p[256 + i] = p[i] = permutation[i];

}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::InitializeTerrain(ID3D11Device* device, int terrainWidth, int terrainHeight, WCHAR* grassTextureFilename, WCHAR* slopeTextureFilename,
	WCHAR* rockTextureFilename, WCHAR* snowTextureFilename)
{
	int index;
	float height = 0.0;
	bool result;

	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	// Create the structure to hold the smooth data.
	m_smoothMap = new HeightMapType[m_terrainWidth * m_terrainHeight];

	if (!m_heightMap)
	{
		return false;
	}

	if (!m_smoothMap)
	{
		return false;
	}

	// Initialise the data in the height map (flat).
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

		}
	}

	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the textures.
	result = LoadTextures(device, grassTextureFilename, slopeTextureFilename, rockTextureFilename, snowTextureFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename, WCHAR* grassTextureFilename, WCHAR* slopeTextureFilename,
	WCHAR* rockTextureFilename, WCHAR* snowTextureFilename)
{
	bool result;

	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if (!result)
	{
		return false;
	}

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the textures.
	result = LoadTextures(device, grassTextureFilename, slopeTextureFilename, rockTextureFilename, snowTextureFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}


void TerrainClass::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

bool TerrainClass::FlattenTerrain(ID3D11Device * device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown && (!m_terrainGeneratedToggle))
	{
		int index;

		//loop through the terrain and set the heights how we want. This is where we generate the terrain
		for (int j = 0; j < m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
				index = (m_terrainHeight * j) + i;

				m_heightMap[index].x = (float)i;
				//Sets height to 0 to reset the terrain
				m_heightMap[index].y = (float)(0.0f);
				m_heightMap[index].z = (float)j;
			}
		}


		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}


int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* TerrainClass::GetGrassTexture()
{
	return grassTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetSlopeTexture()
{
	return slopeTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetRockTexture()
{
	return rockTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetSnowTexture()
{
	return snowTexture->GetTexture();
}

bool TerrainClass::GenerateHeightMap(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown && (!m_terrainGeneratedToggle))
	{
		int index;
		float amplitude = ((rand() % 20) + 1);
		float frequency = 0.1234151534;

		//loop through the terrain and set the heights how we want. This is where we generate the terrain
		for (int j = 0; j < m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
				index = (m_terrainHeight * j) + i;

				m_heightMap[index].x = (float)i;
				//Sets height to 0 to reset the terrain
				m_heightMap[index].y = ((float)(PerlinNoise(i * frequency, 0.0, j * frequency))) * amplitude;
				m_heightMap[index].z = (float)j;
			}
		}


		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}

float TerrainClass::getXZHeight(float xpos, float zpos)
{
	int index = (m_terrainHeight * zpos) + xpos;

	return m_heightMap[index].y;
}

int TerrainClass::GetIndex(int x, int y)
{
	return (m_terrainHeight * y) + x;
}

bool TerrainClass::SmoothTerrain(ID3D11Device* device, bool keydown)
{
	int index;

	if (keydown)
	{
		for (int x = 0; x < m_terrainWidth; x++)
		{
			for (int y = 0; y < m_terrainHeight; y++)
			{
				index = (m_terrainHeight * y) + x;
				float adjacentSections = 0.0f;
				float sectionsTotal = 0.0f;

				if ((x - 1) > 0) // Check to left
				{
					sectionsTotal += m_heightMap[GetIndex(x - 1, y)].y;
					adjacentSections++;

					if ((y - 1) > 0) // Check up and to the left
					{
						sectionsTotal += m_heightMap[GetIndex(x - 1, y - 1)].y;
						adjacentSections++;
					}

					if ((y + 1) < m_terrainHeight) // Check down and to the left
					{
						sectionsTotal += m_heightMap[GetIndex(x - 1, y + 1)].y;
						adjacentSections++;
					}
				}

				if ((x + 1) < m_terrainWidth) // Check to right
				{
					sectionsTotal += m_heightMap[GetIndex(x + 1, y)].y;
					adjacentSections++;

					if ((y - 1) > 0) // Check up and to the right
					{
						sectionsTotal += m_heightMap[GetIndex(x + 1, y - 1)].y;
						adjacentSections++;
					}

					if ((y + 1) < m_terrainHeight) // Check down and to the right
					{
						sectionsTotal += m_heightMap[GetIndex(x + 1, y + 1)].y;
						adjacentSections++;
					}
				}

				if ((y - 1) > 0) // Check above
				{
					sectionsTotal += m_heightMap[GetIndex(x, y - 1)].y;
					adjacentSections++;
				}

				if ((y + 1) < m_terrainHeight) // Check below
				{
					sectionsTotal += m_heightMap[GetIndex(x, y + 1)].y;
					adjacentSections++;
				}

				float average = (m_heightMap[GetIndex(x, y)].y + (sectionsTotal / adjacentSections)) * 0.5f;

				m_smoothMap[GetIndex(x, y)].x = m_heightMap[GetIndex(x, y)].x;
				m_smoothMap[GetIndex(x, y)].y = average;
				m_smoothMap[GetIndex(x, y)].z = m_heightMap[GetIndex(x, y)].z;
			}
		}

		// Overwrite the HeightData info with our new smoothed info
		for (int x = 0; x < m_terrainWidth; x++)
		{
			for (int y = 0; y < m_terrainHeight; y++)
			{
				m_heightMap[GetIndex(x, y)] = m_smoothMap[GetIndex(x, y)];
			}
		}
	}


	bool result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::Faulting(ID3D11Device* device, bool keydown, int faultValue)
{
	int index;

	D3DXVECTOR3 lineA, lineB, crossResult;

	float randPointX = rand() % 100, randPointY = rand() % 100, ranGradX = rand() % 2 - 0.5, randGradY = rand() % 2 - 0.5;

	//Random fault line
	lineA = D3DXVECTOR3(ranGradX, randGradY, 0.0);

	if (keydown)
	{
		for (int x = 0; x < m_terrainWidth; x++)
		{
			for (int y = 0; y < m_terrainHeight; y++)
			{
				index = (m_terrainHeight * y) + x;

				//Point being checked minus random point
				lineB = D3DXVECTOR3(x - randPointX, y - randPointY, 0.0);

				//Cross result of the two
				D3DXVec3Cross(&crossResult, &lineA, &lineB);

				//Raise and lower terrain depending on cross result
				if (crossResult.z > 0)
				{
					m_heightMap[index].y += faultValue;
				}
				else
				{
					m_heightMap[index].y -= faultValue;
				}
			}
		}
	}
}

//Following 4 functions are implemented from improved Perlin Noise site
float TerrainClass::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float TerrainClass::Lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float TerrainClass::Grad(int hash, float x, float y, float z)
{
	int h = hash & 15;

	float u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float TerrainClass::PerlinNoise(float x, float y, float z)
{
	int X = floor(x), Y = floor(y), Z = floor(z);

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = Fade(x), v = Fade(y), w = Fade(z);
#
	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
		B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
		Grad(p[BA], x - 1, y, z)),
		Lerp(u, Grad(p[AB], x, y - 1, z),
			Grad(p[BB], x - 1, y - 1, z))),
		Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
			Grad(p[BA + 1], x - 1, y, z - 1)),
			Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
				Grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;


	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
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

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

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

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k += 3;
		}
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}


void TerrainClass::NormalizeHeightMap()
{
	int i, j;


	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}


bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}


void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool TerrainClass::LoadTextures(ID3D11Device* device, WCHAR* grassTextureFilename, WCHAR* slopeTextureFilename, WCHAR* rockTextureFilename, WCHAR* snowTextureFilename)
{
	bool result;

	// Create the grass texture object.
	grassTexture = new TextureClass;
	if (!grassTexture)
	{
		return false;
	}

	// Initialize the grass texture object.
	result = grassTexture->Initialize(device, grassTextureFilename);
	if (!result)
	{
		return false;
	}

	// Create the slope texture object.
	slopeTexture = new TextureClass;
	if (!slopeTexture)
	{
		return false;
	}

	// Initialize the slope texture object.
	result = slopeTexture->Initialize(device, slopeTextureFilename);
	if (!result)
	{
		return false;
	}

	// Create the rock texture object.
	rockTexture = new TextureClass;
	if (!rockTexture)
	{
		return false;
	}

	// Initialize the rock texture object.
	result = rockTexture->Initialize(device, rockTextureFilename);
	if (!result)
	{
		return false;
	}

	// Create the grass texture object.
	snowTexture = new TextureClass;
	if (!snowTexture)
	{
		return false;
	}

	// Initialize the grass texture object.
	result = snowTexture->Initialize(device, snowTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::ReleaseTextures()
{
	// Release the texture objects.
	if (grassTexture)
	{
		grassTexture->Shutdown();
		delete grassTexture;
		grassTexture = 0;
	}

	if (slopeTexture)
	{
		slopeTexture->Shutdown();
		delete slopeTexture;
		slopeTexture = 0;
	}

	if (rockTexture)
	{
		rockTexture->Shutdown();
		delete rockTexture;
		rockTexture = 0;
	}

	if (snowTexture)
	{
		snowTexture->Shutdown();
		delete snowTexture;
		snowTexture = 0;
	}

	return;
}



bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

															 // Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index3].tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index2].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}