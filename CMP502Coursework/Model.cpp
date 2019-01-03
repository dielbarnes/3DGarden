//
// Model.cpp
// Copyright � 2018 Diel Barnes. All rights reserved.
//

#include "Model.h"

#pragma region Init

Model::Model()
{
	m_pVertexBuffer = nullptr;
	m_iVertexCount = 0;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_pInstanceBuffer = nullptr;
	m_iInstanceCount = 0;
	m_mWorld = XMMatrixIdentity();
}

Model::~Model()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_DELETE_ARRAY(m_modelData);
}

bool Model::InitializeBuffers(ID3D11Device* device, int iInstanceCount, Instance* instances)
{
	HRESULT result = S_OK;

	Vertex* vertices = new Vertex[m_iVertexCount];
	unsigned long* indices = new unsigned long[m_iIndexCount];

	// Load the model data into the vertex and index arrays
	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_modelData[i].x, m_modelData[i].y, m_modelData[i].z);
		vertices[i].texture = XMFLOAT2(m_modelData[i].tu, m_modelData[i].tv);
		vertices[i].normal = XMFLOAT3(m_modelData[i].nx, m_modelData[i].ny, m_modelData[i].nz);

		indices[i] = i;
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {}; // Describes the vertex buffer object to be created
	bufferDesc.ByteWidth = sizeof(Vertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;						// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;								// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Describes the actual data that will be copied to the vertex buffer during creation
	subresourceData.pSysMem = vertices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex buffer.", result);
		return result;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(unsigned long) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create index buffer.", result);
		return result;
	}

	m_iInstanceCount = iInstanceCount;

	if (iInstanceCount > 1)
	{
		// Create the instance buffer

		bufferDesc.ByteWidth = sizeof(Instance) * iInstanceCount;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		
		subresourceData.pSysMem = instances;

		result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pInstanceBuffer);
		if (FAILED(result))
		{
			Utils::ShowError("Failed to create instance buffer.", result);
			return result;
		}
	}

	// Release
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return result;
}

#pragma endregion

#pragma region Setters/Getters

void Model::SetTexture(ID3D11ShaderResourceView &texture)
{
	m_pTexture = &texture;
}

ID3D11ShaderResourceView** Model::GetTexture()
{
	return &m_pTexture;
}

void Model::SetVertexCount(int iCount)
{
	m_iVertexCount = iCount;
}

void Model::SetIndexCount(int iCount)
{
	m_iIndexCount = iCount;
}

int Model::GetIndexCount()
{
	return m_iIndexCount;
}

int Model::GetInstanceCount()
{
	return m_iInstanceCount;
}

void Model::SetModelData(ModelData* modelData)
{
	m_modelData = modelData;
}

XMMATRIX Model::GetWorldMatrix()
{
	return m_mWorld;
}

void Model::TransformWorldMatrix(XMMATRIX mTranslation, XMMATRIX mRotation, XMMATRIX mScaling)
{
	m_mWorld = m_mWorld * mTranslation * mRotation * mScaling;
}

#pragma endregion

#pragma region Render

void Model::Render(ID3D11DeviceContext* immediateContext)
{
	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

	if (m_iInstanceCount == 1)
	{
		UINT uiStrides = sizeof(Vertex);
		UINT uiOffsets = 0;

		immediateContext->IASetVertexBuffers(
							0,					// First input slot for binding
							1,					// Number of vertex buffers in the array
							&m_pVertexBuffer,
							&uiStrides,			// Stride is the size in bytes of the elements to be used from that vertex buffer (one stride for each vertex buffer in the array)
							&uiOffsets);		// Offset is the number of bytes between the first element of a vertex buffer and the first element that will be used (one offset for each vertex buffer in the array)
	}
	else
	{
		UINT strides[2];
		strides[0] = sizeof(Vertex);
		strides[1] = sizeof(Instance);

		UINT offsets[2];
		offsets[0] = 0;
		offsets[1] = 0;

		ID3D11Buffer* bufferPointers[2];
		bufferPointers[0] = m_pVertexBuffer;
		bufferPointers[1] = m_pInstanceBuffer;

		immediateContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	}

	immediateContext->IASetIndexBuffer(
						m_pIndexBuffer,
						DXGI_FORMAT_R32_UINT,	// 32-bit format that supports 32 bits for the red channel
						0);						// Offset in bytes from the start of the index buffer to the first index to use

	// Set the primitive topology (how the GPU obtains the three vertices it requires to render a triangle)
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion