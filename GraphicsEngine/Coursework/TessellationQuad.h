#pragma once
#include "BaseMesh.h"
class TessellationQuad :
	public BaseMesh
{

private:
	float resolutionParams;
	int size=1;
public:
	// initialise buffers and load texture.
	TessellationQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext,int size =100, int subdivisions=5);

	// Release resources.
	~TessellationQuad();
	
	// Build triangle (with texture coordinates and normals).
	void initBuffers(ID3D11Device* device);


	void initBuffers(ID3D11Device* device, int size, int subdivisions)
	{
//		VertexType* vertices;
//		unsigned long* indices;
		int index, i, j;
		float positionX, positionZ, u, v, increment;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;

		//float dim = subdivisions * size;
		// Calculate the number of vertices in the terrain mesh.
		//vertexCount = (resolution - 1) * (resolution - 1) * 6;
		//We are doing quads
		vertexCount = (subdivisions) * (subdivisions)* 4;


		indexCount = vertexCount;
		vertices.resize(vertexCount);
		indices.resize(indexCount);
		//vertices = new VertexType[vertexCount];
		//indices = new unsigned long[indexCount];


		index = 0;
		// UV coords.
		u = 0;
		v = 0;
		increment = 1.0f / (float)subdivisions;
		float fres = (float) subdivisions / (float)size;
		float hres = (float) subdivisions/2.0f;

		for (j = 0; j < subdivisions ; j++)
		{
			for (i = 0; i <subdivisions; i++)
			{

				// upper left
				positionX = ((float)(i)-hres)/fres;
				positionZ = ((float)(j + 1)-hres)/fres;


				vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				vertices[index].texture = XMFLOAT2(u, v + increment);
				vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				indices[index] = index;
				index++;

				// bottom left
				positionX = ((float)i - hres)/fres;
				positionZ = ((float)(j)-hres)/fres;

				vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				vertices[index].texture = XMFLOAT2(u, v);
				vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				indices[index] = index;
				index++;


				// Bottom right
				positionX = ((float)(i + 1)-hres)/fres;
				positionZ = ((float)(j)-hres)/fres;

				vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				vertices[index].texture = XMFLOAT2(u + increment, v);
				vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				indices[index] = index;
				index++;

				// Upper right.
				positionX = ((float)(i + 1) - hres)/fres;
				positionZ = ((float)(j + 1) - hres)/fres;

				vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				vertices[index].texture = XMFLOAT2(u + increment, v + increment);
				vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				indices[index] = index;
				index++;

				


				//// lower left
				//positionX = (float)(i);
				//positionZ = (float)(j + 1);


				//vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				//vertices[index].texture = XMFLOAT2(u, v + ssrWorldLength);
				//vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				//indices[index] = index;
				//index++;

				//// Upper left
				//positionX = (float)(i);
				//positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				//vertices[index].texture = XMFLOAT2(u, v);
				//vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				//indices[index] = index;
				//index++;

				//// Bottom right
				//positionX = (float)(i + 1);
				//positionZ = (float)(j);

				//vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				//vertices[index].texture = XMFLOAT2(u + ssrWorldLength, v);
				//vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				//indices[index] = index;
				//index++;


			

				//// Upper right.
				//positionX = (float)(i + 1);
				//positionZ = (float)(j + 1);

				//vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
				//vertices[index].texture = XMFLOAT2(u + ssrWorldLength, v + ssrWorldLength);
				//vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
				//indices[index] = index;
				//index++;

				u += increment;

			}

			u = 0;
			v += increment;
		}



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

		// Release the arrays now that the buffers have been created and loaded.
//		delete[] vertices;
//		vertices = 0;
//		delete[] indices;
//		indices = 0;
	}

	// Override sendData() to change topology type. Control point patch list is required for tessellation.
	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top) override;


};

