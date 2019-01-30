//
// SkyDome.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "SkyDome.h"

#pragma region Init

SkyDome::SkyDome()
{
}

SkyDome::~SkyDome()
{
}

bool SkyDome::InitializeBuffers(ID3D11Device* device, int iInstanceCount, Instance* instances)
{
	HRESULT result = S_OK;

	SkyDomeVertex* vertices = new SkyDomeVertex[m_iVertexCount];
	unsigned long* indices = new unsigned long[m_iIndexCount];

	// Load the model data into the vertex and index arrays
	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_modelData[i].x, m_modelData[i].y, m_modelData[i].z);

		indices[i] = i;
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {}; // Describes the vertex buffer object to be created
	bufferDesc.ByteWidth = sizeof(SkyDomeVertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;						// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;								// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Describes the actual data that will be copied to the vertex buffer during creation
	subresourceData.pSysMem = vertices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create sky dome vertex buffer.", result);
		return result;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(unsigned long) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create sky dome index buffer.", result);
		return result;
	}

	m_iInstanceCount = iInstanceCount;

	// Release
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return result;
}

#pragma endregion

#pragma region Setters/Getters

void SkyDome::SetApexColor(XMFLOAT4 apexColor)
{
	m_apexColor = apexColor;
}

XMFLOAT4 SkyDome::GetApexColor()
{
	return m_apexColor;
}

void SkyDome::SetCenterColor(XMFLOAT4 centerColor)
{
	m_centerColor = centerColor;
}

XMFLOAT4 SkyDome::GetCenterColor()
{
	return m_centerColor;
}

#pragma endregion
