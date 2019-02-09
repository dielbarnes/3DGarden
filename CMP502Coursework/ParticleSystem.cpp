//
// ParticleSystem.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "ParticleSystem.h"

#pragma region Init

ParticleSystem::ParticleSystem()
{
	m_pVertexBuffer = nullptr;
	m_iVertexCount = 0;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_worldMatrix = XMMatrixIdentity();
	m_fParticleDeviationX = 2.0f;
	m_fParticleDeviationY = 0.3f;
	m_fParticleDeviationZ = 2.2f;
	m_fParticleVelocity = 1.0f;
	m_fParticleVelocityVariation = 0.2f;
	m_fParticleSize = 0.15f;
	m_fParticlesPerSecond = 500.0f;
	m_iMaxParticles = 10000;
	m_iCurrentParticleCount = 0;
	m_fAccumulatedTime = 0.0f;
}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

bool ParticleSystem::Initialize(ID3D11Device* device)
{
	// Initialize the particle array
	m_particles = new Particle[m_iMaxParticles];
	for (int i = 0; i < m_iMaxParticles; i++)
	{
		m_particles[i].isActive = false;
	}

	// Initialize the vertex and index arrays

	m_iVertexCount = m_iMaxParticles * 6;
	m_iIndexCount = m_iVertexCount;

	m_vertices = new ParticleVertex[m_iVertexCount];

	unsigned long* indices = new unsigned long[m_iIndexCount];
	for (int i = 0; i < m_iIndexCount; i++)
	{
		indices[i] = i;
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {}; // Describes the vertex buffer object to be created
	bufferDesc.ByteWidth = sizeof(ParticleVertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;							// Resource is accessible by both the GPU (read only) and the CPU (write only); a dynamic resource will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// Resource is mappable so that the CPU can change its contents

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Describes the actual data that will be copied to the vertex buffer during creation
	subresourceData.pSysMem = m_vertices;

	HRESULT result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create particles vertex buffer.", result);
		return result;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(unsigned long) * m_iIndexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;						// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				// Bind the buffer as an index buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;								// No CPU access is necessary

	subresourceData.pSysMem = indices;

	result = device->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create particles index buffer.", result);
		return result;
	}

	// Release
	SAFE_DELETE_ARRAY(indices);

	return true;
}

#pragma endregion

#pragma region Setters/Getters

void ParticleSystem::SetTexture(ID3D11ShaderResourceView &texture)
{
	m_pTexture = &texture;
}

ID3D11ShaderResourceView** ParticleSystem::GetTexture()
{
	return &m_pTexture;
}

int ParticleSystem::GetIndexCount()
{
	return m_iIndexCount;
}

void ParticleSystem::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

XMMATRIX ParticleSystem::GetWorldMatrix()
{
	return m_worldMatrix;
}

#pragma endregion

#pragma region Update

bool ParticleSystem::Update(float fFrameTime, ID3D11DeviceContext* immediateContext)
{
	KillParticles();
	EmitParticles(fFrameTime);
	UpdateParticles(fFrameTime);

	// Build the vertex array from the particle array (each particle is a quad made out of two triangles)
	memset(m_vertices, 0, sizeof(ParticleVertex) * m_iVertexCount);
	int index = 0;
	for (int i = 0; i < m_iCurrentParticleCount; i++)
	{
		// Triangle 1

		// Bottom left
		m_vertices[index].position = XMFLOAT3(m_particles[i].x - m_fParticleSize, m_particles[i].y - m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;

		// Top left
		m_vertices[index].position = XMFLOAT3(m_particles[i].x - m_fParticleSize, m_particles[i].y + m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;

		// Bottom right
		m_vertices[index].position = XMFLOAT3(m_particles[i].x + m_fParticleSize, m_particles[i].y - m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;

		// Triangle 2

		// Bottom right
		m_vertices[index].position = XMFLOAT3(m_particles[i].x + m_fParticleSize, m_particles[i].y - m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;

		// Top left
		m_vertices[index].position = XMFLOAT3(m_particles[i].x - m_fParticleSize, m_particles[i].y + m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;

		// Top right
		m_vertices[index].position = XMFLOAT3(m_particles[i].x + m_fParticleSize, m_particles[i].y + m_fParticleSize, m_particles[i].z);
		m_vertices[index].textureCoordinate = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particles[i].red, m_particles[i].green, m_particles[i].blue, 1.0f);
		index++;
	}

	// Update the dynamic vertex buffer with the new position of each particle

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Lock the vertex buffer so it can be written to
	HRESULT result = immediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the particles vertex buffer.", result);
		return false;
	}

	// Get a pointer to the vertex buffer data
	ParticleVertex* vertexBufferData = (ParticleVertex*)mappedResource.pData;

	// Copy the vertices into the vertex buffer
	memcpy(vertexBufferData, m_vertices, sizeof(ParticleVertex) * m_iVertexCount);

	// Unlock the vertex buffer
	immediateContext->Unmap(m_pVertexBuffer, 0);

	return true;
}

void ParticleSystem::EmitParticles(float fFrameTime)
{
	// Update the time
	m_fAccumulatedTime += fFrameTime;

	// Check if it is time to emit a new particle
	bool emitParticle = false;
	if (m_fAccumulatedTime > 1000.0f / m_fParticlesPerSecond)
	{
		m_fAccumulatedTime = 0.0f;
		emitParticle = true;
	}

	if (emitParticle && m_iCurrentParticleCount < m_iMaxParticles - 1)
	{
		// Emit one particle per frame

		m_iCurrentParticleCount++;

		// Randomize particle properties
		float x = (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleDeviationX;
		float y = (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleDeviationY;
		float z = (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleDeviationZ;
		float red = 255.0f / 255.0f; // (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float green = 204.0f / 255.0f; // (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float blue = 248.0f / 255.0f; // (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float velocity = m_fParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleVelocityVariation;

		// The particle array is sorted using Z depth since the particles need to be rendered from back to front for blending
		// Find where in the list the particle should be inserted
		int index = 0;
		bool found = false;
		while (!found)
		{
			if (!m_particles[index].isActive || m_particles[index].z < z)
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Copy the array over by one position from the index to make room for the new particle
		int i = m_iCurrentParticleCount;
		int j = i - 1;
		while (i != index)
		{
			m_particles[i].x = m_particles[j].x;
			m_particles[i].y = m_particles[j].y;
			m_particles[i].z = m_particles[j].z;
			m_particles[i].red = m_particles[j].red;
			m_particles[i].green = m_particles[j].green;
			m_particles[i].blue = m_particles[j].blue;
			m_particles[i].velocity = m_particles[j].velocity;
			m_particles[i].isActive = m_particles[j].isActive;
			i--;
			j--;
		}

		// Insert the new particle in the array
		m_particles[index].x = x;
		m_particles[index].y = y;
		m_particles[index].z = z;
		m_particles[index].red = red;
		m_particles[index].green = green;
		m_particles[index].blue = blue;
		m_particles[index].velocity = velocity;
		m_particles[index].isActive = true;
	}
}

void ParticleSystem::UpdateParticles(float fFrameTime)
{
	// Move particles downwards each frame
	for (int i = 0; i < m_iCurrentParticleCount; i++)
	{
		m_particles[i].y = m_particles[i].y - (m_particles[i].velocity * fFrameTime * 0.001f);
	}
}

void ParticleSystem::KillParticles()
{
	// Kill all the particles that have gone below a certain height range
	for (int i = 0; i < m_iMaxParticles; i++)
	{
		if (m_particles[i].isActive && m_particles[i].y < -3.0f)
		{
			m_particles[i].isActive = false;
			m_iCurrentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly
			for (int j = i; j < m_iMaxParticles - 1; j++)
			{
				m_particles[j].x = m_particles[j + 1].x;
				m_particles[j].y = m_particles[j + 1].y;
				m_particles[j].z = m_particles[j + 1].z;
				m_particles[j].red = m_particles[j + 1].red;
				m_particles[j].green = m_particles[j + 1].green;
				m_particles[j].blue = m_particles[j + 1].blue;
				m_particles[j].velocity = m_particles[j + 1].velocity;
				m_particles[j].isActive = m_particles[j + 1].isActive;
			}
		}
	}
}

#pragma endregion

#pragma region Render

void ParticleSystem::Render(ID3D11DeviceContext* immediateContext)
{
	UINT uiStrides = sizeof(ParticleVertex);
	UINT uiOffsets = 0;

	immediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStrides, &uiOffsets);
	immediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion
