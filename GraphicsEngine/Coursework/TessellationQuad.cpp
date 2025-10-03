#include "TessellationQuad.h"

// initialise buffers and load texture.




TessellationQuad::TessellationQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext,int size , int subdivisions)
	:BaseMesh(device,deviceContext),resolutionParams(subdivisions), size(size)

{
	//initBuffers(device);
	initBuffers(device, size,subdivisions);
}

// Release resources.

TessellationQuad::~TessellationQuad()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build triangle (with texture coordinates and normals).

void TessellationQuad::initBuffers(ID3D11Device* device)
{
//	VertexType* vertices;
//	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = 4;
	indexCount = 4;


	vertices.resize(vertexCount);
	indices.resize(indexCount);
//	vertices = new VertexType[vertexCount];
//	indices = new unsigned long[indexCount];

	// Load the vertex array with data.


	vertices[0].position = XMFLOAT3(-1.0f, 0.0f, -1.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 0.0f, 1.0f);  // Top left.
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, 0.0f, 1.0f);  // Top right.
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[3].position = XMFLOAT3(1.0f, 0.0f, -1.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);


	indices[0] = 3;  // bottom left.
	indices[1] = 2;  // top right.
	indices[2] = 1;  // top left.
	indices[3] = 0;	// bottom left



					// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete[] vertices;
	//vertices = 0;
	//delete[] indices;
	//indices = 0;
}

// Override sendData() to change topology type. Control point patch list is required for tessellation.

void TessellationQuad::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top= D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(top);
}
