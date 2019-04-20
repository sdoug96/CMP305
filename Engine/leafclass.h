////////////////////////////////////////////////////////////////////////////////
// Filename: leafclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEAFCLASS_H_
#define _LEAFCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: leafclass
////////////////////////////////////////////////////////////////////////////////
class leafclass
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	leafclass();
	leafclass(const leafclass&);
	~leafclass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	D3DXMATRIX transform;
	D3DXVECTOR4 leafColour = D3DXVECTOR4(0.1333f, 0.545f, 0.1333f, 1.0f); //Colour of leaf
	float scaleFactor = 5; //Used to scale down size of leaf

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif