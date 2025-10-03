#pragma once
#include "BaseMesh.h"

struct ProceduralMeshData
{
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;
};

class ProceduralMeshA :public BaseMesh
{

public:
	bool isTangentMesh = false;


	ProceduralMeshA(
		ID3D11Device* device,
		ID3D11DeviceContext* deviceContext,
		std::vector<VertexType> vertices,
		std::vector<unsigned long> indices) :
		BaseMesh( device, deviceContext)
	{
		this->vertices = vertices;
		this->indices = indices;
		vertexCount = vertices.size();
		indexCount = indices.size();
		initBuffers(device);
	}
	ProceduralMeshA(
		ID3D11Device* device,
		ID3D11DeviceContext* deviceContext,
		BaseMesh* mesh) :
		BaseMesh( device, deviceContext)
	{
		this->vertices = mesh->vertices;
		this->indices = mesh->indices;
		vertexCount = vertices.size();
		indexCount = indices.size();
		initBuffers(device);
	}

    void initBuffers(ID3D11Device*) override
    {
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;


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
     }
};

