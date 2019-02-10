//
// SkyPlane.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 11: Bitmap Clouds (http://www.rastertek.com/tertut11.html)
//

#include "SkyPlane.h"

#pragma region Init

SkyPlane::SkyPlane()
{
	m_pVertexBuffer = nullptr;
	m_iVertexCount = 0;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_worldMatrix = XMMatrixIdentity();
	m_textureTranslation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // Texture1 x & z coordinates, texture2 x & z coordinates
	m_textureTranslationSpeed = XMFLOAT4(0.0001f, 0.0f, 0.00007f, 0.0f);
	m_fBrightness = 0.35f;
}

SkyPlane::~SkyPlane()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

bool SkyPlane::Initialize(ID3D11Device* device)
{
	// Create the sky plane

	int iSkyPlaneResolution = 100;
	float fSkyPlaneWidth = 150.0f;
	float fSkyPlaneTop = 0.5f;
	float fSkyPlaneBottom = 0.0f;
	int iTextureRepeat = 4;

	float fQuadSize = fSkyPlaneWidth / (float)iSkyPlaneResolution; // Size of each quad on the sky plane
	float fRadius = fSkyPlaneWidth / 2.0f; // Radius of the sky plane
	float fConstant = (fSkyPlaneTop - fSkyPlaneBottom) / (fRadius * fRadius); // Height constant to increment by
	float fTextureDelta = (float)iTextureRepeat / (float)iSkyPlaneResolution; // Texture coordinate increment value

	m_iVertexCount = (iSkyPlaneResolution + 1) * (iSkyPlaneResolution + 1) * 6;
	m_iIndexCount = m_iVertexCount;

	m_modelData = new ModelData[(iSkyPlaneResolution + 1) * (iSkyPlaneResolution + 1)];

	for (int i = 0; i <= iSkyPlaneResolution; i++)
	{
		for (int j = 0; j <= iSkyPlaneResolution; j++)
		{
			int index = j * (iSkyPlaneResolution + 1) + i;

			float x = (-0.5f * fSkyPlaneWidth) + ((float)i * fQuadSize);
			float z = (-0.5f * fSkyPlaneWidth) + ((float)j * fQuadSize);

			m_modelData[index].x = x;
			m_modelData[index].y = fSkyPlaneTop - (fConstant * ((x * x) + (z * z)));
			m_modelData[index].z = z;

			m_modelData[index].tu = (float)i * fTextureDelta;
			m_modelData[index].tv = (float)j * fTextureDelta;
		}
	}
	
	// Load the model data into the vertex and index arrays

	SkyPlaneVertex* vertices = new SkyPlaneVertex[m_iVertexCount];
	unsigned long* indices = new unsigned long[m_iIndexCount];

	int index = 0;
	for (int i = 0; i < iSkyPlaneResolution; i++)
	{
		for (int j = 0; j < iSkyPlaneResolution; j++)
		{
			int index1 = j * (iSkyPlaneResolution + 1) + i;
			int index2 = j * (iSkyPlaneResolution + 1) + (i + 1);
			int index3 = (j + 1) * (iSkyPlaneResolution + 1) + i;
			int index4 = (j + 1) * (iSkyPlaneResolution + 1) + (i + 1);

			// Triangle 1

			// Top left
			vertices[index].position = XMFLOAT3(m_modelData[index1].x, m_modelData[index1].y, m_modelData[index1].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index1].tu, m_modelData[index1].tv);
			indices[index] = index;
			index++;

			// Top right
			vertices[index].position = XMFLOAT3(m_modelData[index2].x, m_modelData[index2].y, m_modelData[index2].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index2].tu, m_modelData[index2].tv);
			indices[index] = index;
			index++;

			// Bottom left
			vertices[index].position = XMFLOAT3(m_modelData[index3].x, m_modelData[index3].y, m_modelData[index3].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index3].tu, m_modelData[index3].tv);
			indices[index] = index;
			index++;

			// Triangle 2

			// Bottom left
			vertices[index].position = XMFLOAT3(m_modelData[index3].x, m_modelData[index3].y, m_modelData[index3].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index3].tu, m_modelData[index3].tv);
			indices[index] = index;
			index++;

			// Top right
			vertices[index].position = XMFLOAT3(m_modelData[index2].x, m_modelData[index2].y, m_modelData[index2].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index2].tu, m_modelData[index2].tv);
			indices[index] = index;
			index++;

			// Bottom right
			vertices[index].position = XMFLOAT3(m_modelData[index4].x, m_modelData[index4].y, m_modelData[index4].z);
			vertices[index].textureCoordinate = XMFLOAT2(m_modelData[index4].tu, m_modelData[index4].tv);
			indices[index] = index;
			index++;
		}
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {}; // Describes the vertex buffer object to be created
	bufferDesc.ByteWidth = sizeof(SkyPlaneVertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Describes the actual data that will be copied to the vertex buffer during creation
	subresourceData.pSysMem = vertices;

	HRESULT result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex buffer.", result);
		return false;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(unsigned long) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create index buffer.", result);
		return false;
	}

	// Release
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return true;
}

#pragma endregion

#pragma region Setters/Getters

void SkyPlane::SetTexture1(ID3D11ShaderResourceView &texture)
{
	m_pTexture1 = &texture;
}

ID3D11ShaderResourceView** SkyPlane::GetTexture1()
{
	return &m_pTexture1;
}

void SkyPlane::SetTexture2(ID3D11ShaderResourceView &texture)
{
	m_pTexture2 = &texture;
}

ID3D11ShaderResourceView** SkyPlane::GetTexture2()
{
	return &m_pTexture2;
}

int SkyPlane::GetIndexCount()
{
	return m_iIndexCount;
}

void SkyPlane::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

XMMATRIX SkyPlane::GetWorldMatrix()
{
	return m_worldMatrix;
}

float SkyPlane::GetTexture1TranslationX()
{
	return m_textureTranslation.x;
}

float SkyPlane::GetTexture1TranslationZ()
{
	return m_textureTranslation.y;
}

float SkyPlane::GetTexture2TranslationX()
{
	return m_textureTranslation.z;
}

float SkyPlane::GetTexture2TranslationZ()
{
	return m_textureTranslation.w;
}

float SkyPlane::GetBrightness()
{
	return m_fBrightness;
}

#pragma endregion

#pragma region Render

void SkyPlane::Render(ID3D11DeviceContext* immediateContext)
{
	// Increment the translation values to simulate moving clouds
	m_textureTranslation.x += m_textureTranslationSpeed.x; // Texture1 x coordinate
	m_textureTranslation.y += m_textureTranslationSpeed.y; // Texture1 z coordinate
	m_textureTranslation.z += m_textureTranslationSpeed.z; // Texture2 x coordinate
	m_textureTranslation.w += m_textureTranslationSpeed.w; // Texture2 z coordinate

	// Keep the values in the zero to one range
	if (m_textureTranslation.x > 1.0f) { m_textureTranslation.x -= 1.0f; }
	if (m_textureTranslation.y > 1.0f) { m_textureTranslation.y -= 1.0f; }
	if (m_textureTranslation.z > 1.0f) { m_textureTranslation.z -= 1.0f; }
	if (m_textureTranslation.w > 1.0f) { m_textureTranslation.w -= 1.0f; }
	
	UINT uiStrides = sizeof(SkyPlaneVertex);
	UINT uiOffsets = 0;

	immediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStrides, &uiOffsets);
	immediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion
