////////////////////////////////////////////////////////////////////////////////
// Filename: cylinderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cylinderclass.h"

cylinderclass::cylinderclass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	D3DXMatrixTranslation(&transform, 0.f, 0.f, 0.f);
	D3DXMatrixRotationYawPitchRoll(&transform, 0.f, 0.f, 0.0f);
}


cylinderclass::cylinderclass(const cylinderclass& other)
{
}


cylinderclass::~cylinderclass()
{
}

bool cylinderclass::Initialize(ID3D11Device* device)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void cylinderclass::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void cylinderclass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int cylinderclass::GetIndexCount()
{
	return m_indexCount;
}

bool cylinderclass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	D3DXVECTOR3* outerPoints;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	int sections = 25; //Number of sections/triangles in cylinder base
	const float pi = 3.14159265359; //pi
	float height = 5; //Height of cylinder

	// Set the number of vertices in the vertex array.
	m_vertexCount = ((3 * sections) * 2) + (sections * 6);

	// Set the number of indices in the index array.
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

	outerPoints = new D3DXVECTOR3[sections];

	float deltaTheta = (2 * pi) / sections; //Chnage in theta for each vertex

	D3DXVECTOR3 centrePoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//Render bottom circle
	for (int i = 0; i < sections; i++)
	{
		float theta = i * deltaTheta; //theta is the angle for that triangle
		int index = 3 * i;

		// Load the vertex array with data.
		//Given an angle theta, cos will give you the x coordinate and sin will give you the y coordinate
		vertices[index + 0].position = centrePoint;
		vertices[index + 0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 0] = index + 0;

		vertices[index + 1].position = centrePoint + D3DXVECTOR3(cos(theta), 0.0f, sin(theta));
		vertices[index + 1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 1] = index + 1;
		outerPoints[i] = centrePoint + D3DXVECTOR3(cos(theta), 0.0f, sin(theta));

		vertices[index + 2].position = centrePoint + D3DXVECTOR3(cos(theta + deltaTheta), 0.0f, sin(theta + deltaTheta));
		vertices[index + 2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 2] = index + 2;
	}

	//Render top circle
	for (int i = 0; i < sections; i++)
	{
		float theta = i * deltaTheta; //theta is the angle for that triangle
		int index = (3 * i) + (sections * 3);

		// Load the vertex array with data.
		//Given an angle theta, cos will give you the x coordinate and sin will give you the y coordinate
		vertices[index + 0].position = centrePoint + D3DXVECTOR3(0.0f, height, 0.0f);
		vertices[index + 0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 0] = index + 0;

		vertices[index + 1].position = centrePoint + D3DXVECTOR3(cos(theta + deltaTheta), height, sin(theta + deltaTheta));
		vertices[index + 1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 1] = index + 1;

		vertices[index + 2].position = centrePoint + D3DXVECTOR3(cos(theta), height, sin(theta));
		vertices[index + 2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 2] = index + 2;
	}

	//TODO Render walls
	for (int i = 0; i < sections; i++)
	{
		float theta = i * deltaTheta; //theta is the angle for that triangle
		int index = (6 * i) + (sections * 3 * 2);

		// Load the vertex array with data.
		//Given an angle theta, cos will give you the x coordinate and sin will give you the y coordinate
		vertices[index + 0].position = outerPoints[i];
		vertices[index + 0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 0] = index + 0;

		vertices[index + 1].position = outerPoints[i] + D3DXVECTOR3(0.0f, height, 0.0f);
		vertices[index + 1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 1] = index + 1;

		vertices[index + 2].position = outerPoints[(i + 1) % sections];
		vertices[index + 2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 2] = index + 2;

		vertices[index + 3].position = outerPoints[i] + D3DXVECTOR3(0.0f, height, 0.0f);
		vertices[index + 3].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 3] = index + 3;

		vertices[index + 4].position = outerPoints[(i + 1) % sections] + D3DXVECTOR3(0.0f, height, 0.0f);
		vertices[index + 4].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 4] = index + 4;

		vertices[index + 5].position = outerPoints[(i + 1) % sections];
		vertices[index + 5].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		indices[index + 5] = index + 5;
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

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void cylinderclass::ShutdownBuffers()
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

void cylinderclass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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