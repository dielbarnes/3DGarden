//
// ParticleSystem.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 39: Particle Systems (http://www.rastertek.com/dx11tut39.html)
//

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

using namespace DirectX;

struct ParticleVertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;
	XMFLOAT4 color;
};

struct Particle
{
	float x, y, z;
	float red, green, blue;
	float velocity;
	bool isActive;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	bool Initialize(ID3D11Device* device);
	bool Update(float fFrameTime, ID3D11DeviceContext* immediateContext);
	void Render(ID3D11DeviceContext* immediateContext);

	void SetTexture(ID3D11ShaderResourceView &texture);
	ID3D11ShaderResourceView** GetTexture();
	int GetIndexCount();
	void SetWorldMatrix(XMMATRIX worldMatrix);
	XMMATRIX GetWorldMatrix();

private:
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11Buffer* m_pVertexBuffer;
	ParticleVertex* m_vertices;
	int m_iVertexCount;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;
	Particle* m_particles;
	XMMATRIX m_worldMatrix;
	float m_fParticleDeviationX;
	float m_fParticleDeviationY;
	float m_fParticleDeviationZ;
	float m_fParticleVelocity;
	float m_fParticleVelocityVariation;
	float m_fParticleSize;
	float m_fParticlesPerSecond;
	int m_iMaxParticles;
	int m_iCurrentParticleCount;
	float m_fAccumulatedTime;

	void EmitParticles(float fFrameTime);
	void UpdateParticles(float fFrameTime);
	void KillParticles();
};

#endif
