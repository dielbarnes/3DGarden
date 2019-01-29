//
// ParticleShader.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef PARTICLE_SHADER_H
#define PARTICLE_SHADER_H

#include "Shader.h"
#include "ParticleSystem.h"

class ParticleShader : Shader
{
public:
	ParticleShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ParticleShader();

	HRESULT Initialize();
	bool Render(ParticleSystem *pParticleSystem, Camera* pCamera);

private:
	ID3D11SamplerState* m_pSamplerState;
};

#endif
