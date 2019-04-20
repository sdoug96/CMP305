////////////////////////////////////////////////////////////////////////////////
// Filename: cylinder.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CYLINDERCLASS_H_
#define _CYLINDERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: cylinderclass
////////////////////////////////////////////////////////////////////////////////
class cylinderclass
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	cylinderclass();
	cylinderclass(const cylinderclass&);
	~cylinderclass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	D3DXMATRIX transform;


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif