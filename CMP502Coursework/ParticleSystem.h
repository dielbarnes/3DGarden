#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3d11.h>
#include <directxmath.h>
#include "DDSTextureLoader.h"
#include "Utils.h"

using namespace DirectX;

struct Particle
{
	float positionX, positionY, positionZ;
	float red, green, blue;
	float velocity;
	bool active;
};

struct ParticleVertex
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT4 color;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext *immediateContext, int i);
	bool Update(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext *immediateContext, int i);
	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

private:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	ID3D11ShaderResourceView* m_Texture;
	Particle* m_particleList;
	int m_vertexCount, m_indexCount;
	ParticleVertex* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
};

#endif
